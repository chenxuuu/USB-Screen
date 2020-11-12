/********************************** (C) COPYRIGHT ******************************
* File Name		: UsbManager
* Author		: Antecer
* Version		: V1.0
* Date			: 2020年10月09日
* Description	: USB事件处理程序
*******************************************************************************/
#include "CH552.h"
#include <string.h>
#include "UsbDescriptor.h"
#include "UsbManager.h"
#include "SPI.h"

// 定义USB数据缓冲区范围
xdata UsbBuffer Buffer _at_ 0x0000;
// 定义DMA缓冲区占用标志(0x01缓冲区1收到命令,0x11缓冲区1收到数据; 0x02缓冲区2收到命令,0x22缓冲区2收到数据)
UINT8 DMA_STATUS = 0;

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
	UEP2_3_MOD |= 0x04;											// 端点2 发送使能(单64byte发送缓冲区)

	UEP3_DMA	= Ep3Buffer;									// 端点3 数据传输地址
	UEP3_CTRL	= bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;// 端点3 自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
	UEP2_3_MOD	|= 0x90;										// 端点3 接收使能(双64byte接收缓冲区)

	USB_INT_EN |= 0x03;		// 禁止[SOF&NAK&FIFO]中断；使能[总线挂起或唤醒&传输完成&总线复位]中断
	USB_INT_FG = 0xFF;		// 清中断标志(高3位只读,低5位写1清零)
	IE_USB = 1;				// 使能USB总中断
}

/*******************************************************************************
* Function Name	: USB_DeviceInterrupt
* Description	: USB中断查询函数
* Input			: None
* Return		: None
*******************************************************************************/
void USB_DeviceInterrupt(void) interrupt INT_NO_USB
{
	static UINT8	UsbConfig;		// USB配置模式位

	static PUINT8C	pDescr;			// 缓存USB待发送数据指针
	static UINT16	USBwLength;		// 缓存USB下一阶段需要上传的数据长度
	static UINT8	SetupReq;		// 缓存USB描述符的请求类型
	UINT8	length;					// 缓存USB待发送数据长度

	if (UIF_TRANSFER)															// USB传输完成中断标志
	{
		switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP)) {				// 判断此次中断是由哪个端点发起的
		case UIS_TOKEN_OUT | 3:													// 端点2 下传
			if (U_TOG_OK == 0) break;
			if (UDEV_CTRL&bUD_GP_BIT)
			{
				SetupReq = Ep4Buffer[0];	// Setup事务完成后不再需要此变量,所以这里可以复用
				DMA_STATUS |= USB_RX_LEN==64 ? 0x22 : 0x02;
			}
			else
			{
				SetupReq = Ep3Buffer[0];
				DMA_STATUS |= USB_RX_LEN==64 ? 0x11 : 0x01;
			}
			if (USB_RX_LEN == 1 && (SetupReq == 0xB1))							// Bootloader跳转命令
			{
				((void(code *)(void))IAP_CODE_ADDR)();							// 跳转到Bootloader
			}
			if ((DMA_STATUS&0x0F) == 0x03)	// 缓冲区满，暂停接收
			{
				UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK;
			}
			UDEV_CTRL ^= bUD_GP_BIT;
			break;
		case UIS_TOKEN_IN | 2:													// 端点2 上传
//			UEP2_T_LEN = 0;
//			UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
			break;
		case UIS_TOKEN_IN | 1:													// 端点1 上传
//			UEP1_T_LEN = 0;
//			UEP1_CTRL = UEP1_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;			// 默认应答NAK
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
						Ep0Buffer[0] = UsbConfig;
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
						if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
						{
							if ((USBwValueH == 0x00) && (USBwValueL == 0x01))
							{
								if (CfgReport[7] & 0x20)
								{
									/* 唤醒设备 */
								}
								else length = 0xFF;
							}else length = 0xFF;
						}
						else if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
						{
							switch (USBwIndexL) {
							case 0x81:
								UEP1_CTRL = UEP1_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
								break;
							case 0x82:
								UEP2_CTRL = UEP2_CTRL & ~(bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
								break;
							case 0x03:
								UEP3_CTRL = UEP3_CTRL & ~(bUEP_R_TOG | MASK_UEP_T_RES) | UEP_R_RES_ACK;
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
						if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
						{
							if ((USBwValueH == 0x00) && (USBwValueL == 0x01))
							{
								if (CfgReport[7] & 0x20)
								{
									/* 睡眠设备 */
									SAFE_MOD = 0x55;
									SAFE_MOD = 0xAA;
									WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO | bWAK_RXD1_LO;// USB或者RXD0/1有信号时可被唤醒
									PCON |= PD;// 进入睡眠状态
								}
								else length = 0xFF;// 操作失败
							}
							else length = 0xFF;// 操作失败
						}
						else if ((USBbReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
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
								case 0x03:
									UEP3_CTRL = UEP3_CTRL & (~bUEP_T_TOG) | UEP_R_RES_STALL;// 设置端点3 OUT STALL
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
		USB_DEV_AD &= bUDA_GP_BIT;													// USB总线复位，重置设备地址
		USB_INT_FG = 0xFF;															// 总线复位,清除全部中断
	}
	else if (UIF_SUSPEND)															// 总线挂起或唤醒中断
	{
		UIF_SUSPEND = 0;
		if (USB_MIS_ST & bUMS_SUSPEND)	// 已经有一段时间没有USB活动，请求挂起
		{
			SAFE_MOD = 0x55;
			SAFE_MOD = 0xAA;
			WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO | bWAK_RXD1_LO;// USB或者RXD0/1有信号时可被唤醒
			PCON |= PD;	// 进入睡眠状态
		}
	}
	else UIF_FIFO_OV = 0;															// 清FIFO溢出中断标志
}