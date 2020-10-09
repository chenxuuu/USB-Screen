
#ifndef	SPI_H
#define	SPI_H

#ifndef UINT8
#define UINT8 unsigned char
#endif
#ifndef UINT16
#define UINT16 unsigned short
#endif

sbit SPI_DC = P3 ^ 1;		// 数据&命令(低电平写命令,高电平写数据)
sbit SPI_CS = P1 ^ 4;		// 片选;低电平有效
sbit SPI_SCL = P1 ^ 7;		// SPI时钟
sbit SPI_SDA = P1 ^ 5;		// SPI数据(MOSI)

extern void SPI_Send_DAT(UINT8 dat);
extern void SPI_Send_CMD(UINT8 cmd);

extern void SPI_MasterModeSet(void);
extern void SPI_SendDAT(UINT8 dat);
extern void SPI_SendCMD(UINT8 cmd);

#endif