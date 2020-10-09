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

extern void USB_DeviceInit(void);
extern void HID_DeviceInterrupt(void);

#endif