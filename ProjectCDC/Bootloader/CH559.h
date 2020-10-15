/*--------------------------------------------------------------------------
CH559.H
Header file for CH559 microcontrollers.
****************************************
**  Copyright  (C)  W.ch  1999-2014   **
**  Web:              http://wch.cn   **
****************************************
--------------------------------------------------------------------------*/

// 3 blocks: __BASE_TYPE__, __CH559_H__, __USB_DEF__
// define NO_XSFR_DEFINE before include CH559.H to disable xSFR define xdata address by _at_

#ifndef __BASE_TYPE__
#define __BASE_TYPE__



/*----- constant and type define -----------------------------------------*/

#ifndef TRUE
#define TRUE    1
#define FALSE   0
#endif
#ifndef NULL
#define NULL    0
#endif

#ifndef BOOL
typedef bit                             BOOL;
#endif
#ifndef UINT8
typedef unsigned char                   UINT8;
#endif
#ifndef UINT16
typedef unsigned short                  UINT16;
#endif
#ifndef UINT32
typedef unsigned long                   UINT32;
#endif
#ifndef UINT8D
typedef unsigned char  data             UINT8D;
#endif
#ifndef UINT16D
typedef unsigned short data             UINT16D;
#endif
#ifndef UINT32D
typedef unsigned long  data             UINT32D;
#endif
#ifndef UINT8I
typedef unsigned char  idata            UINT8I;
#endif
#ifndef UINT16I
typedef unsigned short idata            UINT16I;
#endif
#ifndef UINT32I
typedef unsigned long  idata            UINT32I;
#endif
#ifndef UINT8X
typedef unsigned char  xdata            UINT8X;
#endif
#ifndef UINT16X
typedef unsigned short xdata            UINT16X;
#endif
#ifndef UINT32X
typedef unsigned long  xdata            UINT32X;
#endif
#ifndef UINT8V
typedef unsigned char volatile          UINT8V;
#endif
#ifndef UINT8DV
typedef unsigned char volatile data     UINT8DV;
#endif
#ifndef UINT8XV
typedef unsigned char volatile xdata    UINT8XV;
#endif
#ifndef UINT8PV
typedef unsigned char volatile pdata    UINT8PV;
#endif
#ifndef UINT8C
typedef const unsigned char code        UINT8C;
#endif
#ifndef PUINT8
typedef unsigned char                 *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short                *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long                 *PUINT32;
#endif
#ifndef PUINT8I
typedef unsigned char  idata          *PUINT8I;
#endif
#ifndef PUINT16I
typedef unsigned short idata          *PUINT16I;
#endif
#ifndef PUINT32I
typedef unsigned long  idata          *PUINT32I;
#endif
#ifndef PUINT8X
typedef unsigned char  xdata          *PUINT8X;
#endif
#ifndef PUINT16X
typedef unsigned short xdata          *PUINT16X;
#endif
#ifndef PUINT32X
typedef unsigned long  xdata          *PUINT32X;
#endif
#ifndef PUINT8V
typedef unsigned char volatile        *PUINT8V;
#endif
#ifndef PUINT8DV
typedef unsigned char volatile data   *PUINT8DV;
#endif
#ifndef PUINT8XV
typedef unsigned char volatile xdata  *PUINT8XV;
#endif
#ifndef PUINT8PV
typedef unsigned char volatile pdata  *PUINT8PV;
#endif
#ifndef PUINT8C
typedef const unsigned char code      *PUINT8C;
#endif

#ifndef STRUCT_OFFSET
#define STRUCT_OFFSET( s, m )       ( (UINT8)( & (((s) *)0) -> (m) ) )  /* get the offset address for a member of a structure */
#endif



#endif  // __BASE_TYPE__

#ifndef __CH559_H__
#define __CH559_H__



/*----- SFR --------------------------------------------------------------*/
/*  sbit are bit addressable, others are byte addressable */

/*  System Registers  */
sfr PSW             = 0xD0;         // program status word
 sbit CY            = PSW^7;        // carry flag
 sbit AC            = PSW^6;        // auxiliary carry flag
 sbit F0            = PSW^5;        // bit addressable general purpose flag 0
 sbit RS1           = PSW^4;        // register R0-R7 bank selection high bit
 sbit RS0           = PSW^3;        // register R0-R7 bank selection low bit
#define MASK_PSW_RS       0x18      // bit mask of register R0-R7 bank selection
// RS1 & RS0: register R0-R7 bank selection
//    00 - bank 0, R0-R7 @ address 0x00-0x07
//    01 - bank 1, R0-R7 @ address 0x08-0x0F
//    10 - bank 2, R0-R7 @ address 0x10-0x17
//    11 - bank 3, R0-R7 @ address 0x18-0x1F
 sbit OV            = PSW^2;        // overflow flag
 sbit F1            = PSW^1;        // bit addressable general purpose flag 1
 sbit P             = PSW^0;        // ReadOnly: parity flag
sfr ACC             = 0xE0;         // accumulator
sfr B               = 0xF0;         // general purpose register B
sfr SP              = 0x81;         // stack pointer
//sfr16 DPTR          = 0x82;         // DPTR pointer, little-endian
sfr DPL             = 0x82;         // data pointer low
sfr DPH             = 0x83;         // data pointer high
sfr SAFE_MOD        = 0xA1;         // WriteOnly: writing safe mode
//sfr CHIP_ID         = 0xA1;         // ReadOnly: reading chip ID
#define CHIP_ID           SAFE_MOD
sfr GLOBAL_CFG      = 0xB1;         // global config, Write@SafeMode
#define bBOOT_LOAD        0x20      // ReadOnly: boot loader status for discriminating BootLoader or Application: set 1 by power on reset, clear 0 by software reset
#define bSW_RESET         0x10      // software reset bit, auto clear by hardware
#define bCODE_WE          0x08      // enable flash-ROM (include code & data area) being program or erasing: 0=writing protect, 1=enable program and erase
#define bDATA_WE          0x04      // enable Data-Flash (flash-ROM data area) being program or erasing: 0=writing protect, 1=enable program and erase
#define bXIR_XSFR         0x02      // force MOVX_@R0/@R1 only for xSFR area: 0=MOVX_@R0/@R1 for standard xdata area inclde xRAM&xBUS&xSFR, 1=MOVX_@R0/@R1 for xSFR only
#define bWDOG_EN          0x01      // enable watch-dog reset if watch-dog timer overflow: 0=as timer only, 1=enable reset if timer overflow

/* Clock and Sleep and Power Registers */
sfr PCON            = 0x87;         // power control and reset flag
#define SMOD              0x80      // baud rate selection for UART0 mode 1/2/3: 0=slow(Fsys/128 @mode2, TF1/32 @mode1/3, no effect for TF2),
                                    //   1=fast(Fsys/32 @mode2, TF1/16 @mode1/3, no effect for TF2)
#define bRST_FLAG1        0x20      // ReadOnly: recent reset flag high bit
#define bRST_FLAG0        0x10      // ReadOnly: recent reset flag low bit
#define MASK_RST_FLAG     0x30      // ReadOnly: bit mask of recent reset flag
#define RST_FLAG_SW       0x00
#define RST_FLAG_POR      0x10
#define RST_FLAG_WDOG     0x20
#define RST_FLAG_PIN      0x30
// bPC_RST_FLAG1 & bPC_RST_FLAG0: recent reset flag
//    00 - software reset, by bSW_RESET=1 @(bBOOT_LOAD=0 or bWDOG_EN=1)
//    01 - power on reset
//    10 - watch-dog timer overflow reset
//    11 - external input manual reset by RST pin
#define GF1               0x08      // general purpose flag bit 1
#define GF0               0x04      // general purpose flag bit 0
#define PD                0x02      // power-down enable bit, auto clear by wake-up hardware
sfr PLL_CFG         = 0xB2;         // PLL clock config: lower 5 bits for PLL clock Fpll, upper 3 bits for USB 4x clock Fusb4x, Write@SafeMode
#define MASK_PLL_MULT     0x1F      // bit mask of PLL clock Fpll multiple
#define MASK_USB_4X_DIV   0xE0      // bit mask of USB 4x clock Fusb4x divisor, value 000b means 1000b
sfr CLOCK_CFG       = 0xB3;         // system clock config: lower 5 bits for system clock Fsys, Write@SafeMode
#define bOSC_EN_INT       0x80      // internal oscillator enable and original clock selection: 1=enable & select internal clock, 0=disable & select external clock
#define bOSC_EN_XT        0x40      // external oscillator enable, need quartz crystal or ceramic resonator between X1 and X2 pins
#define bWDOG_IF_TO       0x20      // ReadOnly: watch-dog timer overflow interrupt flag, cleared by reload watch-dog count or auto cleared when MCU enter interrupt routine
#define MASK_SYS_CK_DIV   0x1F      // bit mask of system clock Fsys divisor, value 00000b means 100000b
/*
   Fxt = 4MHz~20MHz, from external oscillator @XI&XO
   Fosc = bOSC_EN_INT ? 12MHz : Fxt
   Fpll = Fosc * ( PLL_CFG & MASK_PLL_MULT ) => 288MHz (24MHz~350MHz)
   Kusb = ( PLL_CFG & MASK_USB_4X_DIV ) >> 5
   Fusb4x = Fpll / ( Kusb ? Kusb : 8 ) => 48MHz (Fixed)
   Ksys = CLOCK_CFG & MASK_SYS_CK_DIV
   Fsys = Fpll / ( Ksys ? Ksys : 32 ) => 12MHz (1MHz~56MHz)
*/
sfr SLEEP_CTRL      = 0xEA;         // sleep control, Write@SafeMode
#define bSLP_OFF_USB      0x80      // clock off for USB
#define bSLP_OFF_ADC      0x40      // clock off for ADC
#define bSLP_OFF_UART1    0x20      // clock off for UART1
#define bSLP_OFF_P1S1     0x10      // clock off for PWM1 / SPI1
#define bSLP_OFF_SPI0     0x08      // clock off for SPI0
#define bSLP_OFF_TMR3     0x04      // clock off for timer3
#define bSLP_OFF_LED      0x02      // clock off for LED
#define bSLP_OFF_XRAM     0x01      // clock off for xRAM
sfr WAKE_CTRL       = 0xEB;         // wake-up control, Write@SafeMode
#define bWAK_BY_USB       0x80      // enable wake-up by USB event
#define bWAK_RXD1_LO      0x40      // enable wake-up by RXD1 low level
#define bWAK_P1_5_LO      0x20      // enable wake-up by pin P1.5 low level
#define bWAK_P1_4_LO      0x10      // enable wake-up by pin P1.4 low level
#define bWAK_P0_3_LO      0x08      // enable wake-up by pin P0.3 low level
#define bWAK_CAP3_LO      0x04      // enable wake-up by CAP3 low level
#define bWAK_P3_2E_3L     0x02      // enable wake-up by pin P3.2 (INT0) edge or pin P3.3 (INT1) low level
#define bWAK_RXD0_LO      0x01      // enable wake-up by RXD0 low level
sfr RESET_KEEP      = 0xFE;         // value keeper during reset
sfr WDOG_COUNT      = 0xFF;         // watch-dog count, count by clock frequency Fsys/262144

/*  Interrupt Registers  */
sfr IE              = 0xA8;         // interrupt enable
 sbit EA            = IE^7;         // enable global interrupts: 0=disable, 1=enable if E_DIS=0
 sbit E_DIS         = IE^6;         // disable global interrupts, intend to inhibit interrupt during some flash-ROM operation: 0=enable if EA=1, 1=disable
 sbit ET2           = IE^5;         // enable timer2 interrupt
 sbit ES            = IE^4;         // enable UART0 interrupt
 sbit ET1           = IE^3;         // enable timer1 interrupt
 sbit EX1           = IE^2;         // enable external interrupt INT1
 sbit ET0           = IE^1;         // enable timer0 interrupt
 sbit EX0           = IE^0;         // enable external interrupt INT0 or LED interrupt
sfr IP              = 0xB8;         // interrupt priority and current priority
 sbit PH_FLAG       = IP^7;         // ReadOnly: high level priority action flag
 sbit PL_FLAG       = IP^6;         // ReadOnly: low level priority action flag
// PH_FLAG & PL_FLAG: current interrupt priority
//    00 - no interrupt now
//    01 - low level priority interrupt action now
//    10 - high level priority interrupt action now
//    11 - unknown error
 sbit PT2           = IP^5;         // timer2 interrupt priority level
 sbit PS            = IP^4;         // UART0 interrupt priority level
 sbit PT1           = IP^3;         // timer1 interrupt priority level
 sbit PX1           = IP^2;         // external interrupt INT1 priority level
 sbit PT0           = IP^1;         // timer0 interrupt priority level
 sbit PX0           = IP^0;         // external interrupt INT0 priority level
sfr IE_EX           = 0xE8;         // extend interrupt enable
 sbit IE_WDOG       = IE_EX^7;      // enable watch-dog timer interrupt
 sbit IE_GPIO       = IE_EX^6;      // enable GPIO input interrupt
 sbit IE_PWM1       = IE_EX^5;      // enable PWM1 interrupt
 sbit IE_UART1      = IE_EX^4;      // enable UART1 interrupt
 sbit IE_ADC        = IE_EX^3;      // enable ADC interrupt
 sbit IE_USB        = IE_EX^2;      // enable USB interrupt
 sbit IE_TMR3       = IE_EX^1;      // enable timer3 interrupt
 sbit IE_SPI0       = IE_EX^0;      // enable SPI0 interrupt
sfr IP_EX           = 0xE9;         // extend interrupt priority
#define bIP_LEVEL         0x80      // ReadOnly: current interrupt nested level: 0=no interrupt or two levels, 1=one level
#define bIP_GPIO          0x40      // GPIO input interrupt priority level
#define bIP_PWM1          0x20      // PWM1 interrupt priority level
#define bIP_UART1         0x10      // UART1 interrupt priority level
#define bIP_ADC           0x08      // ADC interrupt priority level
#define bIP_USB           0x04      // USB interrupt priority level
#define bIP_TMR3          0x02      // timer3 interrupt priority level
#define bIP_SPI0          0x01      // SPI0 interrupt priority level
sfr GPIO_IE         = 0xCF;         // GPIO interrupt enable
#define bIE_IO_EDGE       0x80      // enable GPIO edge interrupt: 0=low/high level, 1=falling/rising edge
#define bIE_RXD1_LO       0x40      // enable interrupt by RXD1 low level / falling edge
#define bIE_P5_5_HI       0x20      // enable interrupt by pin P5.5 high level / rising edge
#define bIE_P1_4_LO       0x10      // enable interrupt by pin P1.4 low level / falling edge
#define bIE_P0_3_LO       0x08      // enable interrupt by pin P0.3 low level / falling edge
#define bIE_P5_7_HI       0x04      // enable interrupt by pin P5.7 (RST) high level / rising edge
#define bIE_P4_1_LO       0x02      // enable interrupt by pin P4.1 low level / falling edge
#define bIE_RXD0_LO       0x01      // enable interrupt by RXD0 low level / falling edge

/*  FlashROM and Data-Flash Registers  */
sfr16 ROM_ADDR      = 0x84;         // address for flash-ROM, little-endian
sfr ROM_ADDR_L      = 0x84;         // address low byte for flash-ROM
sfr ROM_ADDR_H      = 0x85;         // address high byte for flash-ROM
sfr16 ROM_DATA      = 0x8E;         // data for flash-ROM writing, little-endian
sfr ROM_DATA_L      = 0x8E;         // data low byte for flash-ROM writing
sfr ROM_DATA_H      = 0x8F;         // data high byte for flash-ROM writing
sfr ROM_CTRL        = 0x86;         // WriteOnly: flash-ROM control
#define ROM_CMD_PROG      0x9A      // WriteOnly: flash-ROM word program operation command, for changing some ROM bit of a word from 1 to 0
#define ROM_CMD_ERASE     0xA6      // WriteOnly: flash-ROM sector erase operation command, for changing all ROM bit of 1KBytes from 0 to 1
//sfr ROM_STATUS      = 0x86;         // ReadOnly: flash-ROM status
#define ROM_STATUS        ROM_CTRL
#define bROM_ADDR_OK      0x40      // ReadOnly: flash-ROM operation address valid flag, can be reviewed before or after operation: 0=invalid parameter, 1=address valid
#define bROM_CMD_ERR      0x02      // ReadOnly: flash-ROM operation command error flag: 0=command accepted, 1=unknown command
#define bROM_CMD_TOUT     0x01      // ReadOnly: flash-ROM operation result: 0=success, 1=operation time out

/*  Port Registers  */
sfr P0              = 0x80;         // port 0 input & output
 sbit UDCD          = P0^7;         // DCD input for UART1
 sbit URI           = P0^6;         // RI input for UART1
 sbit UDSR          = P0^5;         // DSR input for UART1
 sbit UCTS          = P0^4;         // CTS input for UART1
 sbit TXD_          = P0^3;         // alternate pin for TXD of UART0
 sbit RXD_          = P0^2;         // alternate pin for RXD of UART0
 sbit URTS          = P0^1;         // RTS output for UART1
 sbit UDTR          = P0^0;         // DTR output for UART1
sfr P0_DIR          = 0xC4;         // port 0 direction
#define bUDCD             0x80      // DCD input for UART1
#define bURI              0x40      // RI input for UART1
#define bUDSR             0x20      // DSR input for UART1
#define bUCTS             0x10      // CTS input for UART1
#define bTXD_             0x08      // alternate pin for TXD of UART0
#define bRXD_             0x04      // alternate pin for RXD of UART0
#define bURTS             0x02      // RTS output for UART1
#define bUDTR             0x01      // DTR output for UART1
sfr P0_PU           = 0xC5;         // port 0 pullup enable
sfr P1              = 0x90;         // port 1 input & output, not 5VT
 sbit AIN7          = P1^7;         // AIN7 for ADC, not 5VT
 sbit AIN6          = P1^6;         // AIN6 for ADC, not 5VT
 sbit AIN5          = P1^5;         // AIN5 for ADC, not 5VT
 sbit AIN4          = P1^4;         // AIN4 for ADC, not 5VT
 sbit AIN3          = P1^3;         // AIN3 for ADC, not 5VT
 sbit AIN2          = P1^2;         // AIN2 for ADC, not 5VT
 sbit AIN1          = P1^1;         // AIN1 for ADC, not 5VT
 sbit AIN0          = P1^0;         // AIN0 for ADC, not 5VT
 sbit SCK           = P1^7;         // serial clock for SPI0, not 5VT
 sbit MISO          = P1^6;         // master serial data input or slave serial data output for SPI0, not 5VT
 sbit MOSI          = P1^5;         // master serial data output or slave serial data input for SPI0, not 5VT
 sbit SCS           = P1^4;         // slave chip-selection input for SPI0, not 5VT
 sbit PWM3          = P1^2;         // PWM output for timer3, not 5VT
 sbit CAP3          = P1^2;         // capture input for timer3, not 5VT
 sbit T2EX          = P1^1;         // external trigger input for timer2 reload & capture, not 5VT
 sbit CAP2          = P1^1;         // capture2 input for timer2, not 5VT
 sbit T2            = P1^0;         // external count input, not 5VT
 sbit CAP1          = P1^0;         // capture1 input for timer2, not 5VT
sfr P1_IE           = 0xB9;         // port 1 input enable
#define bSCK              0x80      // serial clock for SPI0
#define bMISO             0x40      // master serial data input or slave serial data output for SPI0
#define bMOSI             0x20      // master serial data output or slave serial data input for SPI0
#define bSCS              0x10      // slave chip-selection input for SPI0
#define bPWM3             0x04      // PWM output for timer3
#define bCAP3             bPWM3     // capture input for timer3
#define bT2EX             0x02      // external trigger input for timer2 reload & capture
#define bCAP2             bT2EX     // capture2 input for timer2
#define bT2               0x01      // external count input or clock output for timer2
#define bCAP1             bT2       // capture1 input for timer2
sfr P1_DIR          = 0xBA;         // port 1 direction
sfr P1_PU           = 0xBB;         // port 1 pullup enable
sfr P2              = 0xA0;         // port 2 input & output
 sbit TXD1          = P2^7;         // TXD output for UART1
 sbit DA7           = P2^7;         // address 7 output for direct low address mode
 sbit RXD1          = P2^6;         // RXD input for UART1
 sbit TNOW          = P2^5;         // tx now output for UART1, indicate transmitting
 sbit PWM2          = P2^5;         // second PWM output for PWM1
 sbit T2EX_         = P2^5;         // alternate pin for T2EX
 sbit CAP2_         = P2^5;         // alternate pin for CAP2
 sbit PWM1          = P2^4;         // PWM output for PWM1
 sbit SCK1          = P2^3;         // serial clock output for SPI1
 sbit MISO1         = P2^2;         // master serial data input for SPI1
 sbit MOSI1         = P2^1;         // master serial data output for SPI1
sfr P2_DIR          = 0xBC;         // port 2 direction
#define bTXD1             0x80      // TXD output for UART1
#define bDA7              0x80      // address 7 output for direct low address mode
#define bRXD1             0x40      // RXD input for UART1
#define bTNOW             0x20      // tx now output for UART1, indicate transmitting
#define bPWM2             0x20      // second PWM output for PWM1
#define bT2EX_            0x20      // alternate pin for T2EX
#define bCAP2_            bT2EX_    // alternate pin for CAP2
#define bPWM1             0x10      // PWM output for PWM1
#define bSCK1             0x08      // serial clock output for SPI1
#define bMISO1            0x04      // master serial data input for SPI1
#define bMOSI1            0x02      // master serial data output for SPI1
sfr P2_PU           = 0xBD;         // port 2 pullup enable
sfr P3              = 0xB0;         // port 3 input & output
 sbit RD            = P3^7;         // xdata or xBUS read strobe output
 sbit WR            = P3^6;         // xdata or xBUS write strobe output
 sbit DA6           = P3^5;         // address 6 output for direct low address mode
 sbit T1            = P3^5;         // external count input for timer1
 sbit LEDC          = P3^4;         // LEDC clock output for LED
 sbit XCS0          = P3^4;         // xBUS chip-selection 0# output, for address range 0x4000~0x7FFF
 sbit T0            = P3^4;         // external count input for timer0
 sbit LED1          = P3^3;         // LED1 data output
 sbit INT1          = P3^3;         // external interrupt 1 input
 sbit LED0          = P3^2;         // LED0 data output
 sbit INT0          = P3^2;         // external interrupt 0 input
 sbit TXD           = P3^1;         // TXD output for UART0
 sbit RXD           = P3^0;         // RXD input for UART0
sfr P3_DIR          = 0xBE;         // port 3 direction
#define bRD               0x80      // xdata or xBUS read strobe output
#define bWR               0x40      // xdata or xBUS write strobe output
#define bDA6              0x20      // address 6 output for direct low address mode
#define bT1               0x20      // external count input for timer1
#define bLEDC             0x10      // LEDC clock output for LED
#define bXCS0             0x10      // xBUS chip-selection 0# output, for address range 0x4000~0x7FFF
#define bT0               0x10      // external count input for timer0
#define bLED1             0x08      // LED1 data output
#define bINT1             0x08      // external interrupt 1 input
#define bLED0             0x04      // LED0 data output
#define bINT0             0x04      // external interrupt 0 input
#define bTXD              0x02      // TXD output for UART0
#define bRXD              0x01      // RXD input for UART0
sfr P3_PU           = 0xBF;         // port 3 pullup enable
sfr P4_OUT          = 0xC0;         // port 4 output
 sbit SCK_          = P4_OUT^7;     // alternate pin for SCK
 sbit SCS_          = P4_OUT^6;     // alternate pin for SCS
 sbit PWM2_         = P4_OUT^5;     // alternate pin for PWM2
 sbit LED3          = P4_OUT^4;     // LED3 data output
 sbit TNOW_         = P4_OUT^4;     // alternate pin for TNOW
 sbit TXD1_         = P4_OUT^4;     // alternate pin for TXD1
 sbit PWM1_         = P4_OUT^3;     // alternate pin for PWM1
 sbit PWM3_         = P4_OUT^2;     // alternate pin for PWM3
 sbit CAP3_         = P4_OUT^2;     // alternate pin for CAP3
 sbit LED2          = P4_OUT^0;     // LED2 data output
 sbit RXD1_         = P4_OUT^0;     // alternate pin for RXD1
sfr P4_IN           = 0xC1;         // ReadOnly: port 4 input
#define bSCK_             0x80      // alternate pin for SCK, not 5VT
#define bSCS_             0x40      // alternate pin for SCS, not 5VT
#define bPWM2_            0x20      // alternate pin for PWM2
#define bLED3             0x10      // LED3 data output
#define bTNOW_            0x10      // alternate pin for TNOW
#define bTXD1_            bTNOW_    // alternate pin for TXD1
#define bPWM1_            0x08      // alternate pin for PWM1
#define bPWM3_            0x04      // alternate pin for PWM3
#define bCAP3_            bPWM3_    // alternate pin for CAP3
#define bLED2             0x01      // LED2 data output
#define bRXD1_            0x01      // alternate pin for RXD1
sfr P4_DIR          = 0xC2;         // port 4 direction
sfr P4_PU           = 0xC3;         // port 4 pullup enable
sfr P5_IN           = 0xC7;         // ReadOnly: port 5 input
#define bRST              0x80      // ReadOnly: pin RST input, not 5VT
#define bIO_INT_ACT       0x40      // ReadOnly: GPIO interrupt request action status
#define bHP               0x20      // ReadOnly: pin HP input
#define bHM               0x10      // ReadOnly: pin HM input
#define bDP               0x02      // ReadOnly: pin DP input
#define bDM               0x01      // ReadOnly: pin DM input
//sfr P4_CFG          = 0xC7;         // port 4 config
#define P4_CFG            P5_IN
#define bSPI0_PIN_X       0x08      // SPI0 SCS/SCK alternate pin enable: 0=SCS/SCK on P1.4/P1.7, 1=SCS/SCK on P4.6/P4.7
#define bP4_DRV           0x04      // P4 driving capability: 0=5mA, 1=20mA
sfr PORT_CFG        = 0xC6;         // port 0/1/2/3 config
#define bP3_DRV           0x80      // P3 driving capability: 0=5mA, 1=20mA
#define bP2_DRV           0x40      // P2 driving capability: 0=5mA, 1=20mA
#define bP1_DRV           0x20      // P1 driving capability: 0=5mA, 1=10mA
#define bP0_DRV           0x10      // P0 driving capability: 0=5mA, 1=20mA
#define bP3_OC            0x08      // P3 open-drain output enable: 0=push-pull output, 1=open-drain output
#define bP2_OC            0x04      // P2 open-drain output enable: 0=push-pull output, 1=open-drain output
#define bP1_OC            0x02      // P1 open-drain output enable: 0=push-pull output, 1=open-drain output
#define bP0_OC            0x01      // P0 open-drain output enable: 0=push-pull output, 1=open-drain output
// bPn_OC & Pn_DIR & Pn_PU: pin input & output configuration for Pn (n=0/1/2/3)
//   0 0 0:  input only, without pullup resistance
//   0 0 1:  input only, with pullup resistance
//   0 1 x:  push-pull output, strong driving high level and low level
//   1 0 0:  open-drain output without pullup, support input
//   1 1 0:  open-drain output without pullup, support input, just driving high level strongly for 2 clocks if turning output level from low to high
//   1 0 1:  quasi-bidirectional (simulated 8051 mode), open-drain output with pullup resistance
//   1 1 1:  quasi-bidirectional (standard 8051 mode), open-drain output with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
sfr PIN_FUNC        = 0xCE;         // pin function selection
#define bPWM1_PIN_X       0x80      // PWM1/PWM2 alternate pin enable: 0=PWM1/PWM2 on P2.4/P2.5, 1=PWM1/PWM2 on P4.3/P4.5
#define bTMR3_PIN_X       0x40      // PWM3/CAP3 alternate pin enable: 0=PWM3/CAP3 on P1.2, 1=PWM3/CAP3 on P4.2
#define bT2EX_PIN_X       0x20      // T2EX/CAP2 alternate pin enable: 0=T2EX/CAP2 on P1.1, 1=T2EX/CAP2 on P2.5
#define bUART0_PIN_X      0x10      // UART0 alternate pin enable: 0=RXD0/TXD0 on P3.0/P3.1, 1=RXD0/TXD0 on P0.2/P0.3
#define bXBUS_EN          0x08      // xBUS function enable: 0=disable, 1=enable port 0 used as data bus and P3.6/P3.7 used as bus writing/reading strobe during accessing xBUS
#define bXBUS_CS_OE       0x04      // xBUS chip-selection output enable: 0=disable output,
                                    //   1=output CS0 (chip-selection 0#, low action) at P3.4, output inversion of address_15 (be equal to CS1, low action) at P3.3 if ALE disabled
#define bXBUS_AH_OE       0x02      // xBUS high address output enable: 0=disable output, 1=output address_8~15 at P2.0~P2.7 during accessing xBUS by MOVX_@DPTR instruction
#define bXBUS_AL_OE       0x01      // xBUS low address output enable: 0=multiplex low address and data bus during accessing xBUS, 1=output address_0~7 at P4.0~P4.5 & P3.5 & P2.7 directly
sfr XBUS_AUX        = 0xA2;         // xBUS auxiliary setting
#define bUART0_TX         0x80      // ReadOnly: indicate UART0 transmittal status
#define bUART0_RX         0x40      // ReadOnly: indicate UART0 receiving status
#define bSAFE_MOD_ACT     0x20      // ReadOnly: safe mode action status
#define bALE_CLK_EN       0x10      // enable ALE output 1/12 Fsys clock during non xBUS operation
#define GF2               0x08      // general purpose flag bit 2
#define bDPTR_AUTO_INC    0x04      // enable DPTR auto increase if finished MOVX_@DPTR instruction
#define DPS               0x01      // dual DPTR selection: 0=DPTR0 selected, 1=DPTR1 selected
sfr XBUS_SPEED      = 0xFD;         // xBUS speed config
#define bXBUS1_SETUP      0x80      // xBUS chip-selection 1# setup time: 0=2 clocks, 1=3 clocks
#define bXBUS1_HOLD       0x40      // xBUS chip-selection 1# hold time: 0=1 clocks, 1=2 clocks
#define bXBUS1_WIDTH1     0x20      // xBUS chip-selection 1# access pulse width high bit
#define bXBUS1_WIDTH0     0x10      // xBUS chip-selection 1# access pulse width low bit
#define MASK_XBUS1_WIDTH  0x30      // bit mask of xBUS chip-selection 1# access pulse width
// bXBUSn_WIDTH1 & bXBUSn_WIDTH0: read or write pulse width for xBUS chip-selection n# peripheral
//   00: 2 clocks
//   01: 4 clocks
//   10: 8 clocks
//   11: 16 clocks
#define bXBUS0_SETUP      0x08      // xBUS chip-selection 0# setup time: 0=2 clocks, 1=3 clocks
#define bXBUS0_HOLD       0x04      // xBUS chip-selection 0# hold time: 0=1 clocks, 1=2 clocks
#define bXBUS0_WIDTH1     0x02      // xBUS chip-selection 0# access pulse width high bit
#define bXBUS0_WIDTH0     0x01      // xBUS chip-selection 0# access pulse width low bit
#define MASK_XBUS0_WIDTH  0x03      // bit mask of xBUS chip-selection 0# access pulse width

/*  Timer0/1 Registers  */
sfr TCON            = 0x88;         // timer 0/1 control and external interrupt control
 sbit TF1           = TCON^7;       // timer1 overflow & interrupt flag, auto cleared when MCU enter interrupt routine
 sbit TR1           = TCON^6;       // timer1 run enable
 sbit TF0           = TCON^5;       // timer0 overflow & interrupt flag, auto cleared when MCU enter interrupt routine
 sbit TR0           = TCON^4;       // timer0 run enable
 sbit IE1           = TCON^3;       // INT1 interrupt flag, auto cleared when MCU enter interrupt routine
 sbit IT1           = TCON^2;       // INT1 interrupt type: 0=low level action, 1=falling edge action
 sbit IE0           = TCON^1;       // INT0 interrupt flag, auto cleared when MCU enter interrupt routine
 sbit IT0           = TCON^0;       // INT0 interrupt type: 0=low level action, 1=falling edge action
sfr TMOD            = 0x89;         // timer 0/1 mode
#define bT1_GATE          0x80      // gate control of timer1: 0=timer1 run enable while TR1=1, 1=timer1 run enable while P3.3 (INT1) pin is high and TR1=1
#define bT1_CT            0x40      // counter or timer mode selection for timer1: 0=timer, use internal clock, 1=counter, use P3.5 (T1) pin falling edge as clock
#define bT1_M1            0x20      // timer1 mode high bit
#define bT1_M0            0x10      // timer1 mode low bit
#define MASK_T1_MOD       0x30      // bit mask of timer1 mode
// bT1_M1 & bT1_M0: timer1 mode
//   00: mode 0, 13-bit timer or counter by cascaded TH1 and lower 5 bits of TL1, the upper 3 bits of TL1 are ignored
//   01: mode 1, 16-bit timer or counter by cascaded TH1 and TL1
//   10: mode 2, TL1 operates as 8-bit timer or counter, and TH1 provide initial value for TL1 auto-reload
//   11: mode 3, stop timer1
#define bT0_GATE          0x08      // gate control of timer0: 0=timer0 run enable while TR0=1, 1=timer0 run enable while P3.2 (INT0) pin is high and TR0=1
#define bT0_CT            0x04      // counter or timer mode selection for timer0: 0=timer, use internal clock, 1=counter, use P3.4 (T0) pin falling edge as clock
#define bT0_M1            0x02      // timer0 mode high bit
#define bT0_M0            0x01      // timer0 mode low bit
#define MASK_T0_MOD       0x03      // bit mask of timer0 mode
// bT0_M1 & bT0_M0: timer0 mode
//   00: mode 0, 13-bit timer or counter by cascaded TH0 and lower 5 bits of TL0, the upper 3 bits of TL0 are ignored
//   01: mode 1, 16-bit timer or counter by cascaded TH0 and TL0
//   10: mode 2, TL0 operates as 8-bit timer or counter, and TH0 provide initial value for TL0 auto-reload
//   11: mode 3, TL0 is 8-bit timer or counter controlled by standard timer0 bits, TH0 is 8-bit timer using TF1 and controlled by TR1, timer1 run enable if it is not mode 3
sfr TL0             = 0x8A;         // low byte of timer 0 count
sfr TL1             = 0x8B;         // high byte of timer 1 count
sfr TH0             = 0x8C;         // low byte of timer 0 count
sfr TH1             = 0x8D;         // high byte of timer 1 count

/*  UART0 Registers  */
sfr SCON            = 0x98;         // UART0 control (serial port control)
 sbit SM0           = SCON^7;       // UART0 mode bit0, selection data bit: 0=8 bits data, 1=9 bits data
 sbit SM1           = SCON^6;       // UART0 mode bit1, selection baud rate: 0=fixed, 1=variable
// SM0 & SM1: UART0 mode
//    00 - mode 0, shift Register, baud rate fixed at: Fsys/12
//    01 - mode 1, 8-bit UART,     baud rate = variable by timer1 or timer2 overflow rate
//    10 - mode 2, 9-bit UART,     baud rate fixed at: Fsys/128@SMOD=0, Fsys/32@SMOD=1
//    11 - mode 3, 9-bit UART,     baud rate = variable by timer1 or timer2 overflow rate
 sbit SM2           = SCON^5;       // enable multi-device communication in mode 2/3
#define MASK_UART0_MOD    0xE0      // bit mask of UART0 mode
 sbit REN           = SCON^4;       // enable UART0 receiving
 sbit TB8           = SCON^3;       // the 9th transmitted data bit in mode 2/3
 sbit RB8           = SCON^2;       // 9th data bit received in mode 2/3, or stop bit received for mode 1
 sbit TI            = SCON^1;       // transmit interrupt flag, set by hardware after completion of a serial transmittal, need software clear
 sbit RI            = SCON^0;       // receive interrupt flag, set by hardware after completion of a serial receiving, need software clear
sfr SBUF            = 0x99;         // UART0 data buffer: reading for receiving, writing for transmittal

/*  Timer2/Capture2 Registers  */
sfr T2CON           = 0xC8;         // timer 2 control
 sbit TF2           = T2CON^7;      // timer2 overflow & interrupt flag, need software clear, the flag will not be set when either RCLK=1 or TCLK=1
 sbit CAP1F         = T2CON^7;      // timer2 capture 1 interrupt flag, set by T2 edge trigger if bT2_CAP1_EN=1, need software clear
 sbit EXF2          = T2CON^6;      // timer2 external flag, set by T2EX edge trigger if EXEN2=1, need software clear
 sbit RCLK          = T2CON^5;      // selection UART0 receiving clock: 0=timer1 overflow pulse, 1=timer2 overflow pulse
 sbit TCLK          = T2CON^4;      // selection UART0 transmittal clock: 0=timer1 overflow pulse, 1=timer2 overflow pulse
 sbit EXEN2         = T2CON^3;      // enable T2EX trigger function: 0=ignore T2EX, 1=trigger reload or capture by T2EX edge
 sbit TR2           = T2CON^2;      // timer2 run enable
 sbit C_T2          = T2CON^1;      // timer2 clock source selection: 0=timer base internal clock, 1=external edge counter base T2 falling edge
 sbit CP_RL2        = T2CON^0;      // timer2 function selection (force 0 if RCLK=1 or TCLK=1): 0=timer and auto reload if count overflow or T2EX edge, 1=capture by T2EX edge
sfr T2MOD           = 0xC9;         // timer 2 mode and timer 0/1/2 clock mode
#define bTMR_CLK          0x80      // fastest internal clock mode for timer 0/1/2 under faster clock mode: 0=use divided clock, 1=use original Fsys as clock without dividing
#define bT2_CLK           0x40      // timer2 internal clock frequency selection: 0=standard clock, Fsys/12 for timer mode, Fsys/4 for UART0 clock mode,
                                    //   1=faster clock, Fsys/4 @bTMR_CLK=0 or Fsys @bTMR_CLK=1 for timer mode, Fsys/2 @bTMR_CLK=0 or Fsys @bTMR_CLK=1 for UART0 clock mode
#define bT1_CLK           0x20      // timer1 internal clock frequency selection: 0=standard clock, Fsys/12, 1=faster clock, Fsys/4 if bTMR_CLK=0 or Fsys if bTMR_CLK=1
#define bT0_CLK           0x10      // timer0 internal clock frequency selection: 0=standard clock, Fsys/12, 1=faster clock, Fsys/4 if bTMR_CLK=0 or Fsys if bTMR_CLK=1
#define bT2_CAP_M1        0x08      // timer2 capture mode high bit
#define bT2_CAP_M0        0x04      // timer2 capture mode low bit
// bT2_CAP_M1 & bT2_CAP_M0: timer2 capture point selection
//   x0: from falling edge to falling edge
//   01: from any edge to any edge (level changing)
//   11: from rising edge to rising edge
#define T2OE              0x02      // enable timer2 generated clock output: 0=disable output, 1=enable clock output at T2 pin, frequency = TF2/2
#define bT2_CAP1_EN       0x01      // enable T2 trigger function for capture 1 of timer2 if RCLK=0 & TCLK=0 & CP_RL2=1 & C_T2=0 & T2OE=0
sfr16 RCAP2         = 0xCA;         // reload & capture value, little-endian
sfr RCAP2L          = 0xCA;         // low byte of reload & capture value
sfr RCAP2H          = 0xCB;         // high byte of reload & capture value
sfr16 T2COUNT       = 0xCC;         // counter, little-endian
//sfr16 T2CAP1        = 0xCC;        // ReadOnly: capture 1 value for timer2
#define T2CAP1            T2COUNT
sfr TL2             = 0xCC;         // low byte of timer 2 count
//sfr T2CAP1L         = 0xCC;         // ReadOnly: capture 1 value low byte for timer2
#define T2CAP1L           TL2
sfr TH2             = 0xCD;         // high byte of timer 2 count
//sfr T2CAP1H         = 0xCD;         // ReadOnly: capture 1 value high byte for timer2
#define T2CAP1H           TH2

/*  Timer3/Capture3/PWM3 Registers  */
sfr T3_SETUP        = 0xA3;         // timer 3 setup
#define bT3_IE_END        0x80      // enable interrupt for capture mode count timeout (exceed end value) or PWM mode cycle end
#define bT3_IE_FIFO_OV    0x40      // enable interrupt for FIFO overflow
#define bT3_IE_FIFO_REQ   0x20      // enable interrupt for capture mode FIFO >=4 or PWM mode FIFO <=3
#define bT3_IE_ACT        0x10      // enable interrupt for capture mode input action or PWM mode trigger
#define bT3_CAP_IN        0x04      // ReadOnly: current capture input level after noise filtrating
#define bT3_CAP_CLK       0x02      // force no minimum pulse width limit for capture input if T3_CK_SE=1
#define bT3_EN_CK_SE      0x01      // enable to accessing divisor setting register, else enable to accessing current count register
sfr16 T3_COUNT      = 0xA4;         // ReadOnly: current count value, little-endian
sfr T3_COUNT_L      = 0xA4;         // ReadOnly: current count low byte
sfr T3_COUNT_H      = 0xA5;         // ReadOnly: current count high byte
//sfr16 T3_CK_SE      = 0xA4;         // clock divisor setting, little-endian, lower 12 bits valid only
#define T3_CK_SE          T3_COUNT
//sfr T3_CK_SE_L      = 0xA4;         // clock divisor setting low byte
#define T3_CK_SE_L        T3_COUNT_L
//sfr T3_CK_SE_H      = 0xA5;         // clock divisor setting high byte, lower 4 bits valid only
#define T3_CK_SE_H        T3_COUNT_H
sfr16 T3_END        = 0xA6;         // end value for count, little-endian
sfr T3_END_L        = 0xA6;         // low byte of end value for count
sfr T3_END_H        = 0xA7;         // high byte of end value for count
sfr T3_STAT         = 0xA9;         // timer 3 status
#define bT3_IF_DMA_END    0x80      // interrupt flag for DMA completion, write 1 to clear or write T3_DMA_CN to clear
#define bT3_IF_FIFO_OV    0x40      // interrupt flag for FIFO overflow, write 1 to clear
#define bT3_IF_FIFO_REQ   0x20      // interrupt flag for request FIFO data (capture mode FIFO >=4 or PWM mode FIFO <=3), write 1 to clear
#define bT3_IF_ACT        0x10      // interrupt flag for capture mode input action or PWM mode trigger if bT3_IE_ACT=1, write 1 to clear or accessing FIFO to clear
#define bT3_IF_END        0x10      // interrupt flag for capture mode count timeout (exceed end value) or PWM mode cycle end if bT3_IE_ACT=0, write 1 to clear
#define MASK_T3_FIFO_CNT  0x0F      // ReadOnly: bit mask of timer3 FIFO count
sfr T3_CTRL         = 0xAA;         // timer 3 control
#define bT3_CAP_M1        0x80      // timer3 capture mode high bit
#define bT3_CAP_M0        0x40      // timer3 capture mode low bit
// bT3_CAP_M1 & bT3_CAP_M0: timer3 capture point selection
//   00: disable capture
//   01: from any edge to any edge (level changing)
//   10: from falling edge to falling edge
//   11: from rising edge to rising edge
#define bT3_PWM_POLAR     0x20      // timer3 PWM output polarity: 0=default low and high action, 1=default high and low action
#define bT3_CAP_WIDTH     0x20      // minimum pulse width for timer3 capture: 0=4 divided clocks, 1=1 divided clock
#define bT3_DMA_EN        0x10      // DMA enable and DMA interrupt enable for timer3
#define bT3_OUT_EN        0x08      // timer3 output enable
#define bT3_CNT_EN        0x04      // timer3 count enable
#define bT3_CLR_ALL       0x02      // force clear FIFO and count of timer3
#define bT3_MOD_CAP       0x01      // timer3 mode: 0=timer or PWM, 1=capture
sfr T3_DMA_CN       = 0xAB;         // DMA remainder word count, automatic decreasing after DMA
sfr16 T3_DMA        = 0xAC;         // DMA address, must even address, little-endian, automatic increasing after DMA
sfr T3_DMA_AL       = 0xAC;         // DMA address low byte, automatic increasing after DMA
sfr T3_DMA_AH       = 0xAD;         // DMA address high byte, automatic increasing after DMA
sfr16 T3_FIFO       = 0xAE;         // FIFO word, little-endian
sfr T3_FIFO_L       = 0xAE;         // FIFO low byte
sfr T3_FIFO_H       = 0xAF;         // FIFO high byte

/*  PWM1/2 Registers  */
sfr PWM_DATA2       = 0x9B;         // PWM data for PWM2
sfr PWM_DATA        = 0x9C;         // PWM data for PWM1
sfr PWM_CTRL        = 0x9D;         // PWM 1/2 control
#define bPWM_IE_END       0x80      // enable interrupt for PWM mode cycle end or MFM empty buffer
#define bPWM2_POLAR       0x40      // PWM2 output polarity if bPWM_MOD_MFM=0: 0=default low and high action, 1=default high and low action
#define bMFM_BUF_EMPTY    0x40      // ReadOnly: MFM empty buffer status if bPWM_MOD_MFM=1
#define bPWM_POLAR        0x20      // PWM output polarity: 0=default low and high action, 1=default high and low action
#define bPWM_IF_END       0x10      // interrupt flag for cycle end, write 1 to clear or write PWM_CYCLE or load new data to clear
#define bPWM_OUT_EN       0x08      // PWM1 output enable
#define bPWM2_OUT_EN      0x04      // PWM2 output enable if bPWM_MOD_MFM=0
#define bMFM_BIT_CNT2     0x04      // ReadOnly: MFM encode bit count status if bPWM_MOD_MFM=1: 0=lower 4 bits, 1=upper 4 bits
#define bPWM_CLR_ALL      0x02      // force clear FIFO and count of PWM1/2
#define bPWM_MOD_MFM      0x01      // MFM encode mode for PWM: 0=PWM, 1=MFM encode
sfr PWM_CK_SE       = 0x9E;         // clock divisor setting
sfr PWM_CYCLE       = 0x9F;         // PWM cycle

/*  SPI0/Master0/Slave Registers  */
sfr SPI0_STAT       = 0xF8;         // SPI 0 status
 sbit S0_FST_ACT    = SPI0_STAT^7;  // ReadOnly: indicate first byte received status for SPI0
 sbit S0_IF_OV      = SPI0_STAT^6;  // interrupt flag for slave mode FIFO overflow, direct bit address clear or write 1 to clear
 sbit S0_IF_FIRST   = SPI0_STAT^5;  // interrupt flag for first byte received, direct bit address clear or write 1 to clear
 sbit S0_IF_BYTE    = SPI0_STAT^4;  // interrupt flag for a byte data exchanged, direct bit address clear or write 1 to clear or accessing FIFO to clear if bS0_AUTO_IF=1
 sbit S0_FREE       = SPI0_STAT^3;  // ReadOnly: SPI0 free status
 sbit S0_T_FIFO     = SPI0_STAT^2;  // ReadOnly: tx FIFO count for SPI0
 sbit S0_R_FIFO1    = SPI0_STAT^1;  // ReadOnly: rx FIFO count bit1 for SPI0
 sbit S0_R_FIFO0    = SPI0_STAT^0;  // ReadOnly: rx FIFO count bit0 for SPI0
#define MASK_S0_RFIFO_CNT 0x03      // ReadOnly: bit mask of SPI0 rx FIFO count
sfr SPI0_DATA       = 0xF9;         // FIFO data port: reading for receiving, writing for transmittal
sfr SPI0_CTRL       = 0xFA;         // SPI 0 control
#define bS0_MISO_OE       0x80      // SPI0 MISO output enable, this bit need not enable for slave mode, because MISO automatic output enable if chip selected
#define bS0_MOSI_OE       0x40      // SPI0 MOSI output enable
#define bS0_SCK_OE        0x20      // SPI0 SCK output enable
#define bS0_DATA_DIR      0x10      // SPI0 data direction: 0=out(master_write), 1=in(master_read)
#define bS0_MST_CLK       0x08      // SPI0 master clock mode: 0=mode 0 with default low, 1=mode 3 with default high
#define bS0_2_WIRE        0x04      // enable SPI0 two wire mode: 0=3 wire (SCK+MOSI+MISO), 1=2 wire (SCK+MISO)
#define bS0_CLR_ALL       0x02      // force clear FIFO and count of SPI0
#define bS0_AUTO_IF       0x01      // enable FIFO accessing to auto clear S0_IF_BYTE interrupt flag
sfr SPI0_CK_SE      = 0xFB;         // clock divisor setting
//sfr SPI0_S_PRE      = 0xFB;         // preset value for SPI slave
#define SPI0_S_PRE        SPI0_CK_SE
sfr SPI0_SETUP      = 0xFC;         // SPI 0 setup
#define bS0_MODE_SLV      0x80      // SPI0 slave mode: 0=master, 1=slave
#define bS0_IE_FIFO_OV    0x40      // enable interrupt for slave mode FIFO overflow
#define bS0_IE_FIRST      0x20      // enable interrupt for first byte received for SPI0 slave mode
#define bS0_IE_BYTE       0x10      // enable interrupt for a byte received
#define bS0_BIT_ORDER     0x08      // SPI0 bit data order: 0=MSB first, 1=LSB first
#define bS0_SLV_SELT      0x02      // ReadOnly: SPI0 slave mode chip selected status: 0=unselected, 1=selected
#define bS0_SLV_PRELOAD   0x01      // ReadOnly: SPI0 slave mode data pre-loading status just after chip-selection

/*  SPI1/Master1 Registers  */
sfr SPI1_STAT       = 0xB4;         // SPI 1 status
#define bS1_IF_BYTE       0x10      // interrupt flag for a byte data exchanged, write 1 to clear or accessing FIFO to clear if bS1_AUTO_IF=1
#define bS1_FREE          0x08      // ReadOnly: SPI1 free status
sfr SPI1_DATA       = 0xB5;         // data port: reading for receiving, writing for transmittal
sfr SPI1_CTRL       = 0xB6;         // SPI 1 control
#define bS1_MISO_OE       0x80      // SPI1 MISO output enable
#define bS1_SCK_OE        0x20      // SPI1 SCK output enable, MOSI output enable if bS1_2_WIRE=0
#define bS1_DATA_DIR      0x10      // SPI1 data direction: 0=out(master_write), 1=in(master_read)
#define bS1_MST_CLK       0x08      // SPI1 master clock mode: 0=mode 0 with default low, 1=mode 3 with default high
#define bS1_2_WIRE        0x04      // enable SPI1 two wire mode: 0=3 wire (SCK+MOSI+MISO), 1=2 wire (SCK+MISO)
#define bS1_CLR_ALL       0x02      // force clear FIFO and count of SPI1
#define bS1_AUTO_IF       0x01      // enable FIFO accessing to auto clear bS1_IF_BYTE interrupt flag
sfr SPI1_CK_SE      = 0xB7;         // clock divisor setting

/*  UART1/iRS485 Registers  */
sfr SER1_FIFO       = 0x9A;         // UART1 FIFO data port: reading for receiving, writing for transmittal
//sfr SER1_RBR        = 0x9A;         // ReadOnly: UART1 receiver buffer
#define SER1_RBR          SER1_FIFO
//sfr SER1_THR        = 0x9A;         // WriteOnly: UART1 transmitter holding
#define SER1_THR          SER1_FIFO
sfr SER1_IER        = 0x91;         // UART1 interrupt enable
#define bIER_RESET        0x80      // UART1 software reset control, high action, auto clear
#define bIER_EN_MODEM_O   0x40      // enable UART1 modem output signal, DTR connect P0.0, RTS connect P0.1
#define bIER_PIN_MOD1     0x20      // UART1 pin mode high bit
#define bIER_PIN_MOD0     0x10      // UART1 pin mode low bit
#define MASK_U1_PIN_MOD   0x70      // bit mask of UART1 pin mode
// RS485EN & bIER_PIN_MOD1 & bIER_PIN_MOD0: UART1 pin mode
//  RS485EN = bUH1_DISABLE & ~ ( bXBUS_CS_OE & ~ bXBUS_AL_OE | bALE_CLK_EN )
//   x00: RXD1 connect P4.0, disable TXD1
//   010: RXD1/TXD1 connect P2.6/P2.7
//   001: RXD1/TXD1 connect P4.0/P4.4
//   011: RXD1/TXD1/TNOW connect P2.6/P2.7/P2.5
//   110: RXD1/TXD1 connect iRS485 pins XA/XB
//   101: RXD1/TXD1 connect iRS485 pins XA/XB, TNOW connect P4.4
//   111: RXD1/TXD1 connect iRS485 pins XA/XB, TNOW connect P2.5
#define bIER_MODEM_CHG    0x08      // UART1 interrupt enable for modem status change
#define bIER_LINE_STAT    0x04      // UART1 interrupt enable for receiver line status
#define bIER_THR_EMPTY    0x02      // UART1 interrupt enable for THR empty
#define bIER_RECV_RDY     0x01      // UART1 interrupt enable for receiver data ready
sfr SER1_IIR        = 0x92;         // ReadOnly: UART1 interrupt identification
#define MASK_U1_IIR_ID    0xC0      // ReadOnly: bit mask of UART1 IIR, FIFO enabled flag
#define bIIR_INT_FLAG3    0x08      // ReadOnly: UART1 interrupt flag bit 3
#define bIIR_INT_FLAG2    0x04      // ReadOnly: UART1 interrupt flag bit 2
#define bIIR_INT_FLAG1    0x02      // ReadOnly: UART1 interrupt flag bit 1
#define bIIR_INT_FLAG0    0x01      // ReadOnly: UART1 interrupt flag bit 0
#define MASK_U1_IIR_INT   0x0F      // ReadOnly: bit mask of UART1 interrupt flag
// bIIR_INT_FLAG3 & bIIR_INT_FLAG2 & bIIR_INT_FLAG1 & bIIR_INT_FLAG0: UART1 interrupt flag, list follow:
#define U1_INT_SLV_ADDR   0x0E      // UART1 interrupt by slave address match
#define U1_INT_LINE_STAT  0x06      // UART1 interrupt by receiver line status
#define U1_INT_RECV_RDY   0x04      // UART1 interrupt by receiver data available
#define U1_INT_RECV_TOUT  0x0C      // UART1 interrupt by receiver FIFO timeout
#define U1_INT_THR_EMPTY  0x02      // UART1 interrupt by THR empty
#define U1_INT_MODEM_CHG  0x00      // UART1 interrupt by modem status change
#define U1_INT_NO_INTER   0x01      // no UART interrupt is pending
#define bIIR_NO_INT       0x01      // UART1 no interrupt flag: 0=interrupt action, 1=no interrupt
//sfr SER1_FCR        = 0x92;         // WriteOnly: UART1 FIFO control
#define SER1_FCR          SER1_IIR
#define bFCR_FIFO_TRIG1   0x80      // UART1 receiver FIFO trigger level high bit
#define bFCR_FIFO_TRIG0   0x40      // UART1 receiver FIFO trigger level low bit
#define MASK_U1_FIFO_TRIG 0xC0      // bit mask of UART1 receiver FIFO trigger level
// bFCR_FIFO_TRIG1 & bFCR_FIFO_TRIG0: UART1 receiver FIFO trigger level
//   00: 1 byte
//   01: 2 bytes
//   10: 4 bytes
//   11: 7 bytes
#define bFCR_T_FIFO_CLR   0x04      // clear UART1 transmitter FIFO, high action, auto clear
#define bFCR_R_FIFO_CLR   0x02      // clear UART1 receiver FIFO, high action, auto clear
#define bFCR_FIFO_EN      0x01      // UART1 FIFO enable
sfr SER1_LCR        = 0x93;         // UART1 line control
#define bLCR_DLAB         0x80      // UART1 divisor latch access bit
#define bLCR_BREAK_EN     0x40      // UART1 break control enable
#define bLCR_PAR_MOD1     0x20      // UART1 parity mode high bit
#define bLCR_PAR_MOD0     0x10      // UART1 parity mode low bit
#define MASK_U1_PAR_MOD   0x30      // bit mask of UART1 parity mode
#define U1_PAR_MOD_ODD    0x00
#define U1_PAR_MOD_EVEN   0x10
#define U1_PAR_MOD_MARK   0x20
#define U1_PAR_MOD_SPACE  0x30
// bLCR_PAR_MOD1 & bLCR_PAR_MOD0: UART1 parity mode if bLCR_PAR_EN=1, else ignored
//   00: the 9th bit is odd parity bit
//   01: the 9th bit is even parity bit
//   10: the 9th bit is mark bit
//   11: the 9th bit is space bit
#define bLCR_PAR_EN       0x08      // UART1 parity enable
#define bLCR_STOP_BIT     0x04      // UART1 stop bit length: 0=1 bit, 1=2 bits
#define bLCR_WORD_SZ1     0x02      // UART1 word bit length high
#define bLCR_WORD_SZ0     0x01      // UART1 word bit length low
#define MASK_U1_WORD_SZ   0x03      // bit mask of UART1 word bit length
// bLCR_WORD_SZ1 & bLCR_WORD_SZ0: UART1 word bit length (exclude parity bit)
//   00: 5 bits
//   01: 6 bits
//   10: 7 bits
//   11: 8 bits
sfr SER1_MCR        = 0x94;         // UART1 modem control
#define bMCR_HALF         0x80      // UART1 enable half-duplex mode
#define bMCR_TNOW         0x40      // UART1 enable TNOW output on DTR pin
#define bMCR_AUTO_FLOW    0x20      // UART1 enable autoflow control by CTS & RTS pin
#define bMCR_LOOP         0x10      // UART1 enable local loop back for testing
#define bMCR_OUT2         0x08      // UART1 control OUT2, enable interrupt request output
#define bMCR_OUT1         0x04      // UART1 control OUT1, not real pin
#define bMCR_RTS          0x02      // UART1 control RTS
#define bMCR_DTR          0x01      // UART1 control DTR
sfr SER1_LSR        = 0x95;         // ReadOnly: UART1 line status
#define bLSR_ERR_R_FIFO   0x80      // ReadOnly: error in UART1 receiver fifo, read to clear
#define bLSR_T_ALL_EMP    0x40      // ReadOnly: UART1 transmitter all empty status
#define bLSR_T_FIFO_EMP   0x20      // ReadOnly: UART1 transmitter FIFO empty status
#define bLSR_BREAK_ERR    0x10      // ReadOnly: UART1 receiver break error, read to clear
#define bLSR_FRAME_ERR    0x08      // ReadOnly: UART1 receiver frame error, read to clear
#define bLSR_PAR_ERR      0x04      // ReadOnly: UART1 receiver parity error, read to clear
#define bLSR_OVER_ERR     0x02      // ReadOnly: UART1 receiver overrun error, read to clear
#define bLSR_DATA_RDY     0x01      // ReadOnly: UART1 receiver FIFO data ready status
sfr SER1_MSR        = 0x96;         // ReadOnly: UART1 modem status
#define bMSR_DCD          0x80      // ReadOnly: UART1 DCD action status
#define bMSR_RI           0x40      // ReadOnly: UART1 RI action status
#define bMSR_DSR          0x20      // ReadOnly: UART1 DSR action status
#define bMSR_CTS          0x10      // ReadOnly: UART1 CTS action status
#define bMSR_DCD_CHG      0x08      // ReadOnly: UART1 DCD changed status, high action, read to clear
#define bMSR_RI_CHG       0x04      // ReadOnly: UART1 RI changed status, high action, read to clear
#define bMSR_DSR_CHG      0x02      // ReadOnly: UART1 DSR changed status, high action, read to clear
#define bMSR_CTS_CHG      0x01      // ReadOnly: UART1 CTS changed status, high action, read to clear
sfr SER1_ADDR       = 0x97;         // UART1 slave address for multi-device communication, value 0xFF is disable
//sfr SER1_DLL        = 0x9A;         // UART1 divisor latch LSB byte if bLCR_DLAB=1
#define SER1_DLL          SER1_FIFO
//sfr SER1_DLM        = 0x91;         // UART1 divisor latch MSB byte if bLCR_DLAB=1
#define SER1_DLM          SER1_IER
//sfr SER1_DIV        = 0x97;         // UART1 pre-divisor latch byte if bLCR_DLAB=1
#define SER1_DIV          SER1_ADDR

/*  ADC Registers  */
sfr16 ADC_DMA       = 0xEC;         // DMA address, must even address, little-endian, automatic increasing after DMA
sfr ADC_DMA_AL      = 0xEC;         // DMA address low byte, automatic increasing after DMA
sfr ADC_DMA_AH      = 0xED;         // DMA address high byte, automatic increasing after DMA
sfr ADC_DMA_CN      = 0xEE;         // DMA remainder word count, automatic decreasing after DMA
sfr ADC_CK_SE       = 0xEF;         // ADC clock divisor setting
#define MASK_ADC_CK_SE    0x7F      // bit mask of ADC clock divisor
#define bADC_CHK_CLK_SEL  0x80      // AIN7 level check delay clock frequency selection: 0=slow(1x), 1=fast(4x)
sfr ADC_STAT        = 0xF1;         // ADC status
#define bADC_IF_DMA_END   0x80      // interrupt flag for DMA completion, write 1 to clear or write ADC_DMA_CN to clear
#define bADC_IF_FIFO_OV   0x40      // interrupt flag for FIFO overflow, write 1 to clear
#define bADC_IF_AIN7_LOW  0x20      // interrupt flag for AIN7 low level, write 1 to clear
#define bADC_IF_ACT       0x10      // interrupt flag for a ADC finished, write 1 to clear
#define bADC_AIN7_INT     0x08      // ReadOnly: current AIN7 low level delay status
#define bADC_CHANN_ID     0x04      // ReadOnly: current channel ID for channel automatic switch mode: 0=AIN0 or AIN6, 1=AIN1 or AIN4 or AIN7
#define bADC_DATA_OK      0x04      // ReadOnly: ADC end and data ready flag for channel manual selection mode: 0=data not ready, 1=data ready
#define bADC_FIFO_CNT1    0x02      // ReadOnly: ADC FIFO count bit 1
#define bADC_FIFO_CNT0    0x01      // ReadOnly: ADC FIFO count bit 0
#define MASK_ADC_FIFO_CNT 0x03      // ReadOnly: bit mask of ADC FIFO count
// bADC_FIFO_CNT1 & bADC_FIFO_CNT0: ADC FIFO count
//   00: empty FIFO, return current ADC result if reading FIFO
//   01: 1 result in FIFO
//   01: 2 results in FIFO, FIFO full
//   11: unknown error
sfr ADC_CTRL        = 0xF2;         // ADC control
#define bADC_SAMPLE       0x80      // automatic or manual sample pulse control, high action
#define bADC_SAMP_WIDTH   0x40      // automatic sample pulse width: 0=1 ADC clock, 1=2 ADC clocks
#define bADC_CHANN_MOD1   0x20      // ADC channel control mode high bit
#define bADC_CHANN_MOD0   0x10      // ADC channel control mode low bit
#define MASK_ADC_CHANN    0x30      // bit mask of ADC channel control mode
// bADC_CHANN_MOD1 & bADC_CHANN_MOD0: ADC channel control mode
//   00: manual selection by ADC_CHANN bit
//   01: automatic switch between AIN0 and AIN1
//   10: automatic switch between AIN6 and AIN4
//   11: automatic switch between AIN6 and AIN7
#define MASK_ADC_CYCLE    0x0F      // bit mask of ADC cycle (ADC clock number): 0=manual sample, others=set cycle number for automatic sample
sfr ADC_CHANN       = 0xF3;         // ADC channel seletion
sfr16 ADC_FIFO      = 0xF4;         // ReadOnly: FIFO word, little-endian
sfr ADC_FIFO_L      = 0xF4;         // ReadOnly: FIFO low byte
sfr ADC_FIFO_H      = 0xF5;         // ReadOnly: FIFO high byte
sfr ADC_SETUP       = 0xF6;         // ADC setup
#define bADC_DMA_EN       0x80      // DMA enable and DMA interrupt enable for ADC
#define bADC_IE_FIFO_OV   0x40      // enable interrupt for FIFO overflow
#define bADC_IE_AIN7_LOW  0x20      // enable interrupt for AIN7 low level
#define bADC_IE_ACT       0x10      // enable interrupt for a ADC finished
#define bADC_CLOCK        0x08      // ReadOnly: current level of ADC clock
#define bADC_POWER_EN     0x04      // control ADC power: 0=shut down ADC, 1=enable power for ADC
#define bADC_EXT_SW_EN    0x02      // control extend switch module power: 0=shut down, 1=enable power for extend switch
#define bADC_AIN7_CHK_EN  0x01      // control AIN7 level check module power: 0=shut down, 1=enable power for AIN7 level check
sfr ADC_EX_SW       = 0xF7;         // ADC extend switch control
#define bADC_SW_AIN7_H    0x80      // internal AIN7 extend switch control: 0=float AIN7, 1=tie AIN7 to high level VDD33
#define bADC_SW_AIN6_L    0x40      // internal AIN6 extend switch control: 0=float AIN6, 1=tie AIN6 to low level GND
#define bADC_SW_AIN5_H    0x20      // internal AIN5 extend switch control: 0=float AIN5, 1=tie AIN5 to high level VDD33
#define bADC_SW_AIN4_L    0x10      // internal AIN4 extend switch control: 0=float AIN4, 1=tie AIN4 to low level GND
#define bADC_EXT_SW_SEL   0x08      // extend switch resistance selection: 0=high resistance, 1=low resistance
#define bADC_RESOLUTION   0x04      // ADC resolution: 0=10 bits, 1=11 bits
#define bADC_AIN7_DLY1    0x02      // AIN7 level check delay control bit 1
#define bADC_AIN7_DLY0    0x01      // AIN7 level check delay control bit 0
#define MASK_ADC_AIN7_DLY 0x03      // bit mask for AIN7 check delay control: 01=longest, 10=longer, 11=shorter, 00=shortest (no delay)

/*  USB/Host/Device Registers  */
sfr USB_RX_LEN      = 0xD1;         // ReadOnly: USB receiving length
sfr UEP1_CTRL       = 0xD2;         // endpoint 1 control
#define bUEP_R_TOG        0x80      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define bUEP_T_TOG        0x40      // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define bUEP_AUTO_TOG     0x10      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define bUEP_R_RES1       0x08      // handshake response type high bit for USB endpoint X receiving (OUT)
#define bUEP_R_RES0       0x04      // handshake response type low bit for USB endpoint X receiving (OUT)
#define MASK_UEP_R_RES    0x0C      // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define UEP_R_RES_ACK     0x00
#define UEP_R_RES_TOUT    0x04
#define UEP_R_RES_NAK     0x08
#define UEP_R_RES_STALL   0x0C
// bUEP_R_RES1 & bUEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
#define bUEP_T_RES1       0x02      // handshake response type high bit for USB endpoint X transmittal (IN)
#define bUEP_T_RES0       0x01      // handshake response type low bit for USB endpoint X transmittal (IN)
#define MASK_UEP_T_RES    0x03      // USB端点X传输的握手响应类型的位掩码(IN)
#define UEP_T_RES_ACK     0x00
#define UEP_T_RES_TOUT    0x01
#define UEP_T_RES_NAK     0x02
#define UEP_T_RES_STALL   0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
sfr UEP1_T_LEN      = 0xD3;         // endpoint 1 transmittal length
sfr UEP2_CTRL       = 0xD4;         // endpoint 2 control
sfr UEP2_T_LEN      = 0xD5;         // endpoint 2 transmittal length
sfr UEP3_CTRL       = 0xD6;         // endpoint 3 control
sfr UEP3_T_LEN      = 0xD7;         // endpoint 3 transmittal length
sfr USB_INT_FG      = 0xD8;         // USB interrupt flag
 sbit U_IS_NAK      = USB_INT_FG^7; // 只读: 该位为 1 表示当前 USB 传输过程中接收到 NAK 忙响应
 sbit U_TOG_OK      = USB_INT_FG^6; // 只读: 当前 USB 传输 DATA0/1 同步标志匹配状态，该位为 1 表示同步，数据有效；
 sbit U_SIE_FREE    = USB_INT_FG^5; // 只读: USB协议处理器的空闲状态位,该位为0表示忙，正在进行USB传输,该位为1表示USB空闲
 sbit UIF_FIFO_OV   = USB_INT_FG^4; // USB的FIFO溢出中断标志，									位地址写0清除或USB_INT_FG写1清除
 sbit UIF_HST_SOF   = USB_INT_FG^3; // USB主机SOF定时器中断标志，									位地址写0清除或USB_INT_FG写1清除
 sbit UIF_SUSPEND   = USB_INT_FG^2; // USB挂起或唤醒事件中断标志，								位地址写0清除或USB_INT_FG写1清除
 sbit UIF_TRANSFER  = USB_INT_FG^1; // USB传输完成中断标志，										位地址写0清除或USB_INT_FG写1清除
 sbit UIF_DETECT    = USB_INT_FG^0; // USB 传输完成中断标志位，该中断由一个 USB 传输完成触发，		位地址写0清除或USB_INT_FG写1清除
 sbit UIF_BUS_RST   = USB_INT_FG^0; // USB 总线复位事件中断标志位，该中断由 USB 总线复位事件触发，	位地址写0清除或USB_INT_FG写1清除
sfr USB_INT_ST      = 0xD9;         // ReadOnly: USB interrupt status
#define bUIS_IS_NAK       0x80      // ReadOnly: indicate current USB transfer is NAK received for USB device mode
#define bUIS_TOG_OK       0x40      // ReadOnly: indicate current USB transfer toggle is OK
#define bUIS_TOKEN1       0x20      // ReadOnly: current token PID code bit 1 received for USB device mode
#define bUIS_TOKEN0       0x10      // ReadOnly: current token PID code bit 0 received for USB device mode
#define MASK_UIS_TOKEN    0x30      // ReadOnly: bit mask of current token PID code received for USB device mode
#define UIS_TOKEN_OUT     0x00
#define UIS_TOKEN_SOF     0x10
#define UIS_TOKEN_IN      0x20
#define UIS_TOKEN_SETUP   0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode
//   00: OUT token PID received
//   01: SOF token PID received
//   10: IN token PID received
//   11: SETUP token PID received
#define MASK_UIS_ENDP     0x0F      // ReadOnly: bit mask of current transfer endpoint number for USB device mode
#define MASK_UIS_H_RES    0x0F      // ReadOnly: bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received
sfr USB_MIS_ST      = 0xDA;         // ReadOnly: USB miscellaneous status
#define bUMS_SOF_PRES     0x80      // ReadOnly: indicate host SOF timer presage status
#define bUMS_SOF_ACT      0x40      // ReadOnly: indicate host SOF timer action status for USB host
#define bUMS_SIE_FREE     0x20      // ReadOnly: indicate USB SIE free status
#define bUMS_R_FIFO_RDY   0x10      // ReadOnly: indicate USB receiving FIFO ready status (not empty)
#define bUMS_BUS_RESET    0x08      // ReadOnly: indicate USB bus reset status
#define bUMS_SUSPEND      0x04      // ReadOnly: indicate USB suspend status
#define bUMS_H1_ATTACH    0x02      // ReadOnly: indicate device attached status on USB hub1 HP/HM
#define bUMS_H0_ATTACH    0x01      // ReadOnly: indicate device attached status on USB hub0 DP/DM
sfr USB_HUB_ST      = 0xDB;         // ReadOnly: USB host hub status
#define bUHS_H1_ATTACH    0x80      // ReadOnly: indicate device attached status on USB hub1 HP/HM
#define bUHS_HM_LEVEL     0x40      // ReadOnly: indicate HM level saved at device attached to USB hub1
#define bUHS_HP_PIN       0x20      // ReadOnly: indicate current HP pin level
#define bUHS_HM_PIN       0x10      // ReadOnly: indicate current HM pin level
#define bUHS_H0_ATTACH    0x08      // ReadOnly: indicate device attached status on USB hub0 DP/DM
#define bUHS_DM_LEVEL     0x04      // ReadOnly: indicate DM level saved at device attached to USB hub0
#define bUHS_DP_PIN       0x02      // ReadOnly: indicate current DP pin level
#define bUHS_DM_PIN       0x01      // ReadOnly: indicate current DM pin level
sfr UEP0_CTRL       = 0xDC;         // endpoint 0 control
sfr UEP0_T_LEN      = 0xDD;         // endpoint 0 transmittal length
sfr UEP4_CTRL       = 0xDE;         // endpoint 4 control
sfr UEP4_T_LEN      = 0xDF;         // endpoint 4 transmittal length
sfr USB_INT_EN      = 0xE1;         // USB interrupt enable
#define bUIE_DEV_SOF      0x80      // enable interrupt for SOF received for USB device mode
#define bUIE_DEV_NAK      0x40      // enable interrupt for NAK responded for USB device mode
#define bUIE_FIFO_OV      0x10      // enable interrupt for FIFO overflow
#define bUIE_HST_SOF      0x08      // enable interrupt for host SOF timer action for USB host mode
#define bUIE_SUSPEND      0x04      // enable interrupt for USB suspend or resume event
#define bUIE_TRANSFER     0x02      // enable interrupt for USB transfer completion
#define bUIE_DETECT       0x01      // enable interrupt for USB device detected event for USB host mode
#define bUIE_BUS_RST      0x01      // enable interrupt for USB bus reset event for USB device mode
sfr USB_CTRL        = 0xE2;         // USB base control
#define bUC_HOST_MODE     0x80      // enable USB host mode: 0=device mode, 1=host mode
#define bUC_LOW_SPEED     0x40      // enable USB low speed: 0=full speed, 1=low speed
#define bUC_DEV_PU_EN     0x20      // USB device enable and internal pullup resistance enable
#define bUC_SYS_CTRL1     0x20      // USB system control high bit
#define bUC_SYS_CTRL0     0x10      // USB system control low bit
#define MASK_UC_SYS_CTRL  0x30      // bit mask of USB system control
// bUC_HOST_MODE & bUC_SYS_CTRL1 & bUC_SYS_CTRL0: USB system control
//   0 00: disable USB device and disable internal pullup resistance
//   0 01: enable USB device and disable internal pullup resistance, need external pullup resistance
//   0 10: enable USB device and enable internal pullup resistance
//   0 11: enable USB device and enable internal weak pullup resistance
//   1 00: enable USB host and normal status
//   1 01: enable USB host and force DP/DM output SE0 state
//   1 10: enable USB host and force DP/DM output J state
//   1 11: enable USB host and force DP/DM output resume or K state
#define bUC_INT_BUSY      0x08      // enable automatic responding busy for device mode or automatic pause for host mode during interrupt flag UIF_TRANSFER valid
#define bUC_RESET_SIE     0x04      // force reset USB SIE, need software clear
#define bUC_CLR_ALL       0x02      // force clear FIFO and count of USB
#define bUC_DMA_EN        0x01      // DMA enable and DMA interrupt enable for USB
sfr USB_DEV_AD      = 0xE3;         // USB device address, lower 7 bits for USB device address
#define bUDA_GP_BIT       0x80      // general purpose bit
#define MASK_USB_ADDR     0x7F      // bit mask for USB device address
sfr UDEV_CTRL       = 0xE4;         // USB device physical port control
#define bUD_RECV_DIS      0x40      // disable USB physical port receiver: 0=enable receiver, 1=disable receiver
#define bUD_DP_PD_DIS     0x20      // disable USB DP pulldown resistance: 0=enable pulldown, 1=disable
#define bUD_DM_PD_DIS     0x10      // disable USB DM pulldown resistance: 0=enable pulldown, 1=disable
#define bUD_DIFF_IN       0x08      // ReadOnly: indicate current DP/DM difference input status
#define bUD_LOW_SPEED     0x04      // enable USB physical port low speed: 0=full speed, 1=low speed
#define bUD_GP_BIT        0x02      // general purpose bit
#define bUD_PORT_EN       0x01      // enable USB physical port I/O: 0=disable, 1=enable
//sfr UHUB0_CTRL      = 0xE4;         // USB hub0 control
#define UHUB0_CTRL        UDEV_CTRL
#define bUH_RECV_DIS      0x40      // disable USB hub receiver: 0=enable hub receiver, 1=disable hub receiver
#define bUH_DP_PD_DIS     0x20      // disable USB DP or HP pulldown resistance: 0=enable pulldown, 1=disable
#define bUH_DM_PD_DIS     0x10      // disable USB DM or HM pulldown resistance: 0=enable pulldown, 1=disable
#define bUH_DIFF_IN       0x08      // ReadOnly: indicate current DP/DM or HP/HM difference input status
#define bUH_LOW_SPEED     0x04      // enable USB hub low speed: 0=full speed, 1=low speed
#define bUH_BUS_RESET     0x02      // control USB hub bus reset: 0=normal, 1=force bus reset
#define bUH_PORT_EN       0x01      // enable USB hub port: 0=disable, 1=enable port, automatic disabled if USB device detached
sfr UHUB1_CTRL      = 0xE5;         // USB hub1 control
#define bUH1_DISABLE      0x80      // disable USB hub1 pin: 0=enable hub1 and using HP/HM pin, 1=disable hub1 and releasing HP/HM pin
sfr16 USB_DMA       = 0xE6;         // ReadOnly: current DMA address, little-endian
sfr USB_DMA_AL      = 0xE6;         // ReadOnly: current DMA address low byte
sfr USB_DMA_AH      = 0xE7;         // ReadOnly: current DMA address high byte
//sfr UH_SETUP        = 0xD2;         // host aux setup
#define UH_SETUP          UEP1_CTRL
#define bUH_PRE_PID_EN    0x80      // USB host PRE PID enable for low speed device via hub
#define bUH_SOF_EN        0x40      // USB host automatic SOF enable
//sfr UH_RX_CTRL      = 0xD4;         // host receiver endpoint control
#define UH_RX_CTRL        UEP2_CTRL
#define bUH_R_TOG         0x80      // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define bUH_R_AUTO_TOG    0x10      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define bUH_R_RES         0x04      // prepared handshake response type for host receiving (IN): 0=ACK (ready), 1=no response, time out to device, for isochronous transactions
//sfr UH_EP_PID       = 0xD5;         // host endpoint and token PID, lower 4 bits for endpoint number, upper 4 bits for token PID
#define UH_EP_PID         UEP2_T_LEN
#define MASK_UH_TOKEN     0xF0      // bit mask of token PID for USB host transfer
#define MASK_UH_ENDP      0x0F      // bit mask of endpoint number for USB host transfer
//sfr UH_TX_CTRL      = 0xD6;         // host transmittal endpoint control
#define UH_TX_CTRL        UEP3_CTRL
#define bUH_T_TOG         0x40      // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define bUH_T_AUTO_TOG    0x10      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define bUH_T_RES         0x01      // expected handshake response type for host transmittal (SETUP/OUT): 0=ACK (ready), 1=no response, time out from device, for isochronous transactions
//sfr UH_TX_LEN       = 0xD7;         // host transmittal endpoint transmittal length
#define UH_TX_LEN         UEP3_T_LEN

/*----- XDATA: xRAM, xBUS, xSFR ------------------------------------------*/

#define XDATA_RAM_SIZE    0x1800    // size of expanded xRAM, xdata SRAM embedded chip
#define XDATA_XBUS_ADDR   0x4000    // xdata xBUS start address
#define XDATA_XBUS_CS0    0x4000    // xdata xBUS chip-selection 0#
#define XDATA_XCS0_SIZE   0x4000    // size of xdata xBUS chip-selection 0#: @0x4000~0x7FFF
#define XDATA_XBUS_CS1    0x8000    // xdata xBUS chip-selection 1#
#define XDATA_XCS1_SIZE   0x8000    // size of xdata xBUS chip-selection 1#: @0x8000~0xFFFF

/*  USB auxiliary Registers on xDATA, xSFR  */
#define REG_USB_AUX_BASE  0x2440    // USB auxiliary registers base address
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP4_1_MOD  _at_ 0x2446;   // endpoint 4/1 mode
UINT8PV pUEP4_1_MOD  _at_ 0x2546;
#else
extern UINT8XV  UEP4_1_MOD;
extern UINT8PV pUEP4_1_MOD;
#endif
#define bUEP1_RX_EN       0x80      // enable USB endpoint 1 receiving (OUT)
#define bUEP1_TX_EN       0x40      // enable USB endpoint 1 transmittal (IN)
#define bUEP1_BUF_MOD     0x10      // buffer mode of USB endpoint 1
// bUEPn_RX_EN & bUEPn_TX_EN & bUEPn_BUF_MOD: USB endpoint 1/2/3 buffer mode, buffer start address is UEPn_DMA
//   0 0 x:  disable endpoint and disable buffer
//   1 0 0:  64 bytes buffer for receiving (OUT endpoint)
//   1 0 1:  dual 64 bytes buffer by toggle bit bUEP_R_TOG selection for receiving (OUT endpoint), total=128bytes
//   0 1 0:  64 bytes buffer for transmittal (IN endpoint)
//   0 1 1:  dual 64 bytes buffer by toggle bit bUEP_T_TOG selection for transmittal (IN endpoint), total=128bytes
//   1 1 0:  64 bytes buffer for receiving (OUT endpoint) + 64 bytes buffer for transmittal (IN endpoint), total=128bytes
//   1 1 1:  dual 64 bytes buffer by bUEP_R_TOG selection for receiving (OUT endpoint) + dual 64 bytes buffer by bUEP_T_TOG selection for transmittal (IN endpoint), total=256bytes
#define bUEP4_RX_EN       0x08      // enable USB endpoint 4 receiving (OUT)
#define bUEP4_TX_EN       0x04      // enable USB endpoint 4 transmittal (IN)
// bUEP4_RX_EN & bUEP4_TX_EN: USB endpoint 4 buffer mode, buffer start address is UEP0_DMA
//   0 0:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint)
//   1 0:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint) + 64 bytes buffer for endpoint 4 receiving (OUT endpoint), total=128bytes
//   0 1:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint) + 64 bytes buffer for endpoint 4 transmittal (IN endpoint), total=128bytes
//   1 1:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint)
//           + 64 bytes buffer for endpoint 4 receiving (OUT endpoint) + 64 bytes buffer for endpoint 4 transmittal (IN endpoint), total=192bytes
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP2_3_MOD  _at_ 0x2447;   // endpoint 2/3 mode
UINT8PV pUEP2_3_MOD  _at_ 0x2547;
#else
extern UINT8XV  UEP2_3_MOD;
extern UINT8PV pUEP2_3_MOD;
#endif
#define bUEP3_RX_EN       0x80      // enable USB endpoint 3 receiving (OUT)
#define bUEP3_TX_EN       0x40      // enable USB endpoint 3 transmittal (IN)
#define bUEP3_BUF_MOD     0x10      // buffer mode of USB endpoint 3
#define bUEP2_RX_EN       0x08      // enable USB endpoint 2 receiving (OUT)
#define bUEP2_TX_EN       0x04      // enable USB endpoint 2 transmittal (IN)
#define bUEP2_BUF_MOD     0x01      // buffer mode of USB endpoint 2
//unsigned short volatile xdata UEP0_DMA    _at_ 0x2448;   // endpoint 0&4 buffer start address, must even address, big-endian
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP0_DMA_H  _at_ 0x2448;   // endpoint 0&4 buffer start address high byte
UINT8XV  UEP0_DMA_L  _at_ 0x2449;   // endpoint 0&4 buffer start address low byte
UINT8PV pUEP0_DMA_H  _at_ 0x2548;
UINT8PV pUEP0_DMA_L  _at_ 0x2549;
#else
extern UINT8XV  UEP0_DMA_H;
extern UINT8XV  UEP0_DMA_L;
extern UINT8PV pUEP0_DMA_H;
extern UINT8PV pUEP0_DMA_L;
#endif
#define UEP0_DMA          (unsigned short volatile) UEP0_DMA_H
#define pUEP0_DMA         (unsigned short volatile) pUEP0_DMA_H
//unsigned short volatile xdata UEP1_DMA    _at_ 0x244A;   // endpoint 1 buffer start address, must even address, big-endian
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP1_DMA_H  _at_ 0x244A;   // endpoint 1 buffer start address high byte
UINT8XV  UEP1_DMA_L  _at_ 0x244B;   // endpoint 1 buffer start address low byte
UINT8PV pUEP1_DMA_H  _at_ 0x254A;
UINT8PV pUEP1_DMA_L  _at_ 0x254B;
#else
extern UINT8XV  UEP1_DMA_H;
extern UINT8XV  UEP1_DMA_L;
extern UINT8PV pUEP1_DMA_H;
extern UINT8PV pUEP1_DMA_L;
#endif
#define UEP1_DMA          (unsigned short volatile) UEP1_DMA_H
#define pUEP1_DMA         (unsigned short volatile) pUEP1_DMA_H
//unsigned short volatile xdata UEP2_DMA    _at_ 0x244C;   // endpoint 2 buffer start address, must even address, big-endian
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP2_DMA_H  _at_ 0x244C;   // endpoint 2 buffer start address high byte
UINT8XV  UEP2_DMA_L  _at_ 0x244D;   // endpoint 2 buffer start address low byte
UINT8PV pUEP2_DMA_H  _at_ 0x254C;
UINT8PV pUEP2_DMA_L  _at_ 0x254D;
#else
extern UINT8XV  UEP2_DMA_H;
extern UINT8XV  UEP2_DMA_L;
extern UINT8PV pUEP2_DMA_H;
extern UINT8PV pUEP2_DMA_L;
#endif
#define UEP2_DMA          (unsigned short volatile) UEP2_DMA_H
#define pUEP2_DMA         (unsigned short volatile) pUEP2_DMA_H
//unsigned short volatile xdata UEP3_DMA    _at_ 0x244E;   // endpoint 3 buffer start address, must even address, big-endian
#ifndef NO_XSFR_DEFINE
UINT8XV  UEP3_DMA_H  _at_ 0x244E;   // endpoint 3 buffer start address high byte
UINT8XV  UEP3_DMA_L  _at_ 0x244F;   // endpoint 3 buffer start address low byte
UINT8PV pUEP3_DMA_H  _at_ 0x254E;
UINT8PV pUEP3_DMA_L  _at_ 0x254F;
#else
extern UINT8XV  UEP3_DMA_H;
extern UINT8XV  UEP3_DMA_L;
extern UINT8PV pUEP3_DMA_H;
extern UINT8PV pUEP3_DMA_L;
#endif
#define UEP3_DMA          (unsigned short volatile) UEP3_DMA_H
#define pUEP3_DMA         (unsigned short volatile) pUEP3_DMA_H
//UINT8XV  UH_EP_MOD   _at_ 0x2447;   // host endpoint mode
#define UH_EP_MOD         UEP2_3_MOD
#define pUH_EP_MOD       pUEP2_3_MOD
#define bUH_EP_TX_EN      0x40      // enable USB host OUT endpoint transmittal
#define bUH_EP_TBUF_MOD   0x10      // buffer mode of USB host OUT endpoint
// bUH_EP_TX_EN & bUH_EP_TBUF_MOD: USB host OUT endpoint buffer mode, buffer start address is UH_TX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for transmittal (OUT endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_T_TOG selection for transmittal (OUT endpoint), total=128bytes
#define bUH_EP_RX_EN      0x08      // enable USB host IN endpoint receiving
#define bUH_EP_RBUF_MOD   0x01      // buffer mode of USB host IN endpoint
// bUH_EP_RX_EN & bUH_EP_RBUF_MOD: USB host IN endpoint buffer mode, buffer start address is UH_RX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (IN endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_R_TOG selection for receiving (IN endpoint), total=128bytes
//unsigned short volatile xdata UH_RX_DMA   _at_ 0x244C;   // host rx endpoint buffer start address, big-endian
#define UH_RX_DMA         UEP2_DMA
#define pUH_RX_DMA       pUEP2_DMA
//UINT8XV  UH_RX_DMA_H _at_ 0x244C;   // host rx endpoint buffer start address high byte
#define UH_RX_DMA_H       UEP2_DMA_H
#define pUH_RX_DMA_H     pUEP2_DMA_H
//UINT8XV  UH_RX_DMA_L _at_ 0x244D;   // host rx endpoint buffer start address low byte
#define UH_RX_DMA_L       UEP2_DMA_L
#define pUH_RX_DMA_L     pUEP2_DMA_L
//unsigned short volatile xdata UH_TX_DMA   _at_ 0x244E;   // host rx endpoint buffer start address, big-endian
#define UH_TX_DMA         UEP3_DMA
#define pUH_TX_DMA       pUEP3_DMA
//UINT8XV  UH_TX_DMA_H _at_ 0x244E;   // host rx endpoint buffer start address high byte
#define UH_TX_DMA_H       UEP3_DMA_H
#define pUH_TX_DMA_H     pUEP3_DMA_H
//UINT8XV  UH_TX_DMA_L _at_ 0x244F;   // host rx endpoint buffer start address low byte
#define UH_TX_DMA_L       UEP3_DMA_L
#define pUH_TX_DMA_L     pUEP3_DMA_L

/*  LED Registers on xDATA, xSFR  */
#define REG_LED_BASE      0x2880    // LED registers base address
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_STAT    _at_ 0x2880;   // LED status
UINT8PV pLED_STAT    _at_ 0x2980;
#else
extern UINT8XV  LED_STAT;
extern UINT8PV pLED_STAT;
#endif
#define bLED_IF_DMA_END   0x80      // interrupt flag for DMA completion, write 1 to clear or write LED_DMA_CN to clear
#define bLED_FIFO_EMPTY   0x40      // ReadOnly: indicate FIFO empty status
#define bLED_IF_FIFO_REQ  0x20      // interrupt flag for request FIFO data ( FIFO <=2), write 1 to clear
#define bLED_CLOCK        0x10      // ReadOnly: current LED clock level
#define MASK_LED_FIFO_CNT 0x07      // ReadOnly: bit mask of LED FIFO count
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_CTRL    _at_ 0x2881;   // LED control
UINT8PV pLED_CTRL    _at_ 0x2981;
#else
extern UINT8XV  LED_CTRL;
extern UINT8PV pLED_CTRL;
#endif
#define bLED_CHAN_MOD1    0x80      // LED channel mode high bit
#define bLED_CHAN_MOD0    0x40      // LED channel mode low bit
#define MASK_LED_CHAN_MOD 0xC0      // bit mask of LED channel mode
// bLED_CHAN_MOD1 & bLED_CHAN_MOD0: LED channel mode
//   00: single channel output, LED0
//   01: dual channels output, LED0/1
//   10: 4 channels output, LED0~3
//   11: 4 channels output and LED2/3 from aux buffer, LED0~3
#define bLED_IE_FIFO_REQ  0x20      // enable interrupt for FIFO <=2
#define bLED_DMA_EN       0x10      // DMA enable and DMA interrupt enable for LED
#define bLED_OUT_EN       0x08      // LED output enable
#define bLED_OUT_POLAR    0x04      // LED output polarity: 0=pass, 1=invert
#define bLED_CLR_ALL      0x02      // force clear FIFO and count of LED
#define bLED_BIT_ORDER    0x01      // LED bit data order: 0=LSB first, 1=MSB first
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_DATA    _at_ 0x2882;   // WriteOnly: data port
UINT8PV pLED_DATA    _at_ 0x2982;
#else
extern UINT8XV  LED_DATA;
extern UINT8PV pLED_DATA;
#endif
//UINT8XV  LED_FIFO_CN _at_ 0x2882;   // ReadOnly: FIFO count status
#define LED_FIFO_CN       LED_DATA
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_CK_SE   _at_ 0x2883;   // clock divisor setting
UINT8PV pLED_CK_SE   _at_ 0x2983;
#else
extern UINT8XV  LED_CK_SE;
extern UINT8PV pLED_CK_SE;
#endif
//unsigned short volatile xdata LED_DMA     _at_ 0x2884;   // DMA address, must even address, big-endian, automatic increasing after DMA
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_DMA_AH  _at_ 0x2884;   // DMA address high byte, automatic increasing after DMA
UINT8XV  LED_DMA_AL  _at_ 0x2885;   // DMA address low byte, automatic increasing after DMA
UINT8PV pLED_DMA_AH  _at_ 0x2984;
UINT8PV pLED_DMA_AL  _at_ 0x2985;
#else
extern UINT8XV  LED_DMA_AH;
extern UINT8XV  LED_DMA_AL;
extern UINT8PV pLED_DMA_AH;
extern UINT8PV pLED_DMA_AL;
#endif
#define LED_DMA           (unsigned short volatile) LED_DMA_AH
#define pLED_DMA          (unsigned short volatile) pLED_DMA_AH
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_DMA_CN  _at_ 0x2886;   // DMA remainder word count, just main buffer and exclude aux buffer, automatic decreasing after DMA
UINT8PV pLED_DMA_CN  _at_ 0x2986;
#else
extern UINT8XV  LED_DMA_CN;
extern UINT8PV pLED_DMA_CN;
#endif
//unsigned short volatile xdata LED_DMA_X   _at_ 0x2888;   // aux buffer DMA address, must even address, big-endian, automatic increasing after DMA
#ifndef NO_XSFR_DEFINE
UINT8XV  LED_DMA_XH  _at_ 0x2888;   // aux buffer DMA address high byte, automatic increasing after DMA
UINT8XV  LED_DMA_XL  _at_ 0x2889;   // aux buffer DMA address low byte, automatic increasing after DMA
UINT8PV pLED_DMA_XH  _at_ 0x2988;
UINT8PV pLED_DMA_XL  _at_ 0x2989;
#else
extern UINT8XV  LED_DMA_XH;
extern UINT8XV  LED_DMA_XL;
extern UINT8PV pLED_DMA_XH;
extern UINT8PV pLED_DMA_XL;
#endif
#define LED_DMA_X         (unsigned short volatile) LED_DMA_XH
#define pLED_DMA_X        (unsigned short volatile) pLED_DMA_XH

/*----- Reference Information --------------------------------------------*/
#define ID_CH559          0x59      // chip ID

/* Interrupt routine address and interrupt number */
#define INT_ADDR_INT0     0x0003    // interrupt vector address for INT0 or LED
#define INT_ADDR_TMR0     0x000B    // interrupt vector address for timer0
#define INT_ADDR_INT1     0x0013    // interrupt vector address for INT1
#define INT_ADDR_TMR1     0x001B    // interrupt vector address for timer1
#define INT_ADDR_UART0    0x0023    // interrupt vector address for UART0
#define INT_ADDR_TMR2     0x002B    // interrupt vector address for timer2
#define INT_ADDR_SPI0     0x0033    // interrupt vector address for SPI0
#define INT_ADDR_TMR3     0x003B    // interrupt vector address for timer3
#define INT_ADDR_USB      0x0043    // interrupt vector address for USB
#define INT_ADDR_ADC      0x004B    // interrupt vector address for ADC
#define INT_ADDR_UART1    0x0053    // interrupt vector address for UART1
#define INT_ADDR_PWM1     0x005B    // interrupt vector address for PWM1
#define INT_ADDR_GPIO     0x0063    // interrupt vector address for GPIO
#define INT_ADDR_WDOG     0x006B    // interrupt vector address for watch-dog timer
#define INT_NO_INT0       0         // interrupt number for INT0 or LED
#define INT_NO_TMR0       1         // interrupt number for timer0
#define INT_NO_INT1       2         // interrupt number for INT1
#define INT_NO_TMR1       3         // interrupt number for timer1
#define INT_NO_UART0      4         // interrupt number for UART0
#define INT_NO_TMR2       5         // interrupt number for timer2
#define INT_NO_SPI0       6         // interrupt number for SPI0
#define INT_NO_TMR3       7         // interrupt number for timer3
#define INT_NO_USB        8         // interrupt number for USB
#define INT_NO_ADC        9         // interrupt number for ADC
#define INT_NO_UART1      10        // interrupt number for UART1
#define INT_NO_PWM1       11        // interrupt number for PWM1
#define INT_NO_GPIO       12        // interrupt number for GPIO
#define INT_NO_WDOG       13        // interrupt number for watch-dog timer

/* Special Program Space */
#define DATA_FLASH_ADDR   0xF000    // start address of Data-Flash
#define BOOT_LOAD_ADDR    0xF400    // start address of boot loader program
#define ROM_CFG_ADDR      0xFFFE    // chip configuration information address

/*
New Instruction:   MOVX @DPTR1,A
Instruction Code:  0xA5
Instruction Cycle: 1
Instruction Operation:
   step-1. write ACC @DPTR1 into xdata SRAM embedded chip
   step-2. increase DPTR1
ASM example:
       INC  XBUS_AUX
       MOV  DPTR,#TARGET_ADDR ;DPTR1
       DEC  XBUS_AUX
       MOV  DPTR,#SOURCE_ADDR ;DPTR0
       MOV  R7,#xxH
 LOOP: MOVX A,@DPTR ;DPTR0
       INC  DPTR    ;DPTR0, if need
       DB   0A5H    ;MOVX @DPTR1,A & INC DPTR1
       DJNZ R7,LOOP
*/



#endif  // __CH559_H__

#ifndef __USB_DEF__
#define __USB_DEF__



/*----- USB constant and structure define --------------------------------*/

/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL            0x00    /* reserved PID */
#define USB_PID_SOF             0x05
#define USB_PID_SETUP           0x0D
#define USB_PID_IN              0x09
#define USB_PID_OUT             0x01
#define USB_PID_ACK             0x02
#define USB_PID_NAK             0x0A
#define USB_PID_STALL           0x0E
#define USB_PID_DATA0           0x03
#define USB_PID_DATA1           0x0B
#define USB_PID_PRE             0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS          0x00
#define USB_CLEAR_FEATURE       0x01
#define USB_SET_FEATURE         0x03
#define USB_SET_ADDRESS         0x05
#define USB_GET_DESCRIPTOR      0x06
#define USB_SET_DESCRIPTOR      0x07
#define USB_GET_CONFIGURATION   0x08
#define USB_SET_CONFIGURATION   0x09
#define USB_GET_INTERFACE       0x0A
#define USB_SET_INTERFACE       0x0B
#define USB_SYNCH_FRAME         0x0C
#endif

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS          0x00
#define HUB_CLEAR_FEATURE       0x01
#define HUB_GET_STATE           0x02
#define HUB_SET_FEATURE         0x03
#define HUB_GET_DESCRIPTOR      0x06
#define HUB_SET_DESCRIPTOR      0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT          0x01
#define HID_GET_IDLE            0x02
#define HID_GET_PROTOCOL        0x03
#define HID_SET_REPORT          0x09
#define HID_SET_IDLE            0x0A
#define HID_SET_PROTOCOL        0x0B
#endif

/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN          0x80            /* control IN, device to host */
#define USB_REQ_TYP_OUT         0x00            /* control OUT, host to device */
#define USB_REQ_TYP_READ        0x80            /* control read, device to host */
#define USB_REQ_TYP_WRITE       0x00            /* control write, host to device */
#define USB_REQ_TYP_MASK        0x60            /* bit mask of request type */
#define USB_REQ_TYP_STANDARD    0x00
#define USB_REQ_TYP_CLASS       0x20
#define USB_REQ_TYP_VENDOR      0x40
#define USB_REQ_TYP_RESERVED    0x60
#define USB_REQ_RECIP_MASK      0x1F            /* bit mask of request recipient */
#define USB_REQ_RECIP_DEVICE    0x00
#define USB_REQ_RECIP_INTERF    0x01
#define USB_REQ_RECIP_ENDP      0x02
#define USB_REQ_RECIP_OTHER     0x03
#endif

/* USB request type for hub class request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE   0x20
#define HUB_CLEAR_PORT_FEATURE  0x23
#define HUB_GET_BUS_STATE       0xA3
#define HUB_GET_HUB_DESCRIPTOR  0xA0
#define HUB_GET_HUB_STATUS      0xA0
#define HUB_GET_PORT_STATUS     0xA3
#define HUB_SET_HUB_DESCRIPTOR  0x20
#define HUB_SET_HUB_FEATURE     0x20
#define HUB_SET_PORT_FEATURE    0x23
#endif

/* Hub class feature selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER   0
#define HUB_C_HUB_OVER_CURRENT  1
#define HUB_PORT_CONNECTION     0
#define HUB_PORT_ENABLE         1
#define HUB_PORT_SUSPEND        2
#define HUB_PORT_OVER_CURRENT   3
#define HUB_PORT_RESET          4
#define HUB_PORT_POWER          8
#define HUB_PORT_LOW_SPEED      9
#define HUB_C_PORT_CONNECTION   16
#define HUB_C_PORT_ENABLE       17
#define HUB_C_PORT_SUSPEND      18
#define HUB_C_PORT_OVER_CURRENT 19
#define HUB_C_PORT_RESET        20
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE    0x01
#define USB_DESCR_TYP_CONFIG    0x02
#define USB_DESCR_TYP_STRING    0x03
#define USB_DESCR_TYP_INTERF    0x04
#define USB_DESCR_TYP_ENDP      0x05
#define USB_DESCR_TYP_QUALIF    0x06
#define USB_DESCR_TYP_SPEED     0x07
#define USB_DESCR_TYP_OTG       0x09
#define USB_DESCR_TYP_HID       0x21
#define USB_DESCR_TYP_REPORT    0x22
#define USB_DESCR_TYP_PHYSIC    0x23
#define USB_DESCR_TYP_CS_INTF   0x24
#define USB_DESCR_TYP_CS_ENDP   0x25
#define USB_DESCR_TYP_HUB       0x29
#endif

/* USB device class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED  0x00
#define USB_DEV_CLASS_AUDIO     0x01
#define USB_DEV_CLASS_COMMUNIC  0x02
#define USB_DEV_CLASS_HID       0x03
#define USB_DEV_CLASS_MONITOR   0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER     0x06
#define USB_DEV_CLASS_PRINTER   0x07
#define USB_DEV_CLASS_STORAGE   0x08
#define USB_DEV_CLASS_HUB       0x09
#define USB_DEV_CLASS_VEN_SPEC  0xFF
#endif

/* USB endpoint type and attributes */
#ifndef USB_ENDP_TYPE_MASK
#define USB_ENDP_DIR_MASK       0x80
#define USB_ENDP_ADDR_MASK      0x0F
#define USB_ENDP_TYPE_MASK      0x03
#define USB_ENDP_TYPE_CTRL      0x00
#define USB_ENDP_TYPE_ISOCH     0x01
#define USB_ENDP_TYPE_BULK      0x02
#define USB_ENDP_TYPE_INTER     0x03
#endif

#ifndef USB_DEVICE_ADDR
#define	USB_DEVICE_ADDR			0x02	/* 默认的USB设备地址 */
#endif
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE      8       /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE         64      /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE			0x1F	/* 命令块CBW的总长度 */
#define USB_BO_CSW_SIZE			0x0D	/* 命令状态块CSW的总长度 */
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0         0x55    /* 命令块CBW识别标志'USBC' */
#define USB_BO_CBW_SIG1         0x53
#define USB_BO_CBW_SIG2         0x42
#define USB_BO_CBW_SIG3         0x43
#define USB_BO_CSW_SIG0         0x55    /* 命令状态块CSW识别标志'USBS' */
#define USB_BO_CSW_SIG1         0x53
#define USB_BO_CSW_SIG2         0x42
#define USB_BO_CSW_SIG3         0x53
#endif

typedef struct _USB_SETUP_REQ {
    UINT8 bRequestType;
    UINT8 bRequest;
    UINT8 wValueL;
    UINT8 wValueH;
    UINT8 wIndexL;
    UINT8 wIndexH;
    UINT8 wLengthL;
    UINT8 wLengthH;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

typedef USB_SETUP_REQ xdata *PXUSB_SETUP_REQ;

typedef struct _USB_DEVICE_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdUSBL;
    UINT8 bcdUSBH;
    UINT8 bDeviceClass;
    UINT8 bDeviceSubClass;
    UINT8 bDeviceProtocol;
    UINT8 bMaxPacketSize0;
    UINT8 idVendorL;
    UINT8 idVendorH;
    UINT8 idProductL;
    UINT8 idProductH;
    UINT8 bcdDeviceL;
    UINT8 bcdDeviceH;
    UINT8 iManufacturer;
    UINT8 iProduct;
    UINT8 iSerialNumber;
    UINT8 bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

typedef USB_DEV_DESCR xdata *PXUSB_DEV_DESCR;

typedef struct _USB_CONFIG_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 wTotalLengthL;
    UINT8 wTotalLengthH;
    UINT8 bNumInterfaces;
    UINT8 bConfigurationValue;
    UINT8 iConfiguration;
    UINT8 bmAttributes;
    UINT8 MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

typedef USB_CFG_DESCR xdata *PXUSB_CFG_DESCR;

typedef struct _USB_INTERF_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bInterfaceNumber;
    UINT8 bAlternateSetting;
    UINT8 bNumEndpoints;
    UINT8 bInterfaceClass;
    UINT8 bInterfaceSubClass;
    UINT8 bInterfaceProtocol;
    UINT8 iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

typedef USB_ITF_DESCR xdata *PXUSB_ITF_DESCR;

typedef struct _USB_ENDPOINT_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bEndpointAddress;
    UINT8 bmAttributes;
    UINT8 wMaxPacketSizeL;
    UINT8 wMaxPacketSizeH;
    UINT8 bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

typedef USB_ENDP_DESCR xdata *PXUSB_ENDP_DESCR;

typedef struct _USB_CONFIG_DESCR_LONG {
    USB_CFG_DESCR   cfg_descr;
    USB_ITF_DESCR   itf_descr;
    USB_ENDP_DESCR  endp_descr[1];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

typedef USB_CFG_DESCR_LONG xdata *PXUSB_CFG_DESCR_LONG;

typedef struct _USB_HUB_DESCR {
    UINT8 bDescLength;
    UINT8 bDescriptorType;
    UINT8 bNbrPorts;
    UINT8 wHubCharacteristicsL;
    UINT8 wHubCharacteristicsH;
    UINT8 bPwrOn2PwrGood;
    UINT8 bHubContrCurrent;
    UINT8 DeviceRemovable;
    UINT8 PortPwrCtrlMask;
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

typedef USB_HUB_DESCR xdata *PXUSB_HUB_DESCR;

typedef struct _USB_HID_DESCR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdHIDL;
    UINT8 bcdHIDH;
    UINT8 bCountryCode;
    UINT8 bNumDescriptors;
    UINT8 bDescriptorTypeX;
    UINT8 wDescriptorLengthL;
    UINT8 wDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

typedef USB_HID_DESCR xdata *PXUSB_HID_DESCR;

typedef struct _UDISK_BOC_CBW {         /* command of BulkOnly USB-FlashDisk */
    UINT8 mCBW_Sig0;
    UINT8 mCBW_Sig1;
    UINT8 mCBW_Sig2;
    UINT8 mCBW_Sig3;
    UINT8 mCBW_Tag0;
    UINT8 mCBW_Tag1;
    UINT8 mCBW_Tag2;
    UINT8 mCBW_Tag3;
    UINT8 mCBW_DataLen0;
    UINT8 mCBW_DataLen1;
    UINT8 mCBW_DataLen2;
    UINT8 mCBW_DataLen3;                /* uppest byte of data length, always is 0 */
    UINT8 mCBW_Flag;                    /* transfer direction and etc. */
    UINT8 mCBW_LUN;
    UINT8 mCBW_CB_Len;                  /* length of command block */
    UINT8 mCBW_CB_Buf[16];              /* command block buffer */
} UDISK_BOC_CBW, *PUDISK_BOC_CBW;

typedef UDISK_BOC_CBW xdata *PXUDISK_BOC_CBW;

typedef struct _UDISK_BOC_CSW {         /* status of BulkOnly USB-FlashDisk */
    UINT8 mCSW_Sig0;
    UINT8 mCSW_Sig1;
    UINT8 mCSW_Sig2;
    UINT8 mCSW_Sig3;
    UINT8 mCSW_Tag0;
    UINT8 mCSW_Tag1;
    UINT8 mCSW_Tag2;
    UINT8 mCSW_Tag3;
    UINT8 mCSW_Residue0;                /* return: remainder bytes */
    UINT8 mCSW_Residue1;
    UINT8 mCSW_Residue2;
    UINT8 mCSW_Residue3;                /* uppest byte of remainder length, always is 0 */
    UINT8 mCSW_Status;                  /* return: result status */
} UDISK_BOC_CSW, *PUDISK_BOC_CSW;

typedef UDISK_BOC_CSW xdata *PXUDISK_BOC_CSW;



#endif  // __USB_DEF__
