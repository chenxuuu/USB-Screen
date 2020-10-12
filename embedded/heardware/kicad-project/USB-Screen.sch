EESchema Schematic File Version 4
LIBS:USB-Screen-cache
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "USB-Screen"
Date "2020-09-15"
Rev "1.0"
Comp "chenxuuu"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L USB_Screen_Library:CH551 U2
U 1 1 5F6027B1
P 7250 3350
F 0 "U2" H 7250 3750 50  0000 C CNN
F 1 "CH551" H 7250 2800 50  0000 C CNN
F 2 "USB-Screen:SOP-16_4.55x10.3mm_P1.27mm" H 7250 2600 50  0001 C CNN
F 3 "" H 7250 2600 50  0001 C CNN
	1    7250 3350
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J1
U 1 1 5F605CF0
P 1650 1500
F 0 "J1" H 1550 1850 50  0000 R CNN
F 1 "USB_B_Micro" H 2150 1150 50  0000 R CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 1800 1450 50  0001 C CNN
F 3 "~" H 1800 1450 50  0001 C CNN
	1    1650 1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5F60B2C0
P 1650 1900
F 0 "#PWR0101" H 1650 1650 50  0001 C CNN
F 1 "GND" H 1655 1727 50  0000 C CNN
F 2 "" H 1650 1900 50  0001 C CNN
F 3 "" H 1650 1900 50  0001 C CNN
	1    1650 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5F60BFB5
P 1550 1900
F 0 "#PWR0102" H 1550 1650 50  0001 C CNN
F 1 "GND" H 1555 1727 50  0000 C CNN
F 2 "" H 1550 1900 50  0001 C CNN
F 3 "" H 1550 1900 50  0001 C CNN
	1    1550 1900
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0103
U 1 1 5F60C4BF
P 1950 1300
F 0 "#PWR0103" H 1950 1150 50  0001 C CNN
F 1 "+5V" V 1965 1428 50  0000 L CNN
F 2 "" H 1950 1300 50  0001 C CNN
F 3 "" H 1950 1300 50  0001 C CNN
	1    1950 1300
	0    1    1    0   
$EndComp
Wire Wire Line
	7800 3400 8200 3400
Wire Wire Line
	7800 3500 8200 3500
Text Label 8200 3500 2    50   ~ 0
USB_D+
Text Label 8200 3400 2    50   ~ 0
USB_D-
$Comp
L power:GND #PWR0104
U 1 1 5F614AB8
P 8200 3300
F 0 "#PWR0104" H 8200 3050 50  0001 C CNN
F 1 "GND" V 8205 3172 50  0000 R CNN
F 2 "" H 8200 3300 50  0001 C CNN
F 3 "" H 8200 3300 50  0001 C CNN
	1    8200 3300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8200 3300 7800 3300
Wire Wire Line
	7800 3200 8650 3200
$Comp
L power:+5V #PWR0107
U 1 1 5F61990A
P 8800 3200
F 0 "#PWR0107" H 8800 3050 50  0001 C CNN
F 1 "+5V" V 8815 3328 50  0000 L CNN
F 2 "" H 8800 3200 50  0001 C CNN
F 3 "" H 8800 3200 50  0001 C CNN
	1    8800 3200
	0    1    1    0   
$EndComp
$Comp
L Device:C C5
U 1 1 5F61A372
P 8650 3400
F 0 "C5" H 8765 3446 50  0000 L CNN
F 1 "0.1uF" H 8765 3355 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8688 3250 50  0001 C CNN
F 3 "~" H 8650 3400 50  0001 C CNN
	1    8650 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 3250 8650 3200
Connection ~ 8650 3200
Wire Wire Line
	8650 3200 8800 3200
Wire Wire Line
	8650 3550 8650 3650
$Comp
L power:GND #PWR0108
U 1 1 5F61AEAB
P 8650 3650
F 0 "#PWR0108" H 8650 3400 50  0001 C CNN
F 1 "GND" H 8655 3477 50  0000 C CNN
F 2 "" H 8650 3650 50  0001 C CNN
F 3 "" H 8650 3650 50  0001 C CNN
	1    8650 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5F61B246
P 8050 3100
F 0 "C3" V 7798 3100 50  0000 C CNN
F 1 "0.1uF" V 7889 3100 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8088 2950 50  0001 C CNN
F 3 "~" H 8050 3100 50  0001 C CNN
	1    8050 3100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5F61BFD0
P 8400 3100
F 0 "#PWR0109" H 8400 2850 50  0001 C CNN
F 1 "GND" V 8405 2972 50  0000 R CNN
F 2 "" H 8400 3100 50  0001 C CNN
F 3 "" H 8400 3100 50  0001 C CNN
	1    8400 3100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7800 3100 7900 3100
Wire Wire Line
	8200 3100 8400 3100
$Comp
L power:GND #PWR0110
U 1 1 5F61EFA4
P 2000 5600
F 0 "#PWR0110" H 2000 5350 50  0001 C CNN
F 1 "GND" H 2005 5427 50  0000 C CNN
F 2 "" H 2000 5600 50  0001 C CNN
F 3 "" H 2000 5600 50  0001 C CNN
	1    2000 5600
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0111
U 1 1 5F61F58D
P 1200 5150
F 0 "#PWR0111" H 1200 5000 50  0001 C CNN
F 1 "+5V" V 1215 5278 50  0000 L CNN
F 2 "" H 1200 5150 50  0001 C CNN
F 3 "" H 1200 5150 50  0001 C CNN
	1    1200 5150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1200 5150 1350 5150
Connection ~ 2000 5500
Wire Wire Line
	2000 5500 2000 5600
Connection ~ 1350 5150
$Comp
L power:+3.3V #PWR0112
U 1 1 5F621918
P 2700 5150
F 0 "#PWR0112" H 2700 5000 50  0001 C CNN
F 1 "+3.3V" V 2715 5278 50  0000 L CNN
F 2 "" H 2700 5150 50  0001 C CNN
F 3 "" H 2700 5150 50  0001 C CNN
	1    2700 5150
	0    1    1    0   
$EndComp
Wire Wire Line
	1350 5500 2000 5500
Wire Wire Line
	1350 5200 1350 5150
Wire Wire Line
	1350 5150 1650 5150
Wire Wire Line
	2700 5150 2500 5150
Wire Wire Line
	2000 5450 2000 5500
$Comp
L Device:C C2
U 1 1 5F620133
P 1350 5350
F 0 "C2" H 1465 5396 50  0000 L CNN
F 1 "1uF" H 1465 5305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1388 5200 50  0001 C CNN
F 3 "~" H 1350 5350 50  0001 C CNN
	1    1350 5350
	1    0    0    -1  
$EndComp
$Comp
L USB_Screen_Library:SC662 U3
U 1 1 5F61E515
P 2000 5200
F 0 "U3" H 2000 5400 50  0000 C CNN
F 1 "SC662" H 2150 5000 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2000 5350 50  0001 C CNN
F 3 "" H 2000 5350 50  0001 C CNN
	1    2000 5200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5F6242DA
P 2500 5350
F 0 "C4" H 2615 5396 50  0000 L CNN
F 1 "1uF" H 2615 5305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2538 5200 50  0001 C CNN
F 3 "~" H 2500 5350 50  0001 C CNN
	1    2500 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2500 5200 2500 5150
Connection ~ 2500 5150
Wire Wire Line
	2500 5150 2350 5150
Wire Wire Line
	2500 5500 2000 5500
$Comp
L USB_Screen_Library:ZJY154T-PG04 U1
U 1 1 5F62CDD2
P 1350 3450
F 0 "U1" H 1450 2750 50  0000 L CNN
F 1 "ZJY154T-PG04" H 1100 4100 50  0000 L CNN
F 2 "USB-Screen:ZJY154T-PG04" H 1250 4100 50  0001 C CNN
F 3 "" H 1250 4100 50  0001 C CNN
	1    1350 3450
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5F62DA43
P 2250 3950
F 0 "#PWR04" H 2250 3700 50  0001 C CNN
F 1 "GND" V 2255 3822 50  0000 R CNN
F 2 "" H 2250 3950 50  0001 C CNN
F 3 "" H 2250 3950 50  0001 C CNN
	1    2250 3950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2250 3950 2200 3950
$Comp
L power:GND #PWR02
U 1 1 5F62FDF8
P 1800 3500
F 0 "#PWR02" H 1800 3250 50  0001 C CNN
F 1 "GND" V 1805 3372 50  0000 R CNN
F 2 "" H 1800 3500 50  0001 C CNN
F 3 "" H 1800 3500 50  0001 C CNN
	1    1800 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1600 3550 1800 3550
Wire Wire Line
	1800 3550 1800 3500
Wire Wire Line
	1600 3450 1800 3450
Wire Wire Line
	1800 3450 1800 3500
Connection ~ 1800 3500
$Comp
L power:GND #PWR01
U 1 1 5F6311BD
P 1750 2850
F 0 "#PWR01" H 1750 2600 50  0001 C CNN
F 1 "GND" H 1755 2677 50  0000 C CNN
F 2 "" H 1750 2850 50  0001 C CNN
F 3 "" H 1750 2850 50  0001 C CNN
	1    1750 2850
	-1   0    0    1   
$EndComp
Wire Wire Line
	1600 2850 1750 2850
$Comp
L power:+3.3V #PWR03
U 1 1 5F631EFC
P 2250 3650
F 0 "#PWR03" H 2250 3500 50  0001 C CNN
F 1 "+3.3V" V 2265 3778 50  0000 L CNN
F 2 "" H 2250 3650 50  0001 C CNN
F 3 "" H 2250 3650 50  0001 C CNN
	1    2250 3650
	0    1    1    0   
$EndComp
Wire Wire Line
	2250 3650 2200 3650
$Comp
L Device:C C1
U 1 1 5F636325
P 2200 3800
F 0 "C1" H 2315 3846 50  0000 L CNN
F 1 "0.1uF" H 2315 3755 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2238 3650 50  0001 C CNN
F 3 "~" H 2200 3800 50  0001 C CNN
	1    2200 3800
	-1   0    0    1   
$EndComp
Connection ~ 2200 3950
Wire Wire Line
	2200 3950 1850 3950
Connection ~ 2200 3650
Wire Wire Line
	2200 3650 1600 3650
Wire Wire Line
	1850 3850 1600 3850
Wire Wire Line
	1850 3750 1600 3750
Text Label 1850 3750 2    50   ~ 0
KEDA
Wire Wire Line
	1800 3350 1600 3350
Wire Wire Line
	1800 3250 1600 3250
Wire Wire Line
	1800 3150 1600 3150
Wire Wire Line
	1800 3050 1600 3050
Wire Wire Line
	1800 2950 1600 2950
Text Label 1800 3350 2    50   ~ 0
DC
Text Label 1800 3250 2    50   ~ 0
CS
Text Label 1800 3150 2    50   ~ 0
SCL
Text Label 1800 3050 2    50   ~ 0
SDA
Text Label 1800 2950 2    50   ~ 0
RST
Wire Wire Line
	6500 3200 6700 3200
Text Label 6500 3200 0    50   ~ 0
CS
Wire Wire Line
	8200 3600 7800 3600
Wire Wire Line
	8200 3700 7800 3700
Wire Wire Line
	8200 3800 7800 3800
Text Label 8200 3600 2    50   ~ 0
PWM
Wire Wire Line
	1850 3850 1850 3950
Connection ~ 1850 3950
Wire Wire Line
	1850 3950 1600 3950
Wire Wire Line
	6500 3100 6700 3100
Wire Wire Line
	6500 3700 6700 3700
Wire Wire Line
	6500 3800 6700 3800
Text Label 6500 3100 0    50   ~ 0
RST
Text Label 6500 3700 0    50   ~ 0
DC
Text Label 6500 3800 0    50   ~ 0
LED1
Text Label 8200 3700 2    50   ~ 0
LED2
Text Label 8200 3800 2    50   ~ 0
LED3
$Comp
L Device:LED D1
U 1 1 5F681B3F
P 4300 3050
F 0 "D1" H 4300 3150 50  0000 C CNN
F 1 "LED" H 4450 3000 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4300 3050 50  0001 C CNN
F 3 "~" H 4300 3050 50  0001 C CNN
	1    4300 3050
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5F682560
P 4300 3300
F 0 "D2" H 4300 3400 50  0000 C CNN
F 1 "LED" H 4450 3250 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4300 3300 50  0001 C CNN
F 3 "~" H 4300 3300 50  0001 C CNN
	1    4300 3300
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5F682D92
P 4300 3550
F 0 "D3" H 4300 3650 50  0000 C CNN
F 1 "LED" H 4450 3500 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4300 3550 50  0001 C CNN
F 3 "~" H 4300 3550 50  0001 C CNN
	1    4300 3550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5F684B7B
P 4000 3050
F 0 "#PWR05" H 4000 2800 50  0001 C CNN
F 1 "GND" V 4005 2922 50  0000 R CNN
F 2 "" H 4000 3050 50  0001 C CNN
F 3 "" H 4000 3050 50  0001 C CNN
	1    4000 3050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5F685180
P 4000 3300
F 0 "#PWR06" H 4000 3050 50  0001 C CNN
F 1 "GND" V 4005 3172 50  0000 R CNN
F 2 "" H 4000 3300 50  0001 C CNN
F 3 "" H 4000 3300 50  0001 C CNN
	1    4000 3300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5F6854D2
P 4000 3550
F 0 "#PWR07" H 4000 3300 50  0001 C CNN
F 1 "GND" V 4005 3422 50  0000 R CNN
F 2 "" H 4000 3550 50  0001 C CNN
F 3 "" H 4000 3550 50  0001 C CNN
	1    4000 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	4000 3550 4150 3550
Wire Wire Line
	4000 3300 4150 3300
Wire Wire Line
	4000 3050 4150 3050
$Comp
L Device:R R3
U 1 1 5F68A7DE
P 4750 3050
F 0 "R3" V 4650 3050 50  0000 C CNN
F 1 "33R" V 4750 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4680 3050 50  0001 C CNN
F 3 "~" H 4750 3050 50  0001 C CNN
	1    4750 3050
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5F68B5C4
P 4750 3300
F 0 "R4" V 4650 3300 50  0000 C CNN
F 1 "33R" V 4750 3300 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4680 3300 50  0001 C CNN
F 3 "~" H 4750 3300 50  0001 C CNN
	1    4750 3300
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5F68CF79
P 4750 3550
F 0 "R5" V 4650 3550 50  0000 C CNN
F 1 "33R" V 4750 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4680 3550 50  0001 C CNN
F 3 "~" H 4750 3550 50  0001 C CNN
	1    4750 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	4450 3050 4600 3050
Wire Wire Line
	4450 3300 4600 3300
Wire Wire Line
	4450 3550 4600 3550
Wire Wire Line
	4900 3050 5150 3050
Wire Wire Line
	4900 3300 5150 3300
Wire Wire Line
	4900 3550 5150 3550
Text Label 5150 3050 2    50   ~ 0
LED1
Text Label 5150 3300 2    50   ~ 0
LED2
Text Label 5150 3550 2    50   ~ 0
LED3
NoConn ~ 1950 1700
NoConn ~ 6700 3400
NoConn ~ 6700 3600
Text Label 3300 1500 2    50   ~ 0
USB_D+
Text Label 3300 1600 2    50   ~ 0
USB_D-
$Comp
L power:GND #PWR0106
U 1 1 5F618012
P 3300 1400
F 0 "#PWR0106" H 3300 1150 50  0001 C CNN
F 1 "GND" H 3305 1227 50  0000 C CNN
F 2 "" H 3300 1400 50  0001 C CNN
F 3 "" H 3300 1400 50  0001 C CNN
	1    3300 1400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 1400 3000 1400
Wire Wire Line
	3300 1500 3000 1500
Wire Wire Line
	3300 1600 3000 1600
Text Label 2350 1600 2    50   ~ 0
USB_D-
Text Label 2350 1500 2    50   ~ 0
USB_D+
Wire Wire Line
	1950 1600 2350 1600
Wire Wire Line
	1950 1500 2350 1500
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 5F605182
P 2800 1600
F 0 "J2" H 2800 1300 50  0000 C CNN
F 1 "Conn_01x04" H 2700 1850 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 2800 1600 50  0001 C CNN
F 3 "~" H 2800 1600 50  0001 C CNN
	1    2800 1600
	-1   0    0    1   
$EndComp
Wire Wire Line
	3300 1700 3000 1700
$Comp
L power:+5V #PWR0105
U 1 1 5F617214
P 3300 1700
F 0 "#PWR0105" H 3300 1550 50  0001 C CNN
F 1 "+5V" V 3315 1828 50  0000 L CNN
F 2 "" H 3300 1700 50  0001 C CNN
F 3 "" H 3300 1700 50  0001 C CNN
	1    3300 1700
	0    1    1    0   
$EndComp
$Comp
L USB-Screen-rescue:2SC4213-Transistor_BJT Q1
U 1 1 5F727A10
P 5100 4800
F 0 "Q1" H 5291 4754 50  0000 L CNN
F 1 "NPN" H 5291 4845 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5300 4725 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=19305&prodName=2SC4213" H 5100 4800 50  0001 L CNN
	1    5100 4800
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0114
U 1 1 5F730CBA
P 5000 4400
F 0 "#PWR0114" H 5000 4250 50  0001 C CNN
F 1 "+5V" H 5015 4573 50  0000 C CNN
F 2 "" H 5000 4400 50  0001 C CNN
F 3 "" H 5000 4400 50  0001 C CNN
	1    5000 4400
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5000 4400 5000 4600
Wire Wire Line
	5000 5150 5000 5000
Wire Wire Line
	5300 4800 5600 4800
Text Label 5600 4800 2    50   ~ 0
PWM
Text Label 4050 5150 0    50   ~ 0
KEDA
$Comp
L Device:R R6
U 1 1 5F74A98E
P 4600 5150
F 0 "R6" V 4500 5150 50  0000 C CNN
F 1 "33R" V 4600 5150 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4530 5150 50  0001 C CNN
F 3 "~" H 4600 5150 50  0001 C CNN
	1    4600 5150
	0    -1   1    0   
$EndComp
Wire Wire Line
	4050 5150 4450 5150
Wire Wire Line
	4750 5150 5000 5150
$Comp
L Device:D_Schottky D5
U 1 1 5F848FB6
P 6250 3500
F 0 "D5" H 6350 3600 50  0000 C CNN
F 1 "SS14" H 6150 3600 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 6250 3500 50  0001 C CNN
F 3 "~" H 6250 3500 50  0001 C CNN
	1    6250 3500
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Schottky D4
U 1 1 5F84B388
P 6250 3300
F 0 "D4" H 6350 3400 50  0000 C CNN
F 1 "SS14" H 6150 3400 50  0000 C CNN
F 2 "Diode_SMD:D_SMA" H 6250 3300 50  0001 C CNN
F 3 "~" H 6250 3300 50  0001 C CNN
	1    6250 3300
	-1   0    0    1   
$EndComp
Wire Wire Line
	6400 3300 6700 3300
Wire Wire Line
	6700 3500 6400 3500
Wire Wire Line
	5750 3300 5950 3300
Wire Wire Line
	5750 3500 6050 3500
Text Label 5750 3500 0    50   ~ 0
SCL
Text Label 5750 3300 0    50   ~ 0
SDA
$Comp
L Device:R R2
U 1 1 5F860967
P 6050 3050
F 0 "R2" V 5950 3050 50  0000 C CNN
F 1 "10K" V 6050 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5980 3050 50  0001 C CNN
F 3 "~" H 6050 3050 50  0001 C CNN
	1    6050 3050
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5F8654DD
P 5950 3050
F 0 "R1" V 5850 3050 50  0000 C CNN
F 1 "10K" V 5950 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 5880 3050 50  0001 C CNN
F 3 "~" H 5950 3050 50  0001 C CNN
	1    5950 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 3200 5950 3300
Connection ~ 5950 3300
Wire Wire Line
	5950 3300 6100 3300
Wire Wire Line
	6050 3200 6050 3500
Connection ~ 6050 3500
Wire Wire Line
	6050 3500 6100 3500
$Comp
L power:+3.3V #PWR08
U 1 1 5F873B8C
P 5950 2850
F 0 "#PWR08" H 5950 2700 50  0001 C CNN
F 1 "+3.3V" H 5965 3023 50  0000 C CNN
F 2 "" H 5950 2850 50  0001 C CNN
F 3 "" H 5950 2850 50  0001 C CNN
	1    5950 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	5950 2850 5950 2900
Wire Wire Line
	5950 2850 6050 2850
Wire Wire Line
	6050 2850 6050 2900
Connection ~ 5950 2850
$EndSCHEMATC
