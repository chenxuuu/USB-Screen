#ifndef __LCD_H__
#define __LCD_H__

#ifndef UINT8
#define UINT8 unsigned char
#endif
#ifndef UINT16
#define UINT16 unsigned short
#endif

sbit LCD_RES = P3 ^ 2;		// LCD复位
sbit LCD_BLK = P3 ^ 4;		// LCD背光控制开关

extern void LCD_Init(void);
#endif