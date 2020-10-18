/********************************** (C) COPYRIGHT ******************************
* Project Name	: CH551_ISP
* Author		: Antecer
* Version		: V1.0.0.5
* Date			: 2020年10月18日00点36分
* Description	: WCH芯片模拟USBCDC设备，无需安装驱动
* Warning		: 注1.各种芯片的ISP地址不同，芯片变更后IAP_CODE_ADDR的跳转地址也要改
				  注2.在写BootLoader程序的时候尽量避免使用指针,因为会占用大量ROM空间
*******************************************************************************/
#include "string.h"
#include "CH552.h"
#include "UsbDescriptor.h"

#ifndef	ROM_CMD_PROG
#define	ROM_CMD_PROG	0x9A								// 定义Flash写入命令
#endif

#define ROM_STATUS_RECODE		ROM_STATUS^0x40				// 重定义Flash操作结果返回码(551系列最高位默认为0,558系列最高位默认为1)
#define	IAP_CODE_ADDR			(0x2800-1024*2)				// IAP程序起始地址，该地址至少比实际的IAP地址小4byte且最好为1KB对齐
#define ChipIdData0				(*(PUINT8C)(0x3FFC))		// ChipID最低字节
#define ChipIdData1				(*(PUINT8C)(0x3FFD))		// ChipID次低字节
#define ChipIdData2				(*(PUINT8C)(0x3FFE))		// ChipID次高字节
#define ChipIdData3				(*(PUINT8C)(0x3FFF))		// ChipID　高字节
#define ChipIdData4				(*(PUINT8C)(0x3FFB))		// ChipID保留
#define ChipIdData5				(*(PUINT8C)(0x3FFA))		// ChipID最高字节

// USB数据缓冲区结构体
xdata struct _UsbBuffer{
	UINT8 EP0[0x40];	// 端点0 OUT&IN 64byte收发共用缓冲区
	UINT8 EP1[0x40];	// 端点1 IN 64byte发送缓冲区
	UINT8 EP2[0x80];	// 端点2 OUT&IN 64byte*2收发缓冲区
} Buffer _at_ 0x0000;
#define Ep0Buffer Buffer.EP0
#define Ep1Buffer Buffer.EP1
#define Ep2Buffer Buffer.EP2

#define UsbSetupBuf	((PUSB_SETUP_REQ)Ep0Buffer)	// 定义Setup包结构体
#define USBbReqType	(UsbSetupBuf->bRequestType)
#define USBbRequest	(UsbSetupBuf->bRequest)
#define USBwValueL	(UsbSetupBuf->wValueL)
#define USBwValueH	(UsbSetupBuf->wValueH)
#define USBwIndexL	(UsbSetupBuf->wIndexL)
#define USBwIndexH	(UsbSetupBuf->wIndexH)
#define USBwLengthL	(UsbSetupBuf->wLengthL)
#define USBwLengthH	(UsbSetupBuf->wLengthH)

// 定义IAP状态指示灯
sbit IAP_LED = P3^4;

/*******************************************************************************
* Function Name	:	FlashCoding
* Description	:	CodeFlash写入函数
* Input			:	Ep2BufferOutput 操作数据{ UINT8操作命令 | UINT8数据长度 | UINT16操作地址 | UINT8*数据内容 }
* Return		:	返回操作结果代码
					0x00操作成功；0x02操作错误；0x40地址错误；0xDE数据长度错误；0xFF校验错误；0xE0未知命令
*******************************************************************************/
UINT8 FlashCoding(){
	UINT8	Length;								// 缓存数据长度
	UINT8	i;									// 定义循环变量

	Length	= Ep2Buffer[0x41];					// 获取数据长度
	if(Length > 60) return 0xDE;				// 校验数据长度(不能超过60byte,因为缓冲区仅64byte)
	switch(Ep2Buffer[0x40])						// 分析操作命令
	{
		case 0x81:								// CodeFlash擦除命令(CH551-554系列不需要擦除Flash,这里直接跳过)
		{
			break;
		}
		case 0x82:								// CodeFlash写入命令
		{
			// 关闭Flash写保护
			SAFE_MOD 	= 0x55;
			SAFE_MOD 	= 0xAA;
			GLOBAL_CFG	|= bCODE_WE | bDATA_WE;
			// 设置CodeFlash操作地址
			ROM_ADDR_L	= Ep2Buffer[0x42];		// 写入地址低位
			ROM_ADDR_H	= Ep2Buffer[0x43];		// 写入地址高位
			// 分析操作地址是否符合要求(程序自我保护,防止被自我覆写)
			if (ROM_ADDR_H >= MSB(IAP_CODE_ADDR)) return 0x40;
			// 写入CodeFlash数据
			for(i=0; i<Length;i+=2)
			{
				ROM_DATA_L	= Ep2Buffer[0x44+i];// 写入数据低位
				ROM_DATA_H	= Ep2Buffer[0x45+i];// 写入数据高位
				ROM_CTRL	= ROM_CMD_PROG;		// 执行CodeFlash写入操作
				if(ROM_STATUS_RECODE)			// 检查操作结果
				{
					return ROM_STATUS_RECODE;	// 返回错误代码(0x40地址无效;0x02写入失败)
				}
				ROM_ADDR	+= 2;				// 写入成功后地址+2
			}
			// 启用Flash写保护
			SAFE_MOD	= 0x55;
			SAFE_MOD	= 0xAA;
			GLOBAL_CFG	&= ~(bCODE_WE | bDATA_WE);
			break;
		}
		case 0x83:								// 校验CodeFlash命令
		{
			ROM_ADDR_L	= Ep2Buffer[0x42];		// 写入地址低位
			ROM_ADDR_H	= Ep2Buffer[0x43];		// 写入地址高位
			for(i=0; i<Length; ++i)
			{
				if(Ep2Buffer[0x44+i] != *(PUINT8C)ROM_ADDR)
				{
					return 0xFF;
				}
				++ROM_ADDR;
			}
			break;
		}
		case 0x1B:								// 重启命令[LoadBoot],重启并进入用户程序
		{
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			GLOBAL_CFG |= bSW_RESET;
			break;
		}
		case 0x1D:								// 校验ID加密
		{
			Ep2Buffer[0x42] = CHIP_ID;			// 读芯片型号,命名规则CH5xx(这里获取后两位)
			Ep2Buffer[0x44] = ChipIdData0;
			Ep2Buffer[0x45] = ChipIdData1;
			Ep2Buffer[0x46] = ChipIdData2;
			Ep2Buffer[0x47] = ChipIdData3;
			Ep2Buffer[0x48] = ChipIdData4;
			Ep2Buffer[0x49] = ChipIdData5;
			break;
		}
		default:								// 未知命令[Error]
		{
			return 0xE0;
		}
	}
	return 0x00;
}

/*******************************************************************************
* Function Name	: USB_DeviceInit
* Description	: USB设备模式初始化函数
* Input			: None
* Return		: None
*******************************************************************************/
void USB_DeviceInit(void)
{
	UINT8 i = 0xFF;
	USB_CTRL = 0x06;		// 强制复位USB协议处理器（从IAP到用户程序必须的步骤）
	while (--i);			// 等待USB协议处理器复位完成

	USB_DEV_AD = 0x00;		// 设备地址初始化为0x00
	USB_CTRL = 0x29;		// 0b0010 1001 总线信号12M全速模式，使能USB设备功能，启用内部上拉，使能DMA与DMA中断
	UDEV_CTRL = 0x31;		// 0b0011 0001 使能USB接收，禁用DP&DM下拉，物理端口12M全速模式，使能USB物理端口

	UEP0_DMA = Ep0Buffer;										// 端点0 数据传输地址(端点0单64byte收发共用缓冲区)
	UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;					// 端点0 IN返回NAK，OUT返回ACK
	UEP4_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;					// 端点4 IN返回NAK，OUT返回ACK (注：端点4不支持同步触发位自动翻转功能)
	UEP4_1_MOD = 0x00;											// 端点0单64byte收发共用缓冲区，端点4收发禁止

	UEP1_DMA = Ep1Buffer;										// 端点1 数据传输地址
	UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点1 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP4_1_MOD |= 0x40;											// 端点1 发送使能(单64byte发送缓冲区)

	UEP2_DMA = Ep2Buffer;										// 端点2 数据传输地址
	UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;	// 端点2 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP2_3_MOD |= 0x0C;											// 端点2 收发使能(64byte*2收发缓冲区)

	USB_INT_EN |= 0x00;		// 关闭全部中断使能位
	USB_INT_FG = 0xFF;		// 清中断标志(高3位只读,低5位写1清零)
	IE_USB = 0;				// 禁止USB总中断
}

/*******************************************************************************
* Function Name	: USB_DeviceInterrupt
* Description	: USB中断查询函数 (IAP程序不支持中断，只能使用查询模式)
* Input			: None
* Return		: None
*******************************************************************************/
void USB_DeviceInterrupt(void)
{
	static UINT8	UsbConfig;		// USB配置模式位

	static PUINT8C	pDescr;			// 缓存USB待发送数据指针
	static UINT16	USBwLength;		// 缓存USB下一阶段需要上传的数据长度
	static UINT8	SetupReq;		// 缓存USB描述符的请求类型
	static UINT8	length;			// 缓存USB待发送数据长度

	if (UIF_TRANSFER)															// USB传输完成中断标志
	{
		switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP)) {				// 判断此次中断是由哪个端点发起的
		case UIS_TOKEN_OUT | 2:													// 端点2 下传
			if (U_TOG_OK == 0) break;
			memcpy(Ep2Buffer+0x40, Ep2Buffer, USB_RX_LEN);						// 转存数据
			Ep2Buffer[0x41] = FlashCoding();									// 处理收到的数据
			UEP2_T_LEN = USB_RX_LEN;
			UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;			// 有数据时上传数据并应答ACK
			break;
		case UIS_TOKEN_IN | 2:													// 端点2 上传
			UEP2_T_LEN = 0;
			UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
			break;
		case UIS_TOKEN_IN | 1:													// 端点1 上传
			UEP1_T_LEN = 0;
			UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
			break;
		case UIS_TOKEN_OUT | 0:													// 端点0 下传
			if (SetupReq == SET_LINE_CODING)									// 设置串口属性
			{
				if (U_TOG_OK == 0) break;
				UEP0_CTRL ^= bUEP_R_TOG;
				//memcpy(LineCoding, Ep0Buffer, sizeof(LineCoding));
				UEP0_T_LEN = 0;
				UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_ACK;
			}
			else
			{
				UEP0_T_LEN = 0;
				UEP0_CTRL |= UEP_R_RES_ACK | UEP_T_RES_NAK;						// 状态阶段，对IN响应NAK
			}
			break;
		case UIS_TOKEN_IN | 0:													// 端点0 上传
			switch (SetupReq) {													// 分析描述符类型
			case USB_GET_DESCRIPTOR:											// 主机获取描述符
				UEP0_T_LEN = USBwLength < EP0SIZE ? USBwLength : EP0SIZE;		// 若数据长度超过缓冲区尺寸,则需要截断分多次发送
				memcpy(Ep0Buffer, pDescr, UEP0_T_LEN);							// 拷贝需要发送的数据
				USBwLength -= UEP0_T_LEN;										// 减少已经发送的数据长度
				pDescr += UEP0_T_LEN;											// 数据指针移动到下次需要的位置
				UEP0_CTRL ^= bUEP_T_TOG;										// 同步标志位翻转
				break;
			case USB_SET_ADDRESS:												// 主机设置设备地址
				USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | USBwLength;				// 写设备地址
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
				break;
			default:
				UEP0_T_LEN = 0;													// 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
				break;
			}
			break;
		case UIS_TOKEN_SETUP:													// SETUP事务,用于USB设备初始化
		{
			if (USB_RX_LEN == sizeof(USB_SETUP_REQ))							// 检测SETUP包长度是否正确
			{
				length = 0;														// 默认为成功并且上传0长度
				USBwLength = ((UINT16)USBwLengthH << 8) | USBwLengthL;			// 获取Setup包长度
				SetupReq = USBbRequest;											// 获取bRequest
				if ((USBbReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD) 	// 非标准请求
				{
					switch (USBbRequest) {
					case SET_LINE_CODING:	// 0x20	设置CDC参数
						break;
					case GET_LINE_CODING:	// 0x21 读取CDC参数
					{
						pDescr = LineCoding;									// 获取数据指针
						length = sizeof(LineCoding);							// 获取数据长度
						if(USBwLength > length) USBwLength = length;			// 若主机要求的数据长度超过报告长度,则上传报告长度
						length = USBwLength>EP0SIZE ? EP0SIZE : USBwLength;		// 若数据长度超过缓冲区尺寸,则需要截断分多次发送
						memcpy(Ep0Buffer, pDescr, length);						// 拷贝需要发送的数据
						USBwLength -= length;									// 减少已经发送的数据长度
						pDescr += length;										// 数据指针移动到下次需要的位置
						break;
					}
					case SET_LINE_STATE:	// 0x22 设置CDC状态
						break;
					default:
						length = 0xFF;
						break;
					}
				}
				else
				{																// 标准请求包处理
					switch (USBbRequest) {										// 识别请求码
					case USB_GET_DESCRIPTOR:									// 主机获取设备描述符
					{
						switch (USBwValueH) {
						case USB_DESCR_TYP_DEVICE:								// 设备描述符
							pDescr = DevReport;									// 获取数据指针
							length = pDescr[0];									// 获取数据长度(描述符第一位就是数据长度)
							break;
						case USB_DESCR_TYP_CONFIG:								// 配置描述符
							pDescr = CfgReport;									// 获取数据指针
							length = pDescr[2];									// 获取数据长度(描述符第一位就是数据长度)
							break;
						case USB_DESCR_TYP_STRING:								// 字符串描述符
							pDescr = StrReports[(USBwValueL < sizeof(StrReports) ? USBwValueL : 0)];// 获取数据指针
							length = *pDescr;                           		// 获取数据长度(描述符第一位就是数据长度)
							break;
						default:												// 不支持的描述符
							length = 0xFF;
							break;
						}
						if(USBwLength > length) USBwLength = length;			// 若主机要求的数据长度超过报告长度,则上传报告长度
						length = USBwLength>EP0SIZE ? EP0SIZE : USBwLength;		// 若数据长度超过缓冲区尺寸,则需要截断分多次发送
						memcpy(Ep0Buffer, pDescr, length);						// 拷贝需要发送的数据
						USBwLength -= length;									// 减少已经发送的数据长度
						pDescr += length;										// 数据指针移动到下次需要的位置
						break;
					}
					case USB_SET_ADDRESS:										// 主机为设备分配地址
					{
						USBwLength = USBwValueL;    							// 暂存USB设备地址
						break;
					}
					case USB_GET_CONFIGURATION:									// 主机读取设备配置
					{
						Ep0Buffer[0] = 0x00;
						if (USBwLength >= 1) length = 1;
						break;
					}
					case USB_SET_CONFIGURATION:									// 主机写入设备配置
					{
						UsbConfig = USBwValueL;
						break;
					}
					case USB_GET_INTERFACE:
						break;
					case USB_CLEAR_FEATURE:                    					// 清除设备状态
					{
						if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
						{
							switch (USBwIndexL) {
							case 0x81:
								UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
								break;
							case 0x82:
								UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
								break;
							case 0x02:
								UEP2_CTRL = UEP2_CTRL & ~(bUEP_R_TOG | MASK_UEP_T_RES) | UEP_R_RES_ACK;
								break;
							default:
								length = 0xFF;// 操作失败
								break;
							}
						}
						else length = 0xFF;// 操作失败
						break;
					}
					case USB_SET_FEATURE:                       				// 配置设备状态
					{
						if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
						{
							if ((USBwValueH == 0x00) && (USBwValueL == 0x00))
							{
								switch (USBwIndexL) {
								case 0x81:
									UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点1 IN STALL
									break;
								case 0x82:
									UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;// 设置端点2 IN STALL
									break;
								case 0x02:
									UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_R_RES_STALL;// 设置端点2 OUT STALL
									break;
								default:
									length = 0xFF;// 操作失败
									break;
								}
							}
							else length = 0xFF;// 操作失败
						}
						else length = 0xFF;// 操作失败
						break;
					}
					case USB_GET_STATUS:									// 获取设备状态
					{
						Ep0Buffer[0] = 0x00;
						Ep0Buffer[1] = 0x00;
						length = USBwLength < 2 ? USBwLength : 2;
						break;
					}
					default:
						length = 0xFF;// 操作失败
						break;					
					}
				}
			}
			else length = 0xFF;//SETUP包长度错误
			if (length == 0xFF)
			{
				SetupReq = 0xFF;
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
			}
			else if (length <= EP0SIZE)	// 上传数据长度在端点0缓存区尺寸内,可以发送
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
		default:
			break;
		}
		UIF_TRANSFER = 0;															// 清除USB传输完成中断标志
	}
	else if (UIF_BUS_RST)                                                           // USB总线复位中断
	{
		USB_DEV_AD = 0x00;															// USB总线复位，重置设备地址
		USB_INT_FG = 0xFF;															// 总线复位,清除全部中断
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
	USB_DeviceInit();
	// 初始化全部IO口状态
	P1=P3=0xFF;
    while(1)
    {
		if(USB_INT_FG) USB_DeviceInterrupt();		// 查询USB中断标志
		if(--tick == 0 && --chip == 0) IAP_LED = ~IAP_LED;
    }
}
