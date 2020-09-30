
/********************************** (C) COPYRIGHT *******************************
* File Name          :CompatibilityHID.C
* Author             : WCH
* Version            : V1.2
* Date               : 2018/02/28
* Description        : CH554模拟HID兼容设备，支持中断上下传，支持控制端点上下传，支持设置全速，低速
*******************************************************************************/

//垃圾屏没法用硬件spi，太快了
#define SOFT_SPI

#include "CH552.H"
#include "Debug.H"
#ifdef SOFT_SPI
#else
#include "SPI.H"
#endif
#include <stdio.h>
#include <string.h>


sbit spi_reset = P3^2;
sbit spi_cs = P1^4;
sbit spi_dc = P3^1;
sbit spi_led = P3^4;
sbit led1=P3^0;
sbit led2=P3^3;
sbit led3=P1^1;

#ifdef SOFT_SPI
sbit sclk = P1^7;
sbit mosi = P1^5;
#endif

void LCD_WR_DATA8(UINT8 dat)
{

    spi_cs = 0;
    //
    #ifdef SOFT_SPI
    {
        UINT8 i;
        for(i=0;i<8;i++)
        {
            sclk=0;
            if(dat&0x80)
            {
                mosi=1;
            }
            else
            {
                mosi=0;
            }
            sclk=1;
            dat<<=1;
        }
    }
    #else
        CH554SPIMasterWrite(dat);
    #endif
    spi_cs = 1;
}
void LCD_WR_DATA(UINT16 dat)
{
    LCD_WR_DATA8(dat>>8);
    LCD_WR_DATA8(dat);
}
void LCD_WR_REG(UINT8 dat)
{
    spi_dc = 0;
    LCD_WR_DATA8(dat);
    spi_dc = 1;
}
void LCD_Address_Set(UINT16 x1,UINT16 y1,UINT16 x2,UINT16 y2)
{
	LCD_WR_REG(0x2a);//列地址设置
	LCD_WR_DATA(x1);
	LCD_WR_DATA(x2);
	LCD_WR_REG(0x2b);//行地址设置
	LCD_WR_DATA(y1);
	LCD_WR_DATA(y2);
	LCD_WR_REG(0x2c);//储存器写
}

#define USE_HORIZONTAL = 0

#define Fullspeed               1

#ifdef  Fullspeed
#define THIS_ENDP0_SIZE         64
#else
#define THIS_ENDP0_SIZE         8                                                  //低速USB，中断传输、控制传输最大包长度为8
#endif
UINT8X  Ep0Buffer[8>(THIS_ENDP0_SIZE+2)?8:(THIS_ENDP0_SIZE+2)] _at_ 0x0000;        //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X  Ep2Buffer[128>(2*MAX_PACKET_SIZE+4)?128:(2*MAX_PACKET_SIZE+4)] _at_ 0x0044;//端点2 IN&OUT缓冲区,必须是偶地址
UINT8   SetupReq,SetupLen,Ready,Count,FLAG,UsbConfig;
PUINT8  pDescr;                                                                    //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                       //暂存Setup包
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)

sbit Ep2InKey = P1^5;                                                              //K1按键
#pragma  NOAREGS
/*设备描述符*/
UINT8C DevDesc[18] = {0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                      0x33,0x23/*<--vid*/,0x34,0x24/*<--pid*/,0x00,0x00,0x00,0x00,
                      0x00,0x01
                     };
UINT8C CfgDesc[41] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //配置描述符
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x05,               //接口描述符
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,0x22,0x00,               //HID类描述符
#ifdef  Fullspeed
    0x07,0x05,0x82,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //端点描述符(全速间隔时间改成1ms)
    0x07,0x05,0x02,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //端点描述符
#else
    0x07,0x05,0x82,0x03,THIS_ENDP0_SIZE,0x00,0x0A,              //端点描述符(低速间隔时间最小10ms)
    0x07,0x05,0x02,0x03,THIS_ENDP0_SIZE,0x00,0x0A,              //端点描述符
#endif
};
/*字符串描述符 略*/

/*HID类报表描述符*/
UINT8C HIDRepDesc[ ] =
{
    0x06, 0x00,0xff,
    0x09, 0x01,
    0xa1, 0x01,                                                   //集合开始
    0x09, 0x02,                                                   //Usage Page  用法
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x81, 0x06,                                                   //Input
    0x09, 0x02,                                                   //Usage Page  用法
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x91, 0x06,                                                   //Output
    0xC0
};
// unsigned char  code LangDes[]={0x04,0x03,0x09,0x04};           //语言描述符
// unsigned char  code SerDes[]={
//                           0x28,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//                           0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
//                           0x00,0x00,0x00,0x00,0x00,0x49,0x00,0x43,0x00,0x42,
//                           0x00,0x43,0x00,0x31,0x00,0x00,0x00,0x00,0x00,0x00
//                           };                                   //字符串描述符

UINT8X UserEp2Buf[64];                                            //用户数据定义
UINT8 Endp2Busy = 0;

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
	IE_USB = 0;
	USB_CTRL = 0x00;                                                           // 先设定USB设备模式
	UDEV_CTRL = bUD_PD_DIS;                                                    // 禁止DP/DM下拉电阻
#ifndef Fullspeed
    UDEV_CTRL |= bUD_LOW_SPEED;                                                //选择低速1.5M模式
    USB_CTRL |= bUC_LOW_SPEED;
#else
    UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //选择全速12M模式，默认方式
    USB_CTRL &= ~bUC_LOW_SPEED;
#endif
    UEP2_DMA = Ep2Buffer;                                                      //端点2数据传输地址
    UEP2_3_MOD |= bUEP2_TX_EN | bUEP2_RX_EN;                                   //端点2发送接收使能
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //端点2收发各64字节缓冲区
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK | UEP_R_RES_ACK;                 //端点2自动翻转同步标志位，IN事务返回NAK，OUT返回ACK
    UEP0_DMA = Ep0Buffer;                                                      //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 //OUT事务返回ACK，IN事务返回NAK

	USB_DEV_AD = 0x00;
	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                     // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;                                                  // 允许USB端口
	USB_INT_FG = 0xFF;                                                         // 清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}

/*******************************************************************************
* Function Name  : Enp2BlukIn()
* Description    : USB设备模式端点2的批量上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2BlukIn( )
{
    memcpy( Ep2Buffer+MAX_PACKET_SIZE, UserEp2Buf, sizeof(UserEp2Buf));        //加载上传数据
    UEP2_T_LEN = THIS_ENDP0_SIZE;                                              //上传最大包长度
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //有数据时上传数据并应答ACK
}

/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
void    DeviceInterrupt( void ) interrupt INT_NO_USB using 1                    //USB中断服务程序,使用寄存器组1
{
    UINT8 len,i,type,count;
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# 端点批量上传
            UEP2_T_LEN = 0;                                                    //预使用发送长度一定要清空
//            UEP1_CTRL ^= bUEP_T_TOG;                                          //如果不设置自动翻转则需要手动翻转
            Endp2Busy = 0 ;
			UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            break;
        case UIS_TOKEN_OUT | 2:                                                 //endpoint 2# 端点批量下传
            if ( U_TOG_OK )                                                     // 不同步的数据包将丢弃
            {
                len = USB_RX_LEN;                                               //接收数据长度，数据从Ep2Buffer首地址开始存放
                type = 0;//存储命令类型
                count = 0;//存储剩余字节数
                /*
                    hid --> spi 协议
                    头：命令类型  后面跟的字节长度
                        1bit     7bit
                    然后接上后面的数据
                */
                for ( i = 0; i < len; i ++ )
                {
                    if(count==0)
                    {
                        count = Ep2Buffer[i] & 0x7F;
                        type = Ep2Buffer[i] >> 7;
                        Ep2Buffer[MAX_PACKET_SIZE+i] = 0x00;
                    }
                    else
                    {
                        if(type)//1表示data
                            LCD_WR_DATA8(Ep2Buffer[i]);
                        else//0表示命令
                            LCD_WR_REG(Ep2Buffer[i]);
                        count--;
                        Ep2Buffer[MAX_PACKET_SIZE+i] = 0xcc;
                    }
                }
                UEP2_T_LEN = len;
                UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;       // 允许上传
            }
            break;
        case UIS_TOKEN_SETUP | 0:                                               //SETUP事务
            len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = UsbSetupBuf->wLengthL;
                len = 0;                                                         // 默认为成功并且上传0长度
                SetupReq = UsbSetupBuf->bRequest;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/*HID类命令*/
                {
					switch( SetupReq )
					{
						case 0x01:                                                  //GetReport
							pDescr = UserEp2Buf;                                    //控制端点上传输据
							if(SetupLen >= THIS_ENDP0_SIZE)                         //大于端点0大小，需要特殊处理
							{
								len = THIS_ENDP0_SIZE;
							}
							else
							{
								len = SetupLen;
							}
							break;
						case 0x02:                                                   //GetIdle
							break;
						case 0x03:                                                   //GetProtocol
							break;
						case 0x09:                                                   //SetReport
							break;
						case 0x0A:                                                   //SetIdle
							break;
						case 0x0B:                                                   //SetProtocol
							break;
						default:
							len = 0xFF;  				                              /*命令不支持*/
							break;
					}
					if( SetupLen > len )
					{
						SetupLen = len;    //限制总长度
					}
					len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;   //本次传输长度
					memcpy(Ep0Buffer,pDescr,len);                                     //加载上传数据
					SetupLen -= len;
					pDescr += len;
                }
                else                                                             //标准请求
                {
                    switch(SetupReq)                                             //请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch(UsbSetupBuf->wValueH)
                        {
                        case 1:                                                  //设备描述符
                            pDescr = DevDesc;                                    //把设备描述符送到要发送的缓冲区
                            len = sizeof(DevDesc);
                            break;
                        case 2:                                                  //配置描述符
                            pDescr = CfgDesc;                                    //把设备描述符送到要发送的缓冲区
                            len = sizeof(CfgDesc);
                            break;
                        case 0x22:                                               //报表描述符
                            pDescr = HIDRepDesc;                                 //数据准备上传
                            len = sizeof(HIDRepDesc);
                            break;
                        default:
                            len = 0xff;                                          //不支持的命令或者出错
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;    //限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;//本次传输长度
                        memcpy(Ep0Buffer,pDescr,len);                            //加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;                         //暂存USB设备地址
                        break;
                    case USB_GET_CONFIGURATION:
                        Ep0Buffer[0] = UsbConfig;
                        if ( SetupLen >= 1 )
                        {
                            len = 1;
                        }
                        break;
                    case USB_SET_CONFIGURATION:
                        UsbConfig = UsbSetupBuf->wValueL;
						if(UsbConfig)
						{
							Ready = 1;                                            //set config命令一般代表usb枚举完成的标志
						}
                        break;
                    case 0x0A:
                        break;
                    case USB_CLEAR_FEATURE:                                      //Clear Feature
                        if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                        {
                            switch( UsbSetupBuf->wIndexL )
                            {
                            case 0x82:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x81:
                                UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                                break;
                            case 0x02:
                                UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                                break;
                            default:
                                len = 0xFF;                                       // 不支持的端点
                                break;
                            }
                        }
                        else
                        {
                            len = 0xFF;                                           // 不是端点不支持
                        }
                        break;
                    case USB_SET_FEATURE:                                         /* Set Feature */
                        if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )        /* 设置设备 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                            {
                                if( CfgDesc[ 7 ] & 0x20 )
                                {
                                    /* 设置唤醒使能标志 */
                                }
                                else
                                {
                                    len = 0xFF;                                    /* 操作失败 */
                                }
                            }
                            else
                            {
                                len = 0xFF;                                        /* 操作失败 */
                            }
                        }
                        else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )    /* 设置端点 */
                        {
                            if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                            {
                                switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;
                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;
                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
                                    break;
                                default:
                                    len = 0xFF;                                     /* 操作失败 */
                                    break;
                                }
                            }
                            else
                            {
                                len = 0xFF;                                         /* 操作失败 */
                            }
                        }
                        else
                        {
                            len = 0xFF;                                             /* 操作失败 */
                        }
                        break;
                    case USB_GET_STATUS:
                        Ep0Buffer[0] = 0x00;
                        Ep0Buffer[1] = 0x00;
                        if ( SetupLen >= 2 )
                        {
                            len = 2;
                        }
                        else
                        {
                            len = SetupLen;
                        }
                        break;
                    default:
                        len = 0xff;                                                  //操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xff;                                                          //包长度错误
            }
            if(len == 0xff)
            {
                SetupReq = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len <= THIS_ENDP0_SIZE)                                         //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                                      //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
            case HID_GET_REPORT:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                                   //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                            //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                                      //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            if(SetupReq == 0x09)
            {
                if(Ep0Buffer[0])
                {
                    //printf("Light on Num Lock LED!\n");
                }
                else if(Ep0Buffer[0] == 0)
                {
                    //printf("Light off Num Lock LED!\n");
                }
            }
            UEP0_CTRL ^= bUEP_R_TOG;                                     //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                           //写0清空中断
    }
    if(UIF_BUS_RST)                                                                 //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
		Endp2Busy = 0;
        UIF_BUS_RST = 0;                                                             //清中断标志
    }
    if (UIF_SUSPEND)                                                                 //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                             //挂起
        {
#ifdef DE_PRINTF
            //printf( "zz" );                                                          //睡眠状态
#endif
//             while ( XBUS_AUX & bUART0_TX )
//             {
//                 ;    //等待发送完成
//             }
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                                   //USB或者RXD0有信号时可被唤醒
//             PCON |= PD;                                                               //睡眠
//             SAFE_MOD = 0x55;
//             SAFE_MOD = 0xAA;
//             WAKE_CTRL = 0x00;
        }
    }
    else {                                                                             //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                                             //清中断标志
//      printf("UnknownInt  N");
    }
}



main()
{
    UINT8 i;
    led1=0;led2=0;led3=0;
    CfgFsys( );                                                           //CH559时钟选择配置
    mDelaymS(5);                                                          //修改主频等待内部晶振稳定,必加
    for(i=0; i<64; i++)                                                   //准备演示数据
    {
        UserEp2Buf[i] = i;
    }
    USBDeviceInit();                                                      //USB设备模式初始化
    EA = 1;                                                               //允许单片机中断
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    FLAG = 0;
    Ready = 0;

#ifdef SOFT_SPI
    sclk=0;mosi=0;
#else
    SPIMasterModeSet(3);
    SPI_CK_SET(4);
#endif
    spi_reset = 0;spi_cs = 1;spi_dc = 1;
    mDelaymS(100);
    spi_reset = 1;
    mDelaymS(100);
    spi_led = 1;
    mDelaymS(100);
	LCD_WR_REG(0x11); //Sleep out
	mDelaymS(120);              //Delay 120ms
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x36);
	LCD_WR_DATA8(0x00);

	LCD_WR_REG(0x3A);
	LCD_WR_DATA8(0x05);

	LCD_WR_REG(0xB2);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x0C);
	LCD_WR_DATA8(0x00);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x33);

	LCD_WR_REG(0xB7);
	LCD_WR_DATA8(0x35);

	LCD_WR_REG(0xBB);
	LCD_WR_DATA8(0x32); //Vcom=1.35V

	LCD_WR_REG(0xC2);
	LCD_WR_DATA8(0x01);

	LCD_WR_REG(0xC3);
	LCD_WR_DATA8(0x15); //GVDD=4.8V  颜色深度

	LCD_WR_REG(0xC4);
	LCD_WR_DATA8(0x20); //VDV, 0x20:0v

	LCD_WR_REG(0xC6);
	LCD_WR_DATA8(0x0F); //0x0F:60Hz

	LCD_WR_REG(0xD0);
	LCD_WR_DATA8(0xA4);
	LCD_WR_DATA8(0xA1);

	LCD_WR_REG(0xE0);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x34);

	LCD_WR_REG(0xE1);
	LCD_WR_DATA8(0xD0);
	LCD_WR_DATA8(0x08);
	LCD_WR_DATA8(0x0E);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x09);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x33);
	LCD_WR_DATA8(0x48);
	LCD_WR_DATA8(0x17);
	LCD_WR_DATA8(0x14);
	LCD_WR_DATA8(0x15);
	LCD_WR_DATA8(0x31);
	LCD_WR_DATA8(0x34);
	LCD_WR_REG(0x21);

	LCD_WR_REG(0x29);

    {
        UINT16 i,j,xsta=0,ysta=0,xend=240,yend=240;
        LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
        for(i=ysta;i<yend;i++)
        {
            for(j=xsta;j<xend;j++)
            {
                LCD_WR_DATA(0x0000);
            }
        }
    }

    while(1)
    {
        mDelaymS(100);
        // led1=0;led2=1;led3=0;
        // mDelaymS(500);
        // led1=1;led2=0;led3=1;
        // mDelaymS(500);
    }

}
