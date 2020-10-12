/********************************** (C) COPYRIGHT ******************************
* File Name		: LCD
* Author		: Antecer
* Version		: V1.0
* Date			: 2020年10月09日
* Description	: SPI读写函数
*******************************************************************************/
#include "CH552.h"
#include "Freqsys.h"
#include "SPI.h"
#include "LCD.h"

/*******************************************************************************
* Function Name	: LCD_Init
* Description	: LCD屏幕显示初始化
* Input			: None
* Return		: None
*******************************************************************************/
void LCD_Init(void)
{
	SPI_INIT();			// 初始化硬件SPI总线
	LCD_BLK = 1;		// 打开背光
	SPI_CS = 0;			// LCD片选使能（允许操作屏幕）
	mDelaymS(20);		// 等待
	LCD_RES = 0;		// 复位开始
	mDelaymS(20);		// 等待
	LCD_RES = 1;		// 复位结束
	mDelaymS(20);		// 等待
	SPI_CS = 1;
	SPI_CS = 0;
	//SPI_Mode(0x60);// 启用硬件SPI (使用硬件SPI初始化屏幕); 使用软SPI初始化能看到效果(硬SPI太快,看不到)
	SPI_Send_CMD(0x11);
	SPI_Send_CMD(0x36);
	SPI_Send_DAT(0x00);
	SPI_Send_CMD(0x3A);
	SPI_Send_DAT(0x05);
	SPI_Send_CMD(0xB2);
	SPI_Send_DAT(0x0C);
	SPI_Send_DAT(0x0C);
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0x33);
	SPI_Send_DAT(0x33);
	SPI_Send_CMD(0xB7);
	SPI_Send_DAT(0x35);
	SPI_Send_CMD(0xBB);
	SPI_Send_DAT(0x32);	// Vcom=1.35V
	SPI_Send_CMD(0xC2);
	SPI_Send_DAT(0x01);
	SPI_Send_CMD(0xC3);
	SPI_Send_DAT(0x15);	// GVDD=4.8V  颜色深度
	SPI_Send_CMD(0xC4);
	SPI_Send_DAT(0x20);	// VDV, 0x20:0v
	SPI_Send_CMD(0xC6);
	SPI_Send_DAT(0x0F);	// 0x0F:60Hz
	SPI_Send_CMD(0xD0);
	SPI_Send_DAT(0xA4);
	SPI_Send_DAT(0xA1);
	SPI_Send_CMD(0xE0);
	SPI_Send_DAT(0xD0);
	SPI_Send_DAT(0x08);
	SPI_Send_DAT(0x0E);
	SPI_Send_DAT(0x09);
	SPI_Send_DAT(0x09);
	SPI_Send_DAT(0x05);
	SPI_Send_DAT(0x31);
	SPI_Send_DAT(0x33);
	SPI_Send_DAT(0x48);
	SPI_Send_DAT(0x17);
	SPI_Send_DAT(0x14);
	SPI_Send_DAT(0x15);
	SPI_Send_DAT(0x31);
	SPI_Send_DAT(0x34);
	SPI_Send_CMD(0xE1);
	SPI_Send_DAT(0xD0);
	SPI_Send_DAT(0x08);
	SPI_Send_DAT(0x0E);
	SPI_Send_DAT(0x09);
	SPI_Send_DAT(0x09);
	SPI_Send_DAT(0x15);
	SPI_Send_DAT(0x31);
	SPI_Send_DAT(0x33);
	SPI_Send_DAT(0x48);
	SPI_Send_DAT(0x17);
	SPI_Send_DAT(0x14);
	SPI_Send_DAT(0x15);
	SPI_Send_DAT(0x31);
	SPI_Send_DAT(0x34);
	SPI_Send_CMD(0x21);
	SPI_Send_CMD(0x29);

	SPI_Send_CMD(0x2A);// 列地址设置(0-239)
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0xEF);
	SPI_Send_CMD(0x2B);// 行地址设置(0-239)
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0x00);
	SPI_Send_DAT(0xEF);
	SPI_Send_CMD(0x2C);// 写LCD数据存储器
	{
		UINT8 v, h;
		for (v = 240; v; --v)
		{
			for (h = 240; h; --h)
			{
				SPI_Send_DAT(0x00);
				SPI_Send_DAT(0x00);
			}
		}
	}
	//SPI_Mode(0x00);	// 关闭硬件SPI
}

/*******************************************************************************
* Function Name	: LCD_SET
* Description	: LCD屏幕显示设置
				  内存格式:{1b命令,1b长度,1b列开始地址,1b列结束地址,1b行开始地址,1b行结束地址,连续数据}
* Input			: PUIN8XV addr
* Return		: None
*******************************************************************************/
void LCD_SET(PUINT8X p)
{
	static UINT8 dat,len,i;
	switch(*p) {
		case 0xFA: {
			len = *++p;
			SPI_CS = 1;
			SPI_CS = 0;
			// 启用硬件SPI
			SPI_MODE(0x60);
			// 列地址设置(0-239)，高位在前
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA = 0x2A;
			while(S0_FREE==0); SPI_DC=1; SPI0_DATA = 0x00; dat = *++p;
			while(S0_FREE==0); SPI0_DATA = dat;
			// 行地址设置(0-239)，高位在前
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA = 0x2B;
			while(S0_FREE==0); SPI_DC=1; SPI0_DATA = 0x00; dat = *++p;
			while(S0_FREE==0); SPI0_DATA = dat;
			// 写LCD数据命令
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA = 0x2C; dat = *++p;
			// 写LCD数据内容
			while(S0_FREE==0); SPI_DC=1; SPI0_DATA = dat; dat = *++p;
			while(--len) {while(S0_FREE==0); SPI0_DATA = dat; dat = *++p;}
			// 关闭硬件SPI
			SPI_MODE(0x00);
			break;
		}
		case 0xFB:
			len = *++p;
			SPI_CS = 1;
			SPI_CS = 0;

			SPI_MODE(0x60);
			SPI_CMD(0x2A);		// 列地址设置(0-239)
			SPI_DAT(0x00);
			SPI_DAT(*++p);
			SPI_CMD(0x2B);		// 行地址设置(0-239)
			SPI_DAT(0x00);
			SPI_DAT(*++p);
			SPI_CMD(0x2C);		// 写LCD数据存储器
			while(S0_FREE==0);SPI_DC=1;
			for(;len;--len)
			{
				++p;
				for(i=0x80;i;i>>=1)
				{
					SPI_DAT_PURE(*p&i?0xFF:0x00);
				}
			}
			while(S0_FREE==0);
			SPI_MODE(0x00);
			break;
		default:
			break;
	}
}
