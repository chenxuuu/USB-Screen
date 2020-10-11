#ifndef __LCD_H__
#define __LCD_H__

sbit LCD_RES = P3 ^ 2;		// LCD复位
sbit LCD_BLK = P3 ^ 4;		// LCD背光控制开关

extern LCD_BUFF;

extern void LCD_Init(void);
extern void LCD_SET(PUINT8X addr);
#endif