#ifndef	__SPI_H__
#define	__SPI_H__

sbit SPI_DC = P3 ^ 1;		// 数据&命令(低电平写命令,高电平写数据)
sbit SPI_CS = P1 ^ 4;		// 片选;低电平有效
sbit SPI_SCL = P1 ^ 7;		// SPI时钟
sbit SPI_SDA = P1 ^ 5;		// SPI数据(MOSI)

extern void SPI_Send_DAT(UINT8 dat);
extern void SPI_Send_CMD(UINT8 cmd);

extern void SPI_Init(void);
extern void SPI_Mode(UINT8 mod);
extern void SPI_SendDAT(UINT8 dat);
extern void SPI_SendCMD(UINT8 cmd);

#endif