
#ifndef	SPI_H
#define	SPI_H

sbit	LCD_RES	= P3^2;		// 复位
sbit	LCD_BLK	= P3^4;		// 屏幕背光控制开关

sbit	LCD_DC	= P3^1;		// 数据&命令 控制
sbit	LCD_CS	= P1^4;		// 片选;低电平有效
sbit	LCD_SCL	= P1^7;		// SPI时钟
sbit	LCD_SDA	= P1^5;		// SPI数据(MOSI)

/*******************************************************************************
* Function Name  : mDelayus(UNIT16 i)
* Description    : us延时函数
* Input          : UNIT16 i
*******************************************************************************/
void mDelayuS(UINT16 i)
{
	while(i)
	{
		++SAFE_MOD;
		--i;
	}
}
/*******************************************************************************
* Function Name  : mDelayms(UNIT16 i)
* Description    : ms延时函数
* Input          : UNIT16 i
*******************************************************************************/
void mDelaymS(UINT16 i)
{
	while(i)
	{
		mDelayuS(1000);
		--i;
	}
}
/*******************************************************************************
* Function Name  : SPI_Send_DAT(UINT8 dat) 
* Description    : 软件SPI写数据
* Input          : UINT8 dat
*******************************************************************************/
void SPI_Send_DAT(UINT8 dat)
{
	static UINT8 i;
	for(i=0x80; i; i>>=1)
	{
		LCD_SDA = dat&i;
		LCD_SCL	= 0;
		LCD_SCL = 1;
	}
}
/*******************************************************************************
* Function Name  : SPI_Send_CMD(UINT8 dat) 
* Description    : 软件SPI写命令
* Input          : UINT8 dat
*******************************************************************************/
void SPI_Send_CMD(UINT8 dat)
{
	LCD_DC	= 0;	// 写命令
	SPI_Send_DAT(dat);
	LCD_DC	= 1;	// 写数据
}

/*******************************************************************************
* Function Name  : SPIMasterModeSet() 
* Description    : SPI主机模式初始化
* Input          : UINT8 mode						 
* Output         : None
* Return         : None
*******************************************************************************/
void SPIMasterModeSet()
{
	SPI0_CTRL	= 0x60;		// 模式0		
//	SPI0_CK_SE	= 0x02;		// 分频系数2，SPI频率=Fsys/2
//    P1_MOD_OC  |= 0xF0;		// 高4位设置位开漏输出
//    P1_DIR_PU  |= 0xF0;		// 高4位启用上拉电阻
}
/*******************************************************************************
* Function Name  : SPI_SendDAT(UINT8 dat)
* Description    : 硬件SPI写数据,主机模式
* Input          : UINT8 dat   数据
* Return         : None
*******************************************************************************/
void SPI_SendDAT(UINT8 dat)
{
    SPI0_DATA = dat;                                                           
    while(S0_FREE == 0);	//等待传输完成
}
/*******************************************************************************
* Function Name  : SPI_SendCMD(UINT8 dat)
* Description    : 硬件SPI写命令,主机模式
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_SendCMD(UINT8 dat)
{
	LCD_DC	= 0;			// 写命令
	SPI_SendDAT(dat);
	LCD_DC	= 1;			// 写数据
}
/*******************************************************************************
* Function Name	: LCD_Init
* Description	: LCD屏幕显示初始化
* Input			: None
* Return		: None
*******************************************************************************/
void LCD_Init(void)
{
	SPI0_CK_SE	= 0x02;		// 分频系数2，SPI频率=Fsys/2
	LCD_BLK = 1;		// 打开背光
	LCD_CS	= 0;		// LCD片选使能（允许操作屏幕）
	mDelaymS(20);		// 等待
	LCD_RES = 0;		// 复位开始
	mDelaymS(20);		// 等待
	LCD_RES = 1;		// 复位结束
	mDelaymS(20);		// 等待
	LCD_CS = 1;
	LCD_CS = 0;
//	SPIMasterModeSet();	// 启用硬件SPI (使用硬SPI初始化LCD); 使用软SPI初始化能看到效果(硬SPI太快,看不到)
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
	SPI_Send_DAT(0x32); //Vcom=1.35V
	SPI_Send_CMD(0xC2);
	SPI_Send_DAT(0x01);
	SPI_Send_CMD(0xC3);
	SPI_Send_DAT(0x15); //GVDD=4.8V  颜色深度
	SPI_Send_CMD(0xC4);
	SPI_Send_DAT(0x20); //VDV, 0x20:0v
	SPI_Send_CMD(0xC6);
	SPI_Send_DAT(0x0F); //0x0F:60Hz
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
        UINT8 v,h;
        for(v=240; v; --v)
        {
            for(h=240; h; --h)
            {
				SPI_Send_DAT(0x00);
				SPI_Send_DAT(0x00);
            }
        }
    }
//	SPI0_CTRL = 0x02;	// 关闭硬件SPI
}
#endif