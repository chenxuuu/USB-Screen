/********************************** (C) COPYRIGHT ******************************
* File Name		: SPI
* Author		: Antecer
* Version		: V1.0
* Date			: 2020年10月09日
* Description	: SPI读写函数
*******************************************************************************/
#include "CH552.h"
#include "Freqsys.h"
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
* Function Name  : SPI_MasterModeSet(void)
* Description    : 以主机模式初始化硬件SPI
* Input          : None
* Return         : None
*******************************************************************************/
void SPI_MasterModeSet(void)
{
	SPI0_CTRL = 0x60;		// 模式0		
	SPI0_CK_SE = 0x02;		// 分频系数2，SPI频率=Fsys/2
//	P1_MOD_OC |= 0xF0;		// 高4位设置位开漏输出
//	P1_DIR_PU |= 0xF0;		// 高4位启用上拉电阻
}
/*******************************************************************************
* Function Name  : SPI_SendDAT(UINT8 dat)
* Description    : 硬件SPI写数据(主机模式)
* Input          : UINT8 dat
* Return         : None
*******************************************************************************/
void SPI_SendDAT(UINT8 dat)
{
	SPI0_DATA = dat;
	while (S0_FREE == 0);	//等待传输完成
}
/*******************************************************************************
* Function Name  : SPI_SendCMD(UINT8 cmd)
* Description    : 硬件SPI写命令(主机模式)
* Input          : UINT8 cmd
* Return         : None
*******************************************************************************/
void SPI_SendCMD(UINT8 cmd)
{
	SPI_DC = 0;			// 写命令
	SPI_SendDAT(cmd);
	SPI_DC = 1;			// 写数据
}