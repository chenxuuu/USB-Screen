#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "REG51.h"

#define USE_HORIZONTAL 0  //���ú�������������ʾ 0��1Ϊ���� 2��3Ϊ����


#define LCD_W 240
#define LCD_H 240

#define u8  unsigned char
#define u16 unsigned int

sbit LCD_SCL=P1^0;//SCLK
sbit LCD_SDA=P1^1;//MOSI
sbit LCD_RES=P1^2;//RES
sbit LCD_DC =P1^3;//DC
sbit LCD_CS =P1^4; //CS
sbit LCD_BLK=P1^5; //BLK

//-----------------LCD�˿ڶ���----------------

#define LCD_SCLK_Clr() LCD_SCL=0//SCL=SCLK
#define LCD_SCLK_Set() LCD_SCL=1

#define LCD_MOSI_Clr() LCD_SDA=0//SDA=MOSI
#define LCD_MOSI_Set() LCD_SDA=1

#define LCD_RES_Clr() LCD_RES=0//RES
#define LCD_RES_Set() LCD_RES=1

#define LCD_DC_Clr() LCD_DC=0//DC
#define LCD_DC_Set() LCD_DC=1

#define LCD_CS_Clr()  LCD_CS=0//CS
#define LCD_CS_Set()  LCD_CS=1

#define LCD_BLK_Clr()  LCD_BLK=0//BLK
#define LCD_BLK_Set()  LCD_BLK=1



void delay_ms(unsigned int ms);//��׼ȷ��ʱ����
void LCD_GPIO_Init(void);//��ʼ��GPIO
void LCD_Writ_Bus(u8 dat);//ģ��SPIʱ��
void LCD_WR_DATA8(u8 dat);//д��һ���ֽ�
void LCD_WR_DATA(u16 dat);//д�������ֽ�
void LCD_WR_REG(u8 dat);//д��һ��ָ��
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//�������꺯��
void LCD_Init(void);//LCD��ʼ��
#endif




