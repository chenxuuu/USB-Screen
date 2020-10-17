/********************************** (C) COPYRIGHT ******************************
* File Name		: SPI
* Author		: Antecer
* Version		: V1.0
* Date			: 2020年10月09日
* Description	: SPI读写函数
*******************************************************************************/
#include "CH552.h"
#include "SPI.h"

/*******************************************************************************
* Function Name  : SPI_Send_DAT(UINT8 dat)
* Description    : 软件SPI写数据
* Input          : UINT8 dat
*******************************************************************************/
void SPI_Send_DAT(UINT8 dat)
{
	static UINT8 i;
	for (i = 0x80; i; i >>= 1)
	{
		SPI_SDA = dat & i;
		SPI_SCL = 0;
		SPI_SCL = 1;
	}
}
/*******************************************************************************
* Function Name  : SPI_Send_CMD(UINT8 cmd)
* Description    : 软件SPI写命令
* Input          : UINT8 cmd
*******************************************************************************/
void SPI_Send_CMD(UINT8 cmd)
{
	SPI_DC = 0;	// 写命令
	SPI_Send_DAT(cmd);
	SPI_DC = 1;	// 写数据
}
/*******************************************************************************
* Function Name  : SPI_Init(void)
* Description    : 初始化SPI总线
* Input          : None
* Return         : None
*******************************************************************************/
//void SPI_Init(void)
//{
//	P1_DIR_PU  = 0x1F;	// 第7,6,5位禁用上拉(注意需要外接3.3V弱上拉10K电阻)
//	SPI0_CK_SE = 0x02;	// 分频系数2，SPI频率=Fsys/2
//	SPI0_SETUP = 0x00;	// 主机模式,关中断,高位在前(默认)
//}
/*******************************************************************************
* Function Name  : SPI_Mode(UINT8 mod)
* Description    : 硬件SPI模式配置(0x60模式0;0x68模式1;0x02关闭硬件SPI)
* Input          : UINT8 mod
* Return         : None
*******************************************************************************/
//void SPI_Mode(UINT8 mod)
//{
//	SPI0_CTRL = mod;
//}
/*******************************************************************************
* Function Name  : SPI_SendDAT(UINT8 dat)
* Description    : 硬件SPI写数据(主机模式)
* Input          : UINT8 dat
* Return         : None
*******************************************************************************/
//void SPI_SendDAT(UINT8 dat)
//{
//	SPI0_DATA = dat;
//	while (S0_FREE == 0);	//等待传输完成
//}
/*******************************************************************************
* Function Name  : SPI_SendCMD(UINT8 cmd)
* Description    : 硬件SPI写命令(主机模式)
* Input          : UINT8 cmd
* Return         : None
*******************************************************************************/
//void SPI_SendCMD(UINT8 cmd)
//{
//	SPI_DC = 0;			// 写命令
//	SPI_SendDAT(cmd);
//	SPI_DC = 1;			// 写数据
//}
