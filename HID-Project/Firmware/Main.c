#include <stdio.h>
#include <string.h>
#include "CH552.h"
#include "TypeDefines.h"
#include "UsbReports.h"
#include "SPI.h"
#include "UsbManager.h"

sbit	Bg_Led1	= P3^0;
sbit	Bg_Led2 = P3^3;
sbit	Bg_Led3 = P1^1;

/*******************************************************************************
* Function Name	: SetFsys
* Description	: 设置系统时钟频率
* Info			: 内核时钟12MHz
				: PLL倍频=96MHz
				: USB时钟分频器=96MHz/48MHz		(USB模块工作需要48MHz频率)
				: 系统时钟分频器=96MHz/24MHz
*******************************************************************************/
void SetFsys()
{
	SAFE_MOD	= 0x55;		//解锁安全模式
	SAFE_MOD	= 0xAA;
	CLOCK_CFG	= 0x86;		//系统时钟分频器4分频=24MHz
}
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
    P1_DIR_PU  = 0x1F;		// 第7,6,5位禁用上拉(注意需要外接3.3V弱上拉10K电阻)
}

/*******************************************************************************
* Function Name	: main
* Description	: 主函数
* Input			: None
* Return		: None
*******************************************************************************/
void main(void)
{
	SetFsys();					// 初始化系统时钟
	USB_DeviceInit();			// 初始化USB功能
	EA  = True;					// 使能单片机全局中断
	IOport_Init();				// 初始化IO口状态
	LCD_Init();
	while(true)
	{
		LCD_BLK = IsUsbShut;	// 电脑关机后熄灭屏幕背光
	}
}