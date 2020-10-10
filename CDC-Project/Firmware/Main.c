
#include <string.h>
#include "CH552.h"
#include "Freqsys.h"

#include "LCD.h"
#include "UsbManager.h"

sbit	Bg_Led1	= P3^0;
sbit	Bg_Led2 = P3^3;
sbit	Bg_Led3 = P1^1;


/*******************************************************************************
* Function Name	: IOport_Init
* Description	: IO口默认状态配置位
* Input			: None
* Return		: None
*******************************************************************************/
void IOport_Init(void)
{
	LCD_BLK = 0;
	while(USB_DEV_AD==0);
	LCD_BLK = 1;
	P3 = 0xFF;
	P1 = 0xFF;
}

/*******************************************************************************
* Function Name	: main
* Description	: 主函数
* Input			: None
* Return		: None
*******************************************************************************/
void main(void)
{
	setFsys();					// 初始化系统时钟
	USB_DeviceInit();			// 初始化USB功能
	EA  = 1;					// 使能单片机全局中断
	IOport_Init();				// 初始化IO口状态
	LCD_Init();
	while(1)
	{
	}
}