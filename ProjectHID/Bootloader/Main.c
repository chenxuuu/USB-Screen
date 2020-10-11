/********************************** (C) COPYRIGHT ******************************
* Project Name	: CH551_IAP
* Author		: Antecer
* Version		: V1.0.0.5
* Date			: 2020年10月04日13点35分
* Description	: WCH芯片模拟HID供应商自定义设备，无需安装驱动
* Warning		: 注1.各种芯片的IAP地址不同，芯片变更后IAP_CODE_ADDR的跳转地址也要改
				  注2.在写BootLoader程序的时候尽量避免使用指针,因为会占用大量ROM空间
*******************************************************************************/
#include "string.h"
#include "CH552.h"
#include "UsbReports.h"

#ifndef	ROM_CMD_PROG
#define	ROM_CMD_PROG	0x9A							// 定义Flash写入命令
#endif
#define ROM_STATUS_RECODE		ROM_STATUS^0x40			// 重定义Flash操作结果返回码(551系列最高位默认为0,558系列最高位默认为1)

#define IAP_CODE_ADDR			(0x2800-1024*2)			// (0x2000)定义IAP代码边界，必须为1K整数倍,芯片擦除Flash一次至少1KB

#define ChipIdData0				(*(PUINT8C)(0x3FFC))	// ChipID最低字节
#define ChipIdData1				(*(PUINT8C)(0x3FFD))	// ChipID次低字节
#define ChipIdData2				(*(PUINT8C)(0x3FFE))	// ChipID次高字节
#define ChipIdData3				(*(PUINT8C)(0x3FFF))	// ChipID　高字节
#define ChipIdData4				(*(PUINT8C)(0x3FFB))	// ChipID保留
#define ChipIdData5				(*(PUINT8C)(0x3FFA))	// ChipID最高字节

#define	ROM_DATA_ADDR			(0x1C00)				// 定义代码数据边界(必须为1K整数倍,0x400=1K)，防止数据被擦除
sbit	IAP_LED			=		P3^4;					// 定义IAP状态指示灯，用于判断是否处于IAP模式
#define InitPort();		{P1=P3=0xFF;}					// 定义IO口初始化伪函数
						
UINT8	UsbConfig		=		0x00;					// USB配置属性选择位
UINT8X	Ep0Buffer[0xC0]	_at_	0x0000;					// 端点0 OUT&IN 64byte收发共用缓冲区 + 端点4 OUT&IN 64byte*2收发缓冲区
#define	UsbSetupBuf		((PUSB_SETUP_REQ)Ep0Buffer)		// 暂存Setup包 结构体指针
#define	HIDbReqType		UsbSetupBuf->bRequestType
#define	HIDbRequest		UsbSetupBuf->bRequest
#define	HIDwValueL		UsbSetupBuf->wValueL
#define	HIDwValueH		UsbSetupBuf->wValueH
#define	HIDwIndexL		UsbSetupBuf->wIndexL
#define	HIDwIndexH		UsbSetupBuf->wIndexH
#define	HIDwLengthL		UsbSetupBuf->wLengthL
#define	HIDwLengthH		UsbSetupBuf->wLengthH

#pragma NOAREGS
/*******************************************************************************
* Function Name	:	FlashCoding
* Description	:	CodeFlash写入函数
* Input			:	Ep4BufferOutput 操作数据{ UINT8操作命令 | UINT8数据长度 | UINT16操作地址 | UINT8*数据内容 }
* Return		:	返回操作结果代码
					0x00操作成功；0x02操作错误；0x40地址错误；0xDE数据长度错误；0xFF校验错误；0xE0未知命令
*******************************************************************************/
UINT8 FlashCoding( void ){
	UINT8	Length;									// 缓存数据长度
	UINT8	i;										// 定义循环变量
	
	Length	= Ep0Buffer[0x81];						// 获取数据长度
	if(Length > 60)	return 0xDE;					// 校验数据长度(不能超过60byte,因为缓冲区仅64byte)
	switch(Ep0Buffer[0x80])							// 分析操作命令
	{
		case 0x81:									// CodeFlash擦除命令(CH551-554系列不需要擦除Flash,这里直接跳过)
		{
			break;
		}
		case 0x82:									// CodeFlash写入命令
		{
			// 关闭Flash写保护
			SAFE_MOD 	= 0x55;
			SAFE_MOD 	= 0xAA;
			GLOBAL_CFG	|= bCODE_WE | bDATA_WE;
			// 设置CodeFlash操作地址
			ROM_ADDR_L	= Ep0Buffer[0x82];			// 写入地址低位
			ROM_ADDR_H	= Ep0Buffer[0x83];			// 写入地址高位
			// 分析操作地址是否符合要求(程序自我保护,防止被自我覆写)
			if (ROM_ADDR_H >= MSB(ROM_DATA_ADDR)) return 0x40;
			// 写入CodeFlash数据
			for(i=0; i<Length; i+=2)
			{
				ROM_DATA_L	= Ep0Buffer[0x84+i];	// 写入数据低位
				ROM_DATA_H	= Ep0Buffer[0x85+i];	// 写入数据高位
				ROM_CTRL	= ROM_CMD_PROG;			// 执行CodeFlash写入操作
				if(ROM_STATUS_RECODE)				// 检查操作结果
				{
					return ROM_STATUS_RECODE;		// 返回错误代码(0x40地址无效;0x02写入失败)
				}
				ROM_ADDR	+= 2;					// 写入成功后地址+2
			}
			// 启用Flash写保护
			SAFE_MOD	= 0x55;
			SAFE_MOD	= 0xAA;
			GLOBAL_CFG	&= ~(bCODE_WE | bDATA_WE);
			break;
		}
		case 0x83:									// 校验CodeFlash命令
		{
			ROM_ADDR_L	= Ep0Buffer[0x82];			// 写入地址低位
			ROM_ADDR_H	= Ep0Buffer[0x83];			// 写入地址高位
			for(i=0; i<Length; ++i)
			{
				if(Ep0Buffer[0x84+i] != *(PUINT8C)ROM_ADDR)
				{
					return 0xFF;
				}
				++ROM_ADDR;
			}
			break;
		}
		case 0x1B:									// 重启命令[LoadBoot],重启并进入用户程序
		{
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			GLOBAL_CFG |= bSW_RESET;
			break;
		}
		case 0x1D:									// 校验ID加密
		{
			Ep0Buffer[0x82] = CHIP_ID;				// 读芯片型号,命名规则CH5xx(这里获取后两位)
			Ep0Buffer[0x84] = ChipIdData0;
			Ep0Buffer[0x85] = ChipIdData1;
			Ep0Buffer[0x86] = ChipIdData2;
			Ep0Buffer[0x87] = ChipIdData3;
			Ep0Buffer[0x88] = ChipIdData4;
			Ep0Buffer[0x89] = ChipIdData5;
			break;
		}
		default:									// 未知命令[Error]
		{
			return 0xE0;
		}
	}
	return 0x00;
}
/*******************************************************************************
* Function Name	: HID_DeviceInterrupt
* Description	: HID中断查询函数 (IAP程序不支持中断，只能使用查询模式)
* Input         : None
* Output		: None
* Return		: None
*******************************************************************************/
void HID_DeviceInterrupt(void)
{
	static UINT8		SetupRequest;												// 缓存USB描述符的请求类型
	static UINT8		SetupLength;												// 缓存USB描述符的数据长度
	static PUINT8C		pDescr;														// 缓存USB待发送数据指针
    static UINT8		length;														// 缓存USB待发送数据长度
    if(UIF_TRANSFER)                                                            	// USB传输完成中断标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
			case UIS_TOKEN_IN  | 4:                                                	// 端点4 上传
				UEP4_T_LEN = 0;
				UEP4_CTRL = UEP4_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;           	// 默认应答NAK
				break;
			case UIS_TOKEN_OUT | 4:                                                 // 端点4 下传
				if (U_TOG_OK)                                                     	// 不同步的数据包将丢弃
				{
					memcpy(Ep0Buffer+0x80, Ep0Buffer+0x40, 64);						// 转存数据，数据从Ep0Buffer第0x40地址开始存放
					UEP4_CTRL ^= bUEP_R_TOG; 										// 成功接收数据，翻转端点4接收器同步标志位
					Ep0Buffer[0x81] = FlashCoding();								// 处理数据 (并将需要回传的数据写入Ep0Buffer+0x80发送缓冲区)
					UEP4_T_LEN = 64;
					UEP4_CTRL = UEP4_CTRL & 0x80 ? UEP4_CTRL&0x8F : UEP4_CTRL|0x40;	// 设置端点4发送器同步标志位
					UEP4_CTRL = UEP4_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;		// 有数据时上传数据并应答ACK
				}
				break;
			case UIS_TOKEN_SETUP | 0:                                               // SETUP事务,用于USB设备初始化
			{
				if(USB_RX_LEN != 8)													// 检测SETUP包长度是否正确
				{
					length = 0xFF;
				}
				else
				{
					length = 0;                                             		// 默认为成功并且上传0长度
					SetupRequest = HIDbRequest;										// 获取bRequest
					SetupLength= HIDwLengthH||SetupLength&0x80 ? 0x7F : HIDwLengthL;// 限制SETUP包总长度
					if ((HIDbReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD ) // 只支持标准请求
					{
						switch( SetupRequest ) 
						{
							case HID_GET_REPORT:
								length = 1;	
								Ep0Buffer[0] = 0xAA;										
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
						switch(SetupRequest)                                        // 识别请求码
						{
							case USB_GET_DESCRIPTOR:					//主机获取设备描述符
							{
								switch(HIDwValueH)
								{
									case 1:                 			//设备描述符
										pDescr = DevReport; 			//把设备描述符送到要发送的缓冲区
										length = sizeof(DevReport);
										break;
									case 2:                 			//配置描述符
										pDescr = CfgReport; 			//把配置描述符送到要发送的缓冲区
										length = sizeof(CfgReport);
										break;
									case 3:								//字符串描述符
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
									case 0x22:              			//报表描述符
										pDescr = HidReport; 			//把报表描述符送到要发送的缓冲区
										length = sizeof(HidReport);
										break;
									default:							//不支持的描述符
										length = 0xff;
										break;
								}
								if (SetupLength > length )
								{
									SetupLength = length;    			//限制总长度
								}
								length = SetupLength >= 8 ? 8 : SetupLength;  //本次传输长度
								memcpy(Ep0Buffer, pDescr, length);      //加载上传数据
								SetupLength -= length;
								pDescr += length;
								break;
							}
							case USB_SET_ADDRESS:						//主机为设备分配地址
								SetupLength = HIDwValueL;    			//暂存USB设备地址
								break;
							case USB_GET_CONFIGURATION:					//主机获取设备配置
								Ep0Buffer[0] = UsbConfig;
								if (SetupLength >= 1) length = 1;
								break;
							case USB_SET_CONFIGURATION:					//主机为设备设置配置
								UsbConfig = HIDwValueL;
								break;
							case USB_GET_INTERFACE:
								break;
							case USB_CLEAR_FEATURE:                     //清除设备状态
								if ( ( HIDbReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
								{
								   switch( HIDwIndexL )
								   {
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
							case USB_SET_FEATURE:                       	// 配置设备状态
							{
								if((HIDbReqType & 0x1F ) == 0x00 )			// 设置设备
								{
									if((((UINT16 ) HIDwValueH << 8 ) | HIDwValueL ) == 0x01 )
									{
										if(CfgReport[7] & 0x20 )
										{
											// 设置唤醒使能标志
										}
										else
										{
											length = 0xFF;                  // 操作失败
										}
									}
									else
									{
										length = 0xFF;                      // 操作失败
									}
								}
								else if((HIDbReqType & 0x1F ) == 0x02 )     // 设置端点
								{
									if((((UINT16)HIDwIndexH << 8 ) | HIDwIndexL ) == 0x00 )
									{
										switch(((UINT16)HIDwIndexH << 8 ) | HIDwIndexL )
										{
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
							case USB_GET_STATUS:
								Ep0Buffer[0] = 0x00;
								Ep0Buffer[1] = 0x00;
								length = SetupLength >= 2 ? 2 : SetupLength;
								break;
							default:
								length = 0xFF;								//操作失败
								break;
						}
					}
				}
				
				if(length == 0xFF)
				{
					SetupRequest = 0xFF;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
				}
				else if(length <= 8)		// 上传数据或者状态阶段返回0长度包
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
			case UIS_TOKEN_IN  | 0:												// 端点0 上传
				switch(SetupRequest)
				{
					case USB_GET_DESCRIPTOR:
						length = SetupLength >= 8 ? 8 : SetupLength;			// 本次传输长度
						memcpy(Ep0Buffer, pDescr, length );						// 加载上传数据
						SetupLength -= length;
						pDescr += length;
						UEP0_T_LEN = length;
						UEP0_CTRL ^= bUEP_T_TOG;								// 同步标志位翻转
						break;
					case USB_SET_ADDRESS:
						USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLength;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
						UEP0_T_LEN = 0;											// 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
			case UIS_TOKEN_OUT | 0:												// 端点0 下传
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_ACK;						//默认数据包是DATA0,返回应答ACK
				break;
			default:
				break;
        }
        UIF_TRANSFER = 0;														// 清除USB传输完成中断标志
    }
    else if (UIF_BUS_RST)														// USB总线复位中断
    {
        USB_DEV_AD = 0x00;														// USB总线复位，重置设备地址
		USB_INT_FG = 0xFF;														// 总线复位，清除全部中断标志
    }
	else	// 这里清除前面未用的中断事件标志
	{
		UIF_SUSPEND = 0;	// 清总线挂起或唤醒事件中断标志
		UIF_FIFO_OV = 0;	// 清FIFO溢出中断标志
	}
}
/*******************************************************************************
* Function Name	: main
* Description	: 主函数
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void main( void )
{
	UINT8 tick,chip;
    EA			= 0;								// 关闭单片机总中断，IAP方式无法使用中断(同时防止从用户程序跳转到IAP后触发中断)
	SAFE_MOD	= 0x55;								// 解锁安全模式
	SAFE_MOD	= 0xAA;
	GLOBAL_CFG	&= 0xFE;							// 关闭看门狗复位,防止IAP升级时看门狗溢出
	CLOCK_CFG	= 0x86;								// 系统时钟分频器4分频=24MHz
	// USB设备模式初始化
	USB_CTRL	= 0x06;								// 强制复位USB协议处理器（从用户程序到IAP必须的步骤）
	tick		= 0xFF;								
	while(--tick);									// 等待USB协议处理器复位完成
    USB_DEV_AD	= 0x00;								// 设备地址初始化为0x00 (从用户程序到IAP必须重置)
	USB_CTRL   	= 0x29;								// 0b00101001 配置USB控制寄存器
	UDEV_CTRL	= 0x31;								// 0b00110001 配置USB设备物理端口控制寄存器
    UEP0_DMA	= Ep0Buffer;						// 端点0与端点4 数据缓存地址
    UEP0_CTRL	= UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点0 IN返回NAK，OUT返回ACK
    UEP4_CTRL	= UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点4 IN返回NAK，OUT返回ACK (注：端点4不支持同步触发位自动翻转功能)
    UEP4_1_MOD	= 0x4C;								// 端点0单64byte收发共用缓冲区，端点4 64byte接收缓冲区&64byte发送缓冲区
	USB_INT_EN	= 0x00;								// 关闭全部中断使能位
    USB_INT_FG	= 0xFF;								// 清中断标志(高3位只读,低5位写1清零)
    IE_USB		= 0;								// 禁止USB总中断
	// 初始化全部IO口状态
	InitPort();
    while(1)
    {
		if(USB_INT_FG) HID_DeviceInterrupt();		// 查询USB中断标志
		if(--tick == 0 && --chip == 0) IAP_LED = ~IAP_LED;
    }
}
