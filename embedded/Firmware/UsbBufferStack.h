/********************************** (C) COPYRIGHT ******************************
* File Name		: UsbBufferStack.h
* Author		: Antecer
* Version		: V1.0
* Date			: 2020年2月22日
* Description	: USB模块缓冲区堆栈
*******************************************************************************/

//#ifndef UsbBufferStack_H
//#define UsbBufferStack_H

//UINT8X Ep0Buffer[0xC0] _at_ 0x0000;				// 端点0 OUT&IN 64byte收发共用缓冲区 + 端点4 OUT&IN 64byte*2收发缓冲区
//UINT8X Ep1Buffer[0x10] _at_ 0x00C0;				// 端点1 IN 64byte发送缓冲区
////UINT8X	Ep2Buffer[0x10]	_at_ 0x00D0;			// 端点2 IN16byte发送缓冲区	(默认为64byte发送缓冲区)
////UINT8X	Ep3Buffer[0x10]	_at_ 0x00E0;			// 端点3 IN16byte发送缓冲区	(默认为64byte发送缓冲区)
//#define UsbSetupBuf ((PUSB_SETUP_REQ)Ep0Buffer) // 定义Setup包结构体
//#define HIDbReqType UsbSetupBuf->bRequestType
//#define HIDbRequest UsbSetupBuf->bRequest
//#define HIDwValueL UsbSetupBuf->wValueL
//#define HIDwValueH UsbSetupBuf->wValueH
//#define HIDwIndexL UsbSetupBuf->wIndexL
//#define HIDwIndexH UsbSetupBuf->wIndexH
//#define HIDwLengthL UsbSetupBuf->wLengthL
//#define HIDwLengthH UsbSetupBuf->wLengthH

//#endif /*UsbBufferStack_H*/