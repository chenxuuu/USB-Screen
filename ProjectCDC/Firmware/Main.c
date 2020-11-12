
#include <string.h>
#include "CH552.h"
#include "Freqsys.h"

#include "LCD.h"
#include "UsbManager.h"

sbit	K1	= P3^0;
sbit	K2	= P1^1;
sbit	K3	= P3^3;

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
* Function Name	: PWM_Init
* Description	: PWM初始化(PWM2输出引脚为P3.4)
* Input			: None
* Return		: None
*******************************************************************************/
void PWM_Init()
{
	PWM_CK_SE	= 0x01;			// PWM时钟分频设置
	PWM_DATA2	= 0x7F;			// PWM2占空比设置(占空比=PWM_DATA2/256)
	PWM_CTRL	= bPWM2_OUT_EN;	// PWM2输出使能
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
	PWM_Init();					// 初始化PWM功能
	USB_DeviceInit();			// 初始化USB功能
	EA  = 1;					// 使能单片机全局中断
	IOport_Init();				// 初始化IO口状态
	LCD_Init();					// 初始化LCD屏幕
	while(1)
	{
		if(DMA_STATUS & 0x01)
		{
			ROM_DATA_H = DMA_STATUS&0x10;
			LCD_SET(Ep3Buffer);
			DMA_STATUS &= (~0x11);
			if((UDEV_CTRL&bUD_GP_BIT) == 0) 
			{
				UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
			}
		}
		else if(DMA_STATUS & 0x02)
		{
			ROM_DATA_H = DMA_STATUS&0x20;
			LCD_SET(Ep4Buffer);
			DMA_STATUS &= (~0x22);
			if((UDEV_CTRL&bUD_GP_BIT) != 0) 
			{
				UEP3_CTRL = UEP3_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
			}
		}
	}
}