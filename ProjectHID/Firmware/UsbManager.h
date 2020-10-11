/********************************** (C) COPYRIGHT ******************************
* File Name		: UsbManager.h
* Author		: Antecer
* Version		: V1.0
* Date			: 2017/12/01
* Description	: USB事件处理程序
*******************************************************************************/

#ifndef	UsbManager_H
#define	UsbManager_H

#ifndef	ROM_CMD_PROG
#define	ROM_CMD_PROG	0x9A								// 定义Flash写入命令
#endif
#define ROM_STATUS_RECODE	ROM_STATUS^0x40					// 重定义Flash操作结果返回码(551系列最高位默认为0,558系列最高位默认为1)
#define	IAP_CODE_ADDR			(0x2800-1024*2)				// IAP程序起始地址，该地址至少比实际的IAP地址小4byte且最好为1KB对齐
#define ChipIdData0				(*(PUINT8C)(0x3FFC))		// ChipID最低字节
#define ChipIdData1				(*(PUINT8C)(0x3FFD))		// ChipID次低字节
#define ChipIdData2				(*(PUINT8C)(0x3FFE))		// ChipID次高字节
#define ChipIdData3				(*(PUINT8C)(0x3FFF))		// ChipID　高字节
#define ChipIdData4				(*(PUINT8C)(0x3FFB))		// ChipID保留
#define ChipIdData5				(*(PUINT8C)(0x3FFA))		// ChipID最高字节

struct UsbBuffer{
	UINT8 EP0[0x40];	// 端点0 OUT&IN 64byte收发共用缓冲区
	UINT8 EP4[0x80];	// 端点4 OUT&IN 64byte*2收发缓冲区
	UINT8 EP1[0x80];	// 端点1 OUT&IN 64byte*2收发缓冲区
	UINT8 EP2[0x80];	// 端点2 OUT&IN 64byte*2收发缓冲区
	UINT8 EP3[0x80];	// 端点3 OUT&IN 64byte*2收发缓冲区
}xdata Buffer _at_ 0x0000;
#define Ep0Buffer Buffer.EP0
#define Ep1Buffer Buffer.EP1
#define Ep2Buffer Buffer.EP2
#define Ep3Buffer Buffer.EP3
#define Ep4Buffer Buffer.EP4

#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer) 	// 定义Setup包结构体
#define HIDbReqType UsbSetupBuf->bRequestType
#define HIDbRequest UsbSetupBuf->bRequest
#define HIDwValueL UsbSetupBuf->wValueL
#define HIDwValueH UsbSetupBuf->wValueH
#define HIDwIndexL UsbSetupBuf->wIndexL
#define HIDwIndexH UsbSetupBuf->wIndexH
#define HIDwLengthL UsbSetupBuf->wLengthL
#define HIDwLengthH UsbSetupBuf->wLengthH

Bool	IsUsbShut		= false;				// USB断开标志
UINT8D	UsbConfig		= 0x00;					// USB配置模式位
UINT8D	DevStatus		= 0x00;					// USB设备状态位


/*******************************************************************************
* Function Name	:	RunCommand
* Description	:	指令处理函数
* Input			:	pFlash	操作数据{ UINT8操作命令 | UINT8数据长度 | UINT16操作地址 | UINT8*数据内容 }
* Return		:	返回操作结果代码
					0x00操作成功；0x01操作超时；0x02操作错误；0x40地址错误；0xDE数据长度错误；0xFF校验错误；0xE0命令错误
*******************************************************************************/
void RunCommand(UINT8* pFlash){
	UINT8D dataAddrL;				// 缓存数据地址
	UINT8D length;					// 缓存数据长度
	UINT8D i;						// 定义循环变量
	UINT8D ReCode;					// 操作结果代码

	ReCode		= 0x00;				// 设置默认操作结果代码
	dataAddrL	= pFlash[2];		// 缓存DataFlash地址
	length		= pFlash[1];		// 缓存DataFlash数据长度
	if(length > 60)					// 校验数据长度(不能超过60byte,因为缓冲区仅64byte)
	{
		pFlash[1] = 0xDE;
		return;
	}
	switch (pFlash[0])								// 分析操作命令
	{
		case 0x80:									// 读CodeFlash命令
		{
			// 设置Flash操作地址
			ROM_ADDR_L	= pFlash[2];				// 写入地址低位
			ROM_ADDR_H	= pFlash[3];				// 写入地址高位
			// 循环读取Flash数据
			for(i=0; i<length; ++i)
			{
				pFlash[4+i] = *(PUINT8C)ROM_ADDR;
				++ROM_ADDR;
			}
			break;
		}
		case 0x82:									// 写CodeFlash命令
		{
			// 关闭Flash写保护
			SAFE_MOD 	= 0x55;
			SAFE_MOD 	= 0xAA;
			GLOBAL_CFG	|= bCODE_WE | bDATA_WE;
			// 设置CodeFlash操作地址
			ROM_ADDR_L	= pFlash[2];			// 写入地址低位
			ROM_ADDR_H	= pFlash[3];			// 写入地址高位
			// 写入CodeFlash数据
			for(i=0; i<length; i+=2)
			{
				ROM_DATA_L	= pFlash[4+i];	// 写入数据低位
				ROM_DATA_H	= pFlash[5+i];	// 写入数据高位
				ROM_CTRL	= ROM_CMD_PROG;			// 执行CodeFlash写入操作
				ReCode = ROM_STATUS_RECODE;
				if(ReCode) break;					// 检查操作结果
				ROM_ADDR	+= 2;					// 写入成功后地址+2
			}
			// 启用Flash写保护
			SAFE_MOD	= 0x55;
			SAFE_MOD	= 0xAA;
			GLOBAL_CFG	&= ~(bCODE_WE | bDATA_WE);
			break;
		}
		case 0x90:									// 读DataFlash命令
		{
			ROM_ADDR_L  = pFlash[2];				// 写入地址低位
			ROM_ADDR_H	= 0xC0;						// 写入地址高位(CH551的Data地址范围是0xC000~0xC0FF,仅偶地址有效)
			pFlash += 4;
			for(i=0; i<length; ++i)
			{
				ROM_CTRL	= 0x8E;
				ReCode		= ROM_STATUS_RECODE;		// 获取操作结果代码
				if(ReCode) break;						// 检查操作结果
				*pFlash		= ROM_DATA_L;
				++pFlash;
				ROM_ADDR_L += 2;
			}
			break;
		}
		case 0x92:		// 写DataFlash(因为DataFlash仅支持偶地址存储1byte数据,所以写入数据时需要注意跳过奇地址)
		{
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			GLOBAL_CFG |= bCODE_WE | bDATA_WE;
			
			ROM_ADDR_H	= 0xC0;
			for(i=0; i<length; ++i)
			{
				ROM_ADDR_L	= dataAddrL; 				// 设置DataFlash地址
				ROM_DATA_L	= pFlash[4 + i] ;			// 设置DataFlash数据
				ROM_CTRL	= 0x9A;						// 写数据到DataFlash
				ReCode		= ROM_STATUS_RECODE;		// 获取操作结果代码
				if(ReCode) break;						// 检查操作结果
				dataAddrL	+= 2;
			}
			
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			GLOBAL_CFG &= ~(bCODE_WE | bDATA_WE);
			break;
		}
		case 0x1D:		// 获取芯片唯一ID码
		{
			pFlash[4] = ChipIdData0;
			pFlash[5] = ChipIdData1;
			pFlash[6] = ChipIdData2;
			pFlash[7] = ChipIdData3;
			pFlash[8] = ChipIdData4;
			pFlash[9] = ChipIdData5;
			break;
		}
		case 0xB1:		// 跳转到IAP命令[Bootload]
		{
			((void(code *)(void))IAP_CODE_ADDR)();
			break;
		}
		case 0xFA:		// 硬件SPI更新LCD数据
		{
			LCD_CS = 1;
			LCD_CS = 0;
			SPIMasterModeSet();	// 启用硬件SPI
			SPI_SendCMD(0x2A);	// 列地址设置(0-239)
			SPI_SendDAT(0x00);
			SPI_SendDAT(pFlash[2]);
			SPI_SendDAT(0x00);
			SPI_SendDAT(0xEF);
			SPI_SendCMD(0x2B);	// 行地址设置(0-239)
			SPI_SendDAT(0x00);
			SPI_SendDAT(pFlash[3]);
			SPI_SendDAT(0x00);
			SPI_SendDAT(0xEF);
			SPI_SendCMD(0x2C);	// 写LCD数据存储器
			for(i=4; length; --length)
			{
				SPI_SendDAT(pFlash[i]);
				++i;
			}
			SPI0_CTRL = 0x02;	// 关闭硬件SPI
			break;
		}
		case 0xFB:		// 软件SPI更新LCD数据
		{
			LCD_CS = 1;
			LCD_CS = 0;
			SPI_Send_CMD(0x2A);	// 列地址设置(0-239)
			SPI_Send_DAT(0x00);
			SPI_Send_DAT(pFlash[2]);
			SPI_Send_DAT(0x00);
			SPI_Send_DAT(0xEF);
			SPI_Send_CMD(0x2B);	// 行地址设置(0-239)
			SPI_Send_DAT(0x00);
			SPI_Send_DAT(pFlash[3]);
			SPI_Send_DAT(0x00);
			SPI_Send_DAT(0xEF);
			SPI_Send_CMD(0x2C);	// 写LCD数据存储器
			for(i=4; length; --length)
			{
				SPI_Send_DAT(pFlash[i]);
				++i;
			}
			break;
		}
		default:		// 未知命令[Error]
		{
			ReCode = 0xE0;
			break;
		}
	}
	pFlash[1] = ReCode;	// 返回操作结果命令，用于上位机校验
}

/*******************************************************************************
* Function Name	: USB_DeviceInit
* Description			: USB设备模式初始化函数
* Input						: None
* Return					: None
*******************************************************************************/
void USB_DeviceInit( void )
{
	UINT8 i		= 0xFF;
	USB_CTRL   	= 0x06;		// 强制复位USB协议处理器（从IAP到用户程序必须的步骤）
	while(--i);				// 等待USB协议处理器复位完成
	
    USB_DEV_AD	= 0x00;		// 设备地址初始化为0x00
	USB_CTRL	= 0x29;		// 0b0010 1001 总线信号12M全速模式，使能USB设备功能，启用内部上拉，使能DMA与DMA中断
	UDEV_CTRL	= 0x31;		// 0b0011 0001 使能USB接收，禁用DP&DM下拉，物理端口12M全速模式，使能USB物理端口
	
    UEP0_DMA	= Ep0Buffer;							// 端点0 数据传输地址(端点0单64byte收发共用缓冲区)
    UEP0_CTRL	= UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点0 IN返回NAK，OUT返回ACK
    UEP4_CTRL	= UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点4 IN返回NAK，OUT返回ACK (注：端点4不支持同步触发位自动翻转功能)
    UEP4_1_MOD	= 0x4C;								// 端点0单64byte收发共用缓冲区，端点4 64byte接收缓冲区&64byte发送缓冲区

	UEP1_DMA	= Ep1Buffer;										// 端点1 数据传输地址
    UEP1_CTRL	= bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点1 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP4_1_MOD	|= 0xC0;											// 端点1 接收&发送使能
	
    UEP2_DMA	= Ep2Buffer;										// 端点2 数据传输地址
    UEP2_CTRL	= bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点2 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP2_3_MOD	|= 0x0C;											// 端点2 接收&发送使能
	
    UEP3_DMA	= Ep3Buffer;										// 端点3 数据传输地址
    UEP3_CTRL	= bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点3 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP2_3_MOD	|= 0xC0;											// 端点3 接收&发送使能
	
	USB_INT_EN	|= 0x03;	// 禁止[SOF&NAK&FIFO]中断；使能[总线挂起或唤醒&传输完成&总线复位]中断
    USB_INT_FG	= 0xFF;		// 清中断标志(高3位只读,低5位写1清零)
    IE_USB		= 1;		// 使能USB总中断
}
/*******************************************************************************
* Function Name	: HID_DeviceInterrupt
* Description	: HID中断查询函数
* Input			: None
* Return		: None
*******************************************************************************/
void HID_DeviceInterrupt(void) interrupt INT_NO_USB
{
	static UINT8	SetupRequest;		// 缓存USB描述符的请求类型
	static UINT8	SetupLength;		// 缓存USB描述符的数据长度
	static PUINT8C	pDescr;				// 缓存USB待发送数据指针
    static UINT8	length;				// 缓存USB待发送数据长度
	
	IsUsbShut = false;
    if(UIF_TRANSFER)																// USB传输完成中断标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))						// 判断此次中断是由哪个端点发起的
        {
			case UIS_TOKEN_IN  | 1:													// 端点1 上传
				UEP1_T_LEN = 0;
				UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
				break;
			case UIS_TOKEN_IN  | 2:													// 端点2 上传
				UEP2_T_LEN = 0;
				UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
				break;
			case UIS_TOKEN_IN  | 3:													// 端点3 上传
				UEP3_T_LEN = 0;
				UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
				break;
			case UIS_TOKEN_IN  | 4:													// 端点4 上传
				UEP4_T_LEN = 0;
				UEP4_CTRL ^= bUEP_T_TOG;											// 翻转同步标志
				UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
				break;
			case UIS_TOKEN_OUT | 1:													// 端点1 下传
				RunCommand(Ep1Buffer);												// 处理数据
				memcpy(Ep1Buffer+0x40, Ep1Buffer, 64);								// 回传操作结果
				UEP1_T_LEN = 0x40;
				UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;			// 有数据时上传数据并应答ACK
				break;
			case UIS_TOKEN_OUT | 2:													// 端点2 下传
				RunCommand(Ep2Buffer);												// 处理数据
				memcpy(Ep2Buffer+0x40, Ep2Buffer, 64);								// 回传操作结果
				UEP2_T_LEN = 0x40;
				UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;			// 有数据时上传数据并应答ACK
				break;
			case UIS_TOKEN_OUT | 3:													// 端点3 下传
				RunCommand(Ep3Buffer);												// 处理数据
				memcpy(Ep3Buffer+0x40, Ep3Buffer, 64);								// 回传操作结果
				UEP3_T_LEN = 0x40;
				UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;			// 有数据时上传数据并应答ACK
				break;
			case UIS_TOKEN_OUT | 4:													// 端点4 下传
				if(bUIS_TOG_OK) 													// 检查同步标志,抛弃不同步的数据
				{
					RunCommand(Ep4Buffer);											// 处理数据
					memcpy(Ep4Buffer+0x40, Ep4Buffer, 64);							// 回传操作结果
					UEP4_T_LEN = 0x40;
					UEP4_CTRL ^= bUEP_R_TOG; 										// 成功接收数据，翻转同步标志
					UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK;		// 有数据时上传数据并应答ACK
				}
				break;
			case UIS_TOKEN_SETUP | 0:												// SETUP事务,用于USB设备初始化
			{
				if(USB_RX_LEN != 8)													// 检测SETUP包长度是否正确
				{
					length = 0xFF;
				}
				else
				{
					length = 0;														// 默认为成功并且上传0长度
					SetupRequest = HIDbRequest;										// 获取bRequest
					SetupLength= HIDwLengthH||SetupLength&0x80 ? 0x7F : HIDwLengthL;// 限制SETUP包总长度
					if ((HIDbReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD ) // 只支持标准请求
					{
						switch( SetupRequest ) 
						{
							case HID_GET_REPORT:
								length = 1;	
								HIDbReqType = DevStatus;
								break;
							case HID_GET_IDLE:
								break;	
							case HID_GET_PROTOCOL:
								break;				
							case HID_SET_REPORT:
								break;
							case HID_SET_IDLE:
								break;	
							case HID_SET_PROTOCOL:
								break;
							default:/*意外的请求描述符*/
								length = 0xFF;
								break;
						}
					}
					else
					{																// 标准请求包处理
						switch(SetupRequest)										// 识别请求码
						{
							case USB_GET_DESCRIPTOR:								// 主机获取设备描述符
							{
								switch(HIDwValueH)
								{
									case 1:											//设备描述符
										pDescr = DevReport;							//把设备描述符送到要发送的缓冲区
										length = sizeof(DevReport);
										break;
									case 2:											//配置描述符
										pDescr = CfgReport;							//把配置描述符送到要发送的缓冲区
										length = sizeof(CfgReport);
										break;
									case 3:											//字符串描述符
										if(HIDwValueL < sizeof(StringReports))
										{
											pDescr	= *(StringReports + HIDwValueL);
										}
										else
										{
											pDescr = *StringReports;
										}
										length	= *pDescr;
										break;
									case 0x22:              						//端点描述符
										pDescr = HidReport; 						//把报表描述符送到要发送的缓冲区
										length = sizeof(HidReport);
										break;
									default:
										length = 0xFF;
										break;
								}
								if (SetupLength > length )
								{
									SetupLength = length;    						//限制总长度
								}
								length = SetupLength >= 8 ? 8 : SetupLength;  		//本次传输长度
								memcpy(Ep0Buffer, pDescr, length);      			//加载上传数据
								SetupLength -= length;
								pDescr += length;
								break;
							}
							case USB_SET_ADDRESS:									// 主机为设备分配地址
								SetupLength = HIDwValueL;    						// 暂存USB设备地址
								break;
							case USB_GET_CONFIGURATION:								// 主机获取设备配置
								Ep0Buffer[0] = UsbConfig;
								if (SetupLength >= 1) length = 1;
								break;
							case USB_SET_CONFIGURATION:								// 主机为设备设置配置
								UsbConfig = HIDwValueL;
								break;
							case USB_GET_INTERFACE:
								break;
							case USB_CLEAR_FEATURE:                    				// 清除设备状态
							{
								if ( ( HIDbReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
								{
								   switch( HIDwIndexL )
								   {
									  case 0x81:
										   UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										   break;
									  case 0x01:
										   UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_T_RES ) | UEP_R_RES_ACK;
										   break;
									  case 0x82:
										   UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										   break;
									  case 0x02:
										   UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_T_RES ) | UEP_R_RES_ACK;
										   break;
									  case 0x83:
										   UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										   break;
									  case 0x03:
										   UEP3_CTRL = UEP3_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_T_RES ) | UEP_R_RES_ACK;
										   break;
									  case 0x84:
										   UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										   break;
									  case 0x04:
										   UEP4_CTRL = UEP4_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
										   break;
									  default:
										   length= 0xFF;
										   break;
									}
								 }
								 else
								 {
									length = 0xFF;
								 }
								break;
							 }
							case USB_SET_FEATURE:                       	// 配置设备状态
							{
								if((HIDbReqType & 0x1F ) == 0x00 )			// 设置设备
								{
									if((((UINT16)HIDwValueH << 8 ) | HIDwValueL ) == 0x01 )
									{
										if(CfgReport[7] & 0x20 )
										{
											// 设置唤醒使能标志
										}
										else
										{
											length = 0xFF;                  					// 操作失败
										}
									}
									else
									{
										length = 0xFF;                     					// 操作失败
									}
								}
								else if((HIDbReqType & 0x1F ) == 0x02 )	// 设置端点
								{
									if((((UINT16)HIDwValueH << 8 ) | HIDwValueL ) == 0x00 )
									{
										switch(((UINT16)HIDwIndexH << 8 ) | HIDwIndexL )
										{
										case 0x81:
											UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点1 IN STALL
											break;
										case 0x01:
											UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_R_RES_STALL;// 设置端点1 OUT STALL
											break;
										case 0x82:
											UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点1 IN STALL
											break;
										case 0x02:
											UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_R_RES_STALL;// 设置端点1 OUT STALL
											break;
										case 0x83:
											UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点1 IN STALL
											break;
										case 0x03:
											UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_R_RES_STALL;// 设置端点1 OUT STALL
											break;
										case 0x84:
											UEP4_CTRL = UEP4_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点2 IN STALL
											break;
										case 0x04:
											UEP4_CTRL = UEP4_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;// 设置端点2 OUT STALL
											break;
										default:
											length = 0xFF;                  // 操作失败
											break;
										}
									}
									else
									{
										length = 0xFF;                      // 操作失败
									}
								}
								else
								{
									length = 0xFF;                          // 操作失败
								}
								break;
							}
							case USB_GET_STATUS:				// 获取设备状态
								Ep0Buffer[0] = 0x00;
								Ep0Buffer[1] = 0x00;
								length = SetupLength >= 2 ? 2 : SetupLength;
								break;
							default:
								length = 0xFF;								// 操作失败
								break;
						}
					}
				}
				
				if(length == 0xFF)
				{
					SetupRequest = 0xFF;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
				}
				else if(length <= 8)				// 上传数据或者状态阶段返回0长度包
				{
					UEP0_T_LEN = length;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
				}
				else
				{
					UEP0_T_LEN = 0;			// 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
				}
				break;
			}
			case UIS_TOKEN_IN  | 0:													// 端点0 上传
				switch(SetupRequest)												// 分析描述符类型
				{
					case USB_GET_DESCRIPTOR:
						length = SetupLength >= 8 ? 8 : SetupLength;				// 本次传输长度
						memcpy(Ep0Buffer, pDescr, length );							// 加载上传数据
						SetupLength -= length;
						pDescr += length;
						UEP0_T_LEN = length;
						UEP0_CTRL ^= bUEP_T_TOG;									// 同步标志位翻转
						break;
					case USB_SET_ADDRESS:
						USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLength;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
						UEP0_T_LEN = 0;												// 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
			case UIS_TOKEN_OUT | 0:													// 端点0 下传
				if(SetupRequest == HID_SET_REPORT)
				{
					DevStatus = HIDbReqType;										// 保存设备状态
				}
				UEP0_CTRL ^= bUEP_R_TOG;											// 同步标志位翻转
				break;
			default:
				break;
        }
        UIF_TRANSFER = 0;															// 清除USB传输完成中断标志
    }
    else if (UIF_BUS_RST)                                                           // USB总线复位中断
    {
        USB_DEV_AD = 0x00;															// USB总线复位，重置设备地址
		USB_INT_FG = 0xFF;                                                          // 总线复位，清除全部中断标志
    }
	else if (UIF_SUSPEND)															// 总线挂起或唤醒中断
	{
		UIF_SUSPEND = 0;						// 清总线挂起或唤醒事件中断标志
		IsUsbShut = true;						// 当总线挂起时,将USB断开标志设置为true
	}
	else	// 这里清除前面未用的中断事件标志
	{
		UIF_FIFO_OV = 0;	// 清FIFO溢出中断标志
	}
}
#endif	/*UsbManager_H*/