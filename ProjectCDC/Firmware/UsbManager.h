#ifndef __USBMANAGER_H__
#define __USBMANAGER_H__

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

// USB数据缓冲区结构体{EP0[0x40],EP1[0x40],EP2[0x40],EP3[0x80]}
typedef struct _UsbBuffer{
	UINT8 EP0[0x40];	// 端点0 OUT&IN 64byte收发共用缓冲区
	UINT8 EP1[0x40];	// 端点1 IN 64byte发送缓冲区
	UINT8 EP2[0x40];	// 端点2 IN 64byte发送缓冲区
	UINT8 EP3[0x40];	// 端点3 OUT 64byte*2接收缓冲区(双缓冲接收)
	UINT8 EP4[0x40];	//
} UsbBuffer;

extern xdata UsbBuffer Buffer;
#define Ep0Buffer Buffer.EP0
#define Ep1Buffer Buffer.EP1
#define Ep2Buffer Buffer.EP2
#define Ep3Buffer Buffer.EP3
#define Ep4Buffer Buffer.EP4

#define UsbSetupBuf	((PUSB_SETUP_REQ)Ep0Buffer)	// 定义Setup包结构体
#define USBbReqType	(UsbSetupBuf->bRequestType)
#define USBbRequest	(UsbSetupBuf->bRequest)
#define USBwValueL	(UsbSetupBuf->wValueL)
#define USBwValueH	(UsbSetupBuf->wValueH)
#define USBwIndexL	(UsbSetupBuf->wIndexL)
#define USBwIndexH	(UsbSetupBuf->wIndexH)
#define USBwLengthL	(UsbSetupBuf->wLengthL)
#define USBwLengthH	(UsbSetupBuf->wLengthH)

extern UINT8 DMA_STATUS;
extern void USB_DeviceInit(void);
extern void HID_DeviceInterrupt(void);

#endif