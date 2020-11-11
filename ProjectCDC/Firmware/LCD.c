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
	// 使用软SPI初始化能看到效果(硬SPI太快,看不到)
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
	SPI_MODE(0x60);		// 启用硬件SPI
}

/*******************************************************************************
* Function Name	: LCD_SET
* Description	: LCD屏幕显示设置
* Input			: PUIN8XV addr
* Return		: None
*******************************************************************************/
void LCD_SET(PUINT8XV p)
{
	static UINT8 len;				// 数据长度{最高位:色彩模式命令; 低7位:数据长度0-64}
	static UINT8 mode = 0x00;		// 色彩模式<零:rgb565彩色; 非零:黑白>
	static UINT8 i;					// 黑白模式数据分离
	XBUS_AUX |= bDPTR_AUTO_INC;		// 使能MOVX_@DPTR指令执行后DPTR自动INC
	if(ROM_DATA_H)	// {64byte数据}
	{
		len = 64;
		if(mode==0x00)
		{
			do
			{
				ROM_DATA_L=*p;
				while(S0_FREE==0);
				SPI0_DATA=ROM_DATA_L;
			} while (--len);
		}
		else
		{
			do
			{
				ROM_DATA_L=*p;
				i=0x80;
				do
				{
					ROM_DATA_H = ROM_DATA_L&i ? 0xFF : 0x00;
					while(S0_FREE==0); SPI0_DATA = ROM_DATA_H;
					i>>=1;
					while(S0_FREE==0); SPI0_DATA = ROM_DATA_H;
				} while (i);
			} while (--len);
		}
	}
	else	// {1b数据长度，1b列开始地址，1b列结束地址，1b行开始地址，1b行结束地址，n*2byte数据(数据长度必须为偶数)}
	{
		len = *p;
		if((len&0x40) == 0)	// len^6=1 时,当成纯数据包处理
		{
			// 列地址设置(0-239)，高位在前
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA=0x2A;
			while(S0_FREE==0); SPI_DC=1; SPI0_DATA&=0x00; ROM_DATA_L=*p;	// 列开始地址
			while(S0_FREE==0); SPI0_DATA = ROM_DATA_L;
			while(S0_FREE==0); SPI0_DATA&=0x00; ROM_DATA_L=*p;				// 列结束地址
			while(S0_FREE==0); SPI0_DATA = ROM_DATA_L;
			// 行地址设置(0-239)，高位在前
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA=0x2B;
			while(S0_FREE==0); SPI_DC=1; SPI0_DATA&=0x00; ROM_DATA_L=*p;	// 行开始地址
			while(S0_FREE==0); SPI0_DATA = ROM_DATA_L;
			while(S0_FREE==0); SPI0_DATA&=0x00; ROM_DATA_L=*p;				// 行结束地址
			while(S0_FREE==0); SPI0_DATA = ROM_DATA_L;
			// 写LCD数据命令
			while(S0_FREE==0); SPI_DC=0; SPI0_DATA=0x2C; ROM_DATA_L=*p;
			// 读色彩模式命令
			mode = len&0x80;
			while(S0_FREE==0); SPI_DC=1;
		}
		else if(len == 0xC0) // 亮度调节命令
		{
			PWM_DATA2 = *p;
		}
		else ROM_DATA_L=*p;
		len &= 0x3F;
		// 写LCD数据内容
		if(mode==0x00)
		{
			while(len)
			{
				while(S0_FREE==0);
				SPI0_DATA=ROM_DATA_L;
				ROM_DATA_L=*p;
				--len;
			}
		}
		else
		{
			while(len)
			{
				i=0x80;
				do
				{
					ROM_DATA_H = ROM_DATA_L&i ? 0xFF : 0x00;
					while(S0_FREE==0); SPI0_DATA = ROM_DATA_H;
					i>>=1;
					while(S0_FREE==0); SPI0_DATA = ROM_DATA_H;
				} while (i);
				ROM_DATA_L=*p;
				--len;
			}
		}
	}
	XBUS_AUX &= ~bDPTR_AUTO_INC;		// 禁用MOVX_@DPTR指令执行后DPTR自动INC
}