EESchema Schematic File Version 4
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
P 7550 4150
F 0 "U2" H 7550 4550 50  0000 C CNN
F 1 "CH551" H 7550 3600 50  0000 C CNN
F 2 "USB-Screen:SOP-16_4.55x10.3mm_P1.27mm" H 7550 3400 50  0001 C CNN
F 3 "" H 7550 3400 50  0001 C CNN
	1    7550 4150
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J1
U 1 1 5F605CF0
P 3600 1850
F 0 "J1" H 3500 2200 50  0000 R CNN
F 1 "USB_B_Micro" H 4100 1500 50  0000 R CNN
F 2 "Connector_USB:USB_Micro-B_Molex-105017-0001" H 3750 1800 50  0001 C CNN
F 3 "~" H 3750 1800 50  0001 C CNN
	1    3600 1850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0101
U 1 1 5F60B2C0
P 3600 2250
F 0 "#PWR0101" H 3600 2000 50  0001 C CNN
F 1 "GND" H 3605 2077 50  0000 C CNN
F 2 "" H 3600 2250 50  0001 C CNN
F 3 "" H 3600 2250 50  0001 C CNN
	1    3600 2250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5F60BFB5
P 3500 2250
F 0 "#PWR0102" H 3500 2000 50  0001 C CNN
F 1 "GND" H 3505 2077 50  0000 C CNN
F 2 "" H 3500 2250 50  0001 C CNN
F 3 "" H 3500 2250 50  0001 C CNN
	1    3500 2250
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0103
U 1 1 5F60C4BF
P 3900 1650
F 0 "#PWR0103" H 3900 1500 50  0001 C CNN
F 1 "+5V" V 3915 1778 50  0000 L CNN
F 2 "" H 3900 1650 50  0001 C CNN
F 3 "" H 3900 1650 50  0001 C CNN
	1    3900 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	8100 4200 8500 4200
Wire Wire Line
	8100 4300 8500 4300
Text Label 8500 4300 2    50   ~ 0
USB_D+
Text Label 8500 4200 2    50   ~ 0
USB_D-
$Comp
L power:GND #PWR0104
U 1 1 5F614AB8
P 8500 4100
F 0 "#PWR0104" H 8500 3850 50  0001 C CNN
F 1 "GND" V 8505 3972 50  0000 R CNN
F 2 "" H 8500 4100 50  0001 C CNN
F 3 "" H 8500 4100 50  0001 C CNN
	1    8500 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8500 4100 8100 4100
Wire Wire Line
	8100 4000 8950 4000
$Comp
L power:+5V #PWR0107
U 1 1 5F61990A
P 9100 4000
F 0 "#PWR0107" H 9100 3850 50  0001 C CNN
F 1 "+5V" V 9115 4128 50  0000 L CNN
F 2 "" H 9100 4000 50  0001 C CNN
F 3 "" H 9100 4000 50  0001 C CNN
	1    9100 4000
	0    1    1    0   
$EndComp
$Comp
L Device:C C5
U 1 1 5F61A372
P 8950 4200
F 0 "C5" H 9065 4246 50  0000 L CNN
F 1 "0.1uF" H 9065 4155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8988 4050 50  0001 C CNN
F 3 "~" H 8950 4200 50  0001 C CNN
	1    8950 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 4050 8950 4000
Connection ~ 8950 4000
Wire Wire Line
	8950 4000 9100 4000
Wire Wire Line
	8950 4350 8950 4450
$Comp
L power:GND #PWR0108
U 1 1 5F61AEAB
P 8950 4450
F 0 "#PWR0108" H 8950 4200 50  0001 C CNN
F 1 "GND" H 8955 4277 50  0000 C CNN
F 2 "" H 8950 4450 50  0001 C CNN
F 3 "" H 8950 4450 50  0001 C CNN
	1    8950 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C3
U 1 1 5F61B246
P 8350 3900
F 0 "C3" V 8098 3900 50  0000 C CNN
F 1 "0.1uF" V 8189 3900 50  0000 C CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 8388 3750 50  0001 C CNN
F 3 "~" H 8350 3900 50  0001 C CNN
	1    8350 3900
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0109
U 1 1 5F61BFD0
P 8700 3900
F 0 "#PWR0109" H 8700 3650 50  0001 C CNN
F 1 "GND" V 8705 3772 50  0000 R CNN
F 2 "" H 8700 3900 50  0001 C CNN
F 3 "" H 8700 3900 50  0001 C CNN
	1    8700 3900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8100 3900 8200 3900
Wire Wire Line
	8500 3900 8700 3900
$Comp
L power:GND #PWR0110
U 1 1 5F61EFA4
P 2300 6400
F 0 "#PWR0110" H 2300 6150 50  0001 C CNN
F 1 "GND" H 2305 6227 50  0000 C CNN
F 2 "" H 2300 6400 50  0001 C CNN
F 3 "" H 2300 6400 50  0001 C CNN
	1    2300 6400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0111
U 1 1 5F61F58D
P 1500 5950
F 0 "#PWR0111" H 1500 5800 50  0001 C CNN
F 1 "+5V" V 1515 6078 50  0000 L CNN
F 2 "" H 1500 5950 50  0001 C CNN
F 3 "" H 1500 5950 50  0001 C CNN
	1    1500 5950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1500 5950 1650 5950
Connection ~ 2300 6300
Wire Wire Line
	2300 6300 2300 6400
Connection ~ 1650 5950
$Comp
L power:+3.3V #PWR0112
U 1 1 5F621918
P 3000 5950
F 0 "#PWR0112" H 3000 5800 50  0001 C CNN
F 1 "+3.3V" V 3015 6078 50  0000 L CNN
F 2 "" H 3000 5950 50  0001 C CNN
F 3 "" H 3000 5950 50  0001 C CNN
	1    3000 5950
	0    1    1    0   
$EndComp
Wire Wire Line
	1650 6300 2300 6300
Wire Wire Line
	1650 6000 1650 5950
Wire Wire Line
	1650 5950 1950 5950
Wire Wire Line
	3000 5950 2800 5950
Wire Wire Line
	2300 6250 2300 6300
$Comp
L Device:C C2
U 1 1 5F620133
P 1650 6150
F 0 "C2" H 1765 6196 50  0000 L CNN
F 1 "1uF" H 1765 6105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 1688 6000 50  0001 C CNN
F 3 "~" H 1650 6150 50  0001 C CNN
	1    1650 6150
	1    0    0    -1  
$EndComp
$Comp
L USB_Screen_Library:SC662 U3
U 1 1 5F61E515
P 2300 6000
F 0 "U3" H 2300 6200 50  0000 C CNN
F 1 "SC662" H 2450 5800 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 2300 6150 50  0001 C CNN
F 3 "" H 2300 6150 50  0001 C CNN
	1    2300 6000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 5F6242DA
P 2800 6150
F 0 "C4" H 2915 6196 50  0000 L CNN
F 1 "1uF" H 2915 6105 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2838 6000 50  0001 C CNN
F 3 "~" H 2800 6150 50  0001 C CNN
	1    2800 6150
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 6000 2800 5950
Connection ~ 2800 5950
Wire Wire Line
	2800 5950 2650 5950
Wire Wire Line
	2800 6300 2300 6300
$Comp
L USB_Screen_Library:ZJY154T-PG04 U1
U 1 1 5F62CDD2
P 1650 4250
F 0 "U1" H 1750 3550 50  0000 L CNN
F 1 "ZJY154T-PG04" H 1400 4900 50  0000 L CNN
F 2 "USB-Screen:ZJY154T-PG04" H 1550 4900 50  0001 C CNN
F 3 "" H 1550 4900 50  0001 C CNN
	1    1650 4250
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR04
U 1 1 5F62DA43
P 2550 4750
F 0 "#PWR04" H 2550 4500 50  0001 C CNN
F 1 "GND" V 2555 4622 50  0000 R CNN
F 2 "" H 2550 4750 50  0001 C CNN
F 3 "" H 2550 4750 50  0001 C CNN
	1    2550 4750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2550 4750 2500 4750
$Comp
L power:GND #PWR02
U 1 1 5F62FDF8
P 2100 4300
F 0 "#PWR02" H 2100 4050 50  0001 C CNN
F 1 "GND" V 2105 4172 50  0000 R CNN
F 2 "" H 2100 4300 50  0001 C CNN
F 3 "" H 2100 4300 50  0001 C CNN
	1    2100 4300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1900 4350 2100 4350
Wire Wire Line
	2100 4350 2100 4300
Wire Wire Line
	1900 4250 2100 4250
Wire Wire Line
	2100 4250 2100 4300
Connection ~ 2100 4300
$Comp
L power:GND #PWR01
U 1 1 5F6311BD
P 2050 3650
F 0 "#PWR01" H 2050 3400 50  0001 C CNN
F 1 "GND" H 2055 3477 50  0000 C CNN
F 2 "" H 2050 3650 50  0001 C CNN
F 3 "" H 2050 3650 50  0001 C CNN
	1    2050 3650
	-1   0    0    1   
$EndComp
Wire Wire Line
	1900 3650 2050 3650
$Comp
L power:+3.3V #PWR03
U 1 1 5F631EFC
P 2550 4450
F 0 "#PWR03" H 2550 4300 50  0001 C CNN
F 1 "+3.3V" V 2565 4578 50  0000 L CNN
F 2 "" H 2550 4450 50  0001 C CNN
F 3 "" H 2550 4450 50  0001 C CNN
	1    2550 4450
	0    1    1    0   
$EndComp
Wire Wire Line
	2550 4450 2500 4450
$Comp
L Device:C C1
U 1 1 5F636325
P 2500 4600
F 0 "C1" H 2615 4646 50  0000 L CNN
F 1 "0.1uF" H 2615 4555 50  0000 L CNN
F 2 "Capacitor_SMD:C_0402_1005Metric" H 2538 4450 50  0001 C CNN
F 3 "~" H 2500 4600 50  0001 C CNN
	1    2500 4600
	-1   0    0    1   
$EndComp
Connection ~ 2500 4750
Wire Wire Line
	2500 4750 2150 4750
Connection ~ 2500 4450
Wire Wire Line
	2500 4450 1900 4450
Wire Wire Line
	2150 4650 1900 4650
Wire Wire Line
	2150 4550 1900 4550
Text Label 2150 4550 2    50   ~ 0
KEDA
Wire Wire Line
	2100 4150 1900 4150
Wire Wire Line
	2100 4050 1900 4050
Wire Wire Line
	2100 3950 1900 3950
Wire Wire Line
	2100 3850 1900 3850
Wire Wire Line
	2100 3750 1900 3750
Text Label 2100 4150 2    50   ~ 0
DC
Text Label 2100 4050 2    50   ~ 0
CS
Text Label 2100 3950 2    50   ~ 0
SCL
Text Label 2100 3850 2    50   ~ 0
SDA
Text Label 2100 3750 2    50   ~ 0
RST
Wire Wire Line
	6800 4000 7000 4000
Text Label 6800 4000 0    50   ~ 0
CS
Wire Wire Line
	8500 4400 8100 4400
Wire Wire Line
	8500 4500 8100 4500
Wire Wire Line
	8500 4600 8100 4600
Text Label 8500 4400 2    50   ~ 0
PWM
Wire Wire Line
	2150 4650 2150 4750
Connection ~ 2150 4750
Wire Wire Line
	2150 4750 1900 4750
Wire Wire Line
	6800 3900 7000 3900
Wire Wire Line
	6800 4500 7000 4500
Wire Wire Line
	6800 4600 7000 4600
Text Label 6800 3900 0    50   ~ 0
RST
Text Label 6800 4500 0    50   ~ 0
DC
Text Label 6800 4600 0    50   ~ 0
LED1
Text Label 8500 4500 2    50   ~ 0
LED2
Text Label 8500 4600 2    50   ~ 0
LED3
$Comp
L Device:LED D1
U 1 1 5F681B3F
P 4450 3800
F 0 "D1" H 4450 3900 50  0000 C CNN
F 1 "LED" H 4600 3750 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4450 3800 50  0001 C CNN
F 3 "~" H 4450 3800 50  0001 C CNN
	1    4450 3800
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D2
U 1 1 5F682560
P 4450 4050
F 0 "D2" H 4450 4150 50  0000 C CNN
F 1 "LED" H 4600 4000 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4450 4050 50  0001 C CNN
F 3 "~" H 4450 4050 50  0001 C CNN
	1    4450 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:LED D3
U 1 1 5F682D92
P 4450 4300
F 0 "D3" H 4450 4400 50  0000 C CNN
F 1 "LED" H 4600 4250 50  0000 C CNN
F 2 "LED_SMD:LED_0402_1005Metric" H 4450 4300 50  0001 C CNN
F 3 "~" H 4450 4300 50  0001 C CNN
	1    4450 4300
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 5F684B7B
P 4150 3800
F 0 "#PWR05" H 4150 3550 50  0001 C CNN
F 1 "GND" V 4155 3672 50  0000 R CNN
F 2 "" H 4150 3800 50  0001 C CNN
F 3 "" H 4150 3800 50  0001 C CNN
	1    4150 3800
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR06
U 1 1 5F685180
P 4150 4050
F 0 "#PWR06" H 4150 3800 50  0001 C CNN
F 1 "GND" V 4155 3922 50  0000 R CNN
F 2 "" H 4150 4050 50  0001 C CNN
F 3 "" H 4150 4050 50  0001 C CNN
	1    4150 4050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR07
U 1 1 5F6854D2
P 4150 4300
F 0 "#PWR07" H 4150 4050 50  0001 C CNN
F 1 "GND" V 4155 4172 50  0000 R CNN
F 2 "" H 4150 4300 50  0001 C CNN
F 3 "" H 4150 4300 50  0001 C CNN
	1    4150 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4150 4300 4300 4300
Wire Wire Line
	4150 4050 4300 4050
Wire Wire Line
	4150 3800 4300 3800
$Comp
L Device:R R3
U 1 1 5F68A7DE
P 4900 3800
F 0 "R3" V 4800 3800 50  0000 C CNN
F 1 "33R" V 4900 3800 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4830 3800 50  0001 C CNN
F 3 "~" H 4900 3800 50  0001 C CNN
	1    4900 3800
	0    1    1    0   
$EndComp
$Comp
L Device:R R4
U 1 1 5F68B5C4
P 4900 4050
F 0 "R4" V 4800 4050 50  0000 C CNN
F 1 "33R" V 4900 4050 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4830 4050 50  0001 C CNN
F 3 "~" H 4900 4050 50  0001 C CNN
	1    4900 4050
	0    1    1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 5F68CF79
P 4900 4300
F 0 "R5" V 4800 4300 50  0000 C CNN
F 1 "33R" V 4900 4300 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4830 4300 50  0001 C CNN
F 3 "~" H 4900 4300 50  0001 C CNN
	1    4900 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4600 3800 4750 3800
Wire Wire Line
	4600 4050 4750 4050
Wire Wire Line
	4600 4300 4750 4300
Wire Wire Line
	5050 3800 5300 3800
Wire Wire Line
	5050 4050 5300 4050
Wire Wire Line
	5050 4300 5300 4300
Text Label 5300 3800 2    50   ~ 0
LED1
Text Label 5300 4050 2    50   ~ 0
LED2
Text Label 5300 4300 2    50   ~ 0
LED3
NoConn ~ 3900 2050
NoConn ~ 7000 4200
NoConn ~ 7000 4400
Text Label 5400 1750 2    50   ~ 0
USB_D+
Text Label 5400 1850 2    50   ~ 0
USB_D-
$Comp
L power:GND #PWR0106
U 1 1 5F618012
P 5400 1650
F 0 "#PWR0106" H 5400 1400 50  0001 C CNN
F 1 "GND" H 5405 1477 50  0000 C CNN
F 2 "" H 5400 1650 50  0001 C CNN
F 3 "" H 5400 1650 50  0001 C CNN
	1    5400 1650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 1650 5100 1650
Wire Wire Line
	5400 1750 5100 1750
Wire Wire Line
	5400 1850 5100 1850
Text Label 4300 1950 2    50   ~ 0
USB_D-
Text Label 4300 1850 2    50   ~ 0
USB_D+
Wire Wire Line
	3900 1950 4300 1950
Wire Wire Line
	3900 1850 4300 1850
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 5F605182
P 4900 1850
F 0 "J2" H 4900 1550 50  0000 C CNN
F 1 "Conn_01x04" H 4800 2100 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Horizontal" H 4900 1850 50  0001 C CNN
F 3 "~" H 4900 1850 50  0001 C CNN
	1    4900 1850
	-1   0    0    1   
$EndComp
Wire Wire Line
	5400 1950 5100 1950
$Comp
L power:+5V #PWR0105
U 1 1 5F617214
P 5400 1950
F 0 "#PWR0105" H 5400 1800 50  0001 C CNN
F 1 "+5V" V 5415 2078 50  0000 L CNN
F 2 "" H 5400 1950 50  0001 C CNN
F 3 "" H 5400 1950 50  0001 C CNN
	1    5400 1950
	0    1    1    0   
$EndComp
$Comp
L USB-Screen-rescue:2SC4213-Transistor_BJT Q1
U 1 1 5F727A10
P 5400 5600
F 0 "Q1" H 5591 5554 50  0000 L CNN
F 1 "NPN" H 5591 5645 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23" H 5600 5525 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=19305&prodName=2SC4213" H 5400 5600 50  0001 L CNN
	1    5400 5600
	-1   0    0    -1  
$EndComp
$Comp
L power:+5V #PWR0114
U 1 1 5F730CBA
P 5300 5200
F 0 "#PWR0114" H 5300 5050 50  0001 C CNN
F 1 "+5V" H 5315 5373 50  0000 C CNN
F 2 "" H 5300 5200 50  0001 C CNN
F 3 "" H 5300 5200 50  0001 C CNN
	1    5300 5200
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5300 5200 5300 5400
Wire Wire Line
	5300 5950 5300 5800
Wire Wire Line
	5600 5600 5900 5600
Text Label 5900 5600 2    50   ~ 0
PWM
Text Label 4350 5950 0    50   ~ 0
KEDA
$Comp
L Device:R R6
U 1 1 5F74A98E
P 4900 5950
F 0 "R6" V 4800 5950 50  0000 C CNN
F 1 "33R" V 4900 5950 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 4830 5950 50  0001 C CNN
F 3 "~" H 4900 5950 50  0001 C CNN
	1    4900 5950
	0    -1   1    0   
$EndComp
Wire Wire Line
	4350 5950 4750 5950
Wire Wire Line
	5050 5950 5300 5950
$Comp
L Device:D_Schottky D5
U 1 1 5F848FB6
P 6550 4300
F 0 "D5" H 6400 4350 50  0000 C CNN
F 1 "1n4148" H 6500 4400 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" H 6550 4300 50  0001 C CNN
F 3 "~" H 6550 4300 50  0001 C CNN
	1    6550 4300
	-1   0    0    1   
$EndComp
$Comp
L Device:D_Schottky D4
U 1 1 5F84B388
P 6550 4100
F 0 "D4" H 6400 4150 50  0000 C CNN
F 1 "1n4148" H 6500 4200 50  0000 C CNN
F 2 "Diode_SMD:D_SOD-323" H 6550 4100 50  0001 C CNN
F 3 "~" H 6550 4100 50  0001 C CNN
	1    6550 4100
	-1   0    0    1   
$EndComp
Wire Wire Line
	6700 4100 7000 4100
Wire Wire Line
	7000 4300 6700 4300
Wire Wire Line
	6050 4100 6250 4100
Wire Wire Line
	6050 4300 6350 4300
Text Label 6050 4300 0    50   ~ 0
SCL
Text Label 6050 4100 0    50   ~ 0
SDA
$Comp
L Device:R R2
U 1 1 5F860967
P 6350 3850
F 0 "R2" V 6250 3850 50  0000 C CNN
F 1 "10K" V 6350 3850 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6280 3850 50  0001 C CNN
F 3 "~" H 6350 3850 50  0001 C CNN
	1    6350 3850
	-1   0    0    1   
$EndComp
$Comp
L Device:R R1
U 1 1 5F8654DD
P 6250 3850
F 0 "R1" V 6150 3850 50  0000 C CNN
F 1 "10K" V 6250 3850 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 6180 3850 50  0001 C CNN
F 3 "~" H 6250 3850 50  0001 C CNN
	1    6250 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 4000 6250 4100
Connection ~ 6250 4100
Wire Wire Line
	6250 4100 6400 4100
Wire Wire Line
	6350 4000 6350 4300
Connection ~ 6350 4300
Wire Wire Line
	6350 4300 6400 4300
$Comp
L power:+3.3V #PWR08
U 1 1 5F873B8C
P 6250 3650
F 0 "#PWR08" H 6250 3500 50  0001 C CNN
F 1 "+3.3V" H 6265 3823 50  0000 C CNN
F 2 "" H 6250 3650 50  0001 C CNN
F 3 "" H 6250 3650 50  0001 C CNN
	1    6250 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6250 3650 6250 3700
Wire Wire Line
	6250 3650 6350 3650
Wire Wire Line
	6350 3650 6350 3700
Connection ~ 6250 3650
$Comp
L Connector:USB_C_Receptacle_USB2.0 J3
U 1 1 5F8814AB
P 1750 1800
F 0 "J3" H 1857 2667 50  0000 C CNN
F 1 "USB_C_Receptacle_USB2.0" H 1857 2576 50  0000 C CNN
F 2 "Connector_USB:USB_C_Receptacle_Palconn_UTC16-G" H 1900 1800 50  0001 C CNN
F 3 "https://www.usb.org/sites/default/files/documents/usb_type-c.zip" H 1900 1800 50  0001 C CNN
	1    1750 1800
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR011
U 1 1 5F883ED9
P 2350 1200
F 0 "#PWR011" H 2350 1050 50  0001 C CNN
F 1 "+5V" V 2365 1328 50  0000 L CNN
F 2 "" H 2350 1200 50  0001 C CNN
F 3 "" H 2350 1200 50  0001 C CNN
	1    2350 1200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 5F885F51
P 1750 2700
F 0 "#PWR010" H 1750 2450 50  0001 C CNN
F 1 "GND" H 1755 2527 50  0000 C CNN
F 2 "" H 1750 2700 50  0001 C CNN
F 3 "" H 1750 2700 50  0001 C CNN
	1    1750 2700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 5F8869F5
P 1450 2700
F 0 "#PWR09" H 1450 2450 50  0001 C CNN
F 1 "GND" H 1455 2527 50  0000 C CNN
F 2 "" H 1450 2700 50  0001 C CNN
F 3 "" H 1450 2700 50  0001 C CNN
	1    1450 2700
	1    0    0    -1  
$EndComp
NoConn ~ 2350 2300
NoConn ~ 2350 2400
Wire Wire Line
	2350 1700 2350 1800
Connection ~ 2350 1800
Wire Wire Line
	2350 1900 2350 2000
Connection ~ 2350 2000
Wire Wire Line
	2350 2000 2750 2000
Wire Wire Line
	2350 1800 2750 1800
Text Label 2750 2000 2    50   ~ 0
USB_D+
Text Label 2750 1800 2    50   ~ 0
USB_D-
$Comp
L Device:R R7
U 1 1 5F878E06
P 2650 1400
F 0 "R7" V 2550 1400 50  0000 C CNN
F 1 "5.1K" V 2650 1400 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2580 1400 50  0001 C CNN
F 3 "~" H 2650 1400 50  0001 C CNN
	1    2650 1400
	0    1    1    0   
$EndComp
$Comp
L Device:R R8
U 1 1 5F88C5EE
P 2650 1500
F 0 "R8" V 2550 1500 50  0000 C CNN
F 1 "5.1K" V 2650 1500 50  0000 C CNN
F 2 "Resistor_SMD:R_0402_1005Metric" V 2580 1500 50  0001 C CNN
F 3 "~" H 2650 1500 50  0001 C CNN
	1    2650 1500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2350 1400 2500 1400
Wire Wire Line
	2350 1500 2500 1500
Wire Wire Line
	2800 1400 2800 1500
$Comp
L power:GND #PWR012
U 1 1 5F898A63
P 2800 1500
F 0 "#PWR012" H 2800 1250 50  0001 C CNN
F 1 "GND" H 2805 1327 50  0000 C CNN
F 2 "" H 2800 1500 50  0001 C CNN
F 3 "" H 2800 1500 50  0001 C CNN
	1    2800 1500
	1    0    0    -1  
$EndComp
Connection ~ 2800 1500
$EndSCHEMATC
