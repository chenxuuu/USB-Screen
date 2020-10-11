#ifndef	__SPI_H__
#define	__SPI_H__

sbit SPI_DC = P3 ^ 1;					// 数据&命令(低电平写命令,高电平写数据)
sbit SPI_CS = P1 ^ 4;					// 片选;低电平有效
sbit SPI_SCL = P1 ^ 7;					// SPI时钟
sbit SPI_SDA = P1 ^ 5;					// SPI数据(MOSI)

// 初始化SPI总线<禁用SPI总线上拉(注意需要外接3.3V弱上拉10K电阻)>
#ifndef SPI_INIT
	#define SPI_INIT(); {P1_DIR_PU=0x1F;SPI0_CK_SE=0x02;}
#endif

// 硬件SPI模式配置(0x60模式0;0x68模式1;0x02关闭硬件SPI)
#ifndef SPI_MODE	
	#define SPI_MODE(x); SPI0_CTRL=(x);		
#endif

// 硬件SPI写1Byte数据
#ifndef SPI_DAT
	#define SPI_DAT(x); {SPI0_DATA=(x);while(S0_FREE==0);}
#endif

// 硬件SPI写1Byte命令
#ifndef SPI_CMD
	#define SPI_CMD(x); {SPI_DC=0;SPI0_DATA=(x);while(S0_FREE==0);SPI_DC=1;}
#endif

// 软件SPI写1Byte数据
extern void SPI_Send_DAT(UINT8 dat);
// 软件SPI写1Byte命令
extern void SPI_Send_CMD(UINT8 cmd);

#endif