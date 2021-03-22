EESchema Schematic File Version 4
LIBS:SNES Jukebox-cache
EELAYER 29 0
EELAYER END
$Descr User 12051 8268
encoding utf-8
Sheet 1 1
Title "SNES Jukebox (Mega Version)"
Date "2021-03-22"
Rev "2.0"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	8650 4050 8850 4050
$Comp
L SNES_Jukebox-eagle-import:R-US_0207_10 R3
U 1 1 A04F324A
P 8050 4350
F 0 "R3" H 7900 4409 59  0000 L BNN
F 1 "47" H 7900 4220 59  0000 L BNN
F 2 "SNES Jukebox:0207_10" H 8050 4350 50  0001 C CNN
F 3 "" H 8050 4350 50  0001 C CNN
	1    8050 4350
	1    0    0    -1  
$EndComp
$Comp
L SNES_Jukebox-eagle-import:R-US_0207_10 R4
U 1 1 BADBB996
P 8400 4600
F 0 "R4" H 8250 4659 59  0000 L BNN
F 1 "47" H 8250 4470 59  0000 L BNN
F 2 "SNES Jukebox:0207_10" H 8400 4600 50  0001 C CNN
F 3 "" H 8400 4600 50  0001 C CNN
	1    8400 4600
	0    -1   -1   0   
$EndComp
$Comp
L SNES_Jukebox-eagle-import:PINHD-1X10 J4
U 1 1 6B60D7D7
P 3850 6450
F 0 "J4" H 3600 6975 59  0000 L BNN
F 1 "Adafruit 358" H 3600 5750 59  0000 L BNN
F 2 "SNES Jukebox:1X10" H 3850 6450 50  0001 C CNN
F 3 "" H 3850 6450 50  0001 C CNN
	1    3850 6450
	0    1    1    0   
$EndComp
$Comp
L SNES_Jukebox-eagle-import:PINHD-2X12 J6
U 1 1 D61E78B2
P 7250 2900
F 0 "J6" H 7000 3525 59  0000 L BNN
F 1 "SPC-700" H 7000 2100 59  0000 L BNN
F 2 "SNES Jukebox:2X12" H 7250 2900 50  0001 C CNN
F 3 "" H 7250 2900 50  0001 C CNN
	1    7250 2900
	1    0    0    -1  
$EndComp
$Comp
L SNES_Jukebox-eagle-import:1503_02 X2
U 1 1 9186E423
P 9050 4150
F 0 "X2" H 8950 4310 59  0000 L BNN
F 1 "Audio Out" H 8950 3800 59  0000 L BNN
F 2 "SNES Jukebox:1503_02" H 9050 4150 50  0001 C CNN
F 3 "" H 9050 4150 50  0001 C CNN
	1    9050 4150
	1    0    0    -1  
$EndComp
$Comp
L SNES_Jukebox-eagle-import:PINHD-1X7 J5
U 1 1 BAF77653
P 6300 6050
F 0 "J5" H 6050 6475 59  0000 L BNN
F 1 "Controller" H 6050 5550 59  0000 L BNN
F 2 "SNES Jukebox:1X07" H 6300 6050 50  0001 C CNN
F 3 "" H 6300 6050 50  0001 C CNN
	1    6300 6050
	0    1    1    0   
$EndComp
Text Notes 4800 6909 2    59   ~ 0
Pin 1 considered leftmost when viewing screen
Text Notes 7000 6459 2    59   ~ 0
Pin 1 considered square end
Text Notes 8100 4900 0    59   ~ 0
Right
Text Notes 7350 4350 0    59   ~ 0
Left
Text Notes 450  650  0    59   ~ 0
SNES Jukebox Hardware Design 2.0\nOriginal Design Copyright (C) 2013-2021 Moonspine\nYou are free to use, distribute, or modify this design in any way.
Wire Wire Line
	8250 4350 8850 4350
Wire Wire Line
	8400 4250 8850 4250
Wire Wire Line
	8400 4400 8400 4250
Wire Wire Line
	6000 5950 6000 5500
Wire Wire Line
	8650 4050 8650 4450
$Comp
L Connector_Generic:Conn_02x18_Odd_Even J1
U 1 1 605AE6A8
P 4900 4600
F 0 "J1" V 4904 3613 50  0000 R CNN
F 1 "Arduino Mega Header" V 4995 3613 50  0000 R CNN
F 2 "" H 4900 4600 50  0001 C CNN
F 3 "~" H 4900 4600 50  0001 C CNN
	1    4900 4600
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x08_Male J2
U 1 1 605B85EB
P 3350 3450
F 0 "J2" H 3458 3931 50  0000 C CNN
F 1 "Arduino Mega PK (Analog 8-15)" H 3458 3840 50  0000 C CNN
F 2 "" H 3350 3450 50  0001 C CNN
F 3 "~" H 3350 3450 50  0001 C CNN
	1    3350 3450
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x08_Male J3
U 1 1 605C0044
P 3350 2450
F 0 "J3" H 3458 2931 50  0000 C CNN
F 1 "Arduino Mega PF (Analog 0-7)" H 3458 2840 50  0000 C CNN
F 2 "" H 3350 2450 50  0001 C CNN
F 3 "~" H 3350 2450 50  0001 C CNN
	1    3350 2450
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 605C7A12
P 5700 4150
F 0 "#PWR?" H 5700 4000 50  0001 C CNN
F 1 "+5V" H 5715 4323 50  0000 C CNN
F 2 "" H 5700 4150 50  0001 C CNN
F 3 "" H 5700 4150 50  0001 C CNN
	1    5700 4150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 605C820C
P 6600 5500
F 0 "#PWR?" H 6600 5350 50  0001 C CNN
F 1 "+5V" H 6615 5673 50  0000 C CNN
F 2 "" H 6600 5500 50  0001 C CNN
F 3 "" H 6600 5500 50  0001 C CNN
	1    6600 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4900 6700 4900
Text Label 5950 4900 0    50   ~ 0
D23
Wire Wire Line
	5600 3100 7150 3100
Text Label 5600 4150 1    50   ~ 0
D22
$Comp
L power:+5V #PWR?
U 1 1 605CC728
P 8700 2200
F 0 "#PWR?" H 8700 2050 50  0001 C CNN
F 1 "+5V" H 8715 2373 50  0000 C CNN
F 2 "" H 8700 2200 50  0001 C CNN
F 3 "" H 8700 2200 50  0001 C CNN
	1    8700 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 2400 8700 2200
Wire Wire Line
	8700 3200 8700 2400
Connection ~ 8700 2400
Wire Wire Line
	8700 3500 8700 3200
Connection ~ 8700 3200
$Comp
L power:GND #PWR?
U 1 1 605CDEF3
P 6850 3750
F 0 "#PWR?" H 6850 3500 50  0001 C CNN
F 1 "GND" H 6855 3577 50  0000 C CNN
F 2 "" H 6850 3750 50  0001 C CNN
F 3 "" H 6850 3750 50  0001 C CNN
	1    6850 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2400 7150 2400
Wire Wire Line
	6850 3300 7150 3300
Wire Wire Line
	6850 3300 6850 3500
Wire Wire Line
	7150 3500 6850 3500
Connection ~ 6850 3500
Wire Wire Line
	6850 3500 6850 3750
$Comp
L power:GND #PWR?
U 1 1 605D20FF
P 6000 5500
F 0 "#PWR?" H 6000 5250 50  0001 C CNN
F 1 "GND" H 6005 5327 50  0000 C CNN
F 2 "" H 6000 5500 50  0001 C CNN
F 3 "" H 6000 5500 50  0001 C CNN
	1    6000 5500
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 605D253C
P 8650 4450
F 0 "#PWR?" H 8650 4200 50  0001 C CNN
F 1 "GND" H 8655 4277 50  0000 C CNN
F 2 "" H 8650 4450 50  0001 C CNN
F 3 "" H 8650 4450 50  0001 C CNN
	1    8650 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3400 7150 3400
Wire Wire Line
	6950 3400 6950 4350
Wire Wire Line
	6950 4350 7850 4350
Wire Wire Line
	7450 3400 7650 3400
Wire Wire Line
	7650 3400 7650 4800
Wire Wire Line
	7650 4800 8400 4800
Wire Wire Line
	6600 2500 7150 2500
Wire Wire Line
	7650 2250 7650 2500
Wire Wire Line
	7650 2500 7450 2500
Wire Wire Line
	3550 2350 6550 2350
Wire Wire Line
	6550 2350 6550 2600
Wire Wire Line
	6550 2600 7150 2600
Wire Wire Line
	3550 2450 6500 2450
Wire Wire Line
	7700 2100 7700 2600
Wire Wire Line
	7700 2600 7450 2600
NoConn ~ 3550 2550
NoConn ~ 3550 2650
NoConn ~ 3550 2750
NoConn ~ 3550 2850
Wire Wire Line
	5100 3150 5100 2700
Wire Wire Line
	5100 2700 7150 2700
Wire Wire Line
	3550 3150 5100 3150
Wire Wire Line
	3550 3350 5200 3350
Wire Wire Line
	5200 3350 5200 2800
Wire Wire Line
	5200 2800 7150 2800
Wire Wire Line
	3550 3550 5300 3550
Wire Wire Line
	5300 3550 5300 2900
Wire Wire Line
	5300 2900 7150 2900
Wire Wire Line
	3550 3750 5400 3750
Wire Wire Line
	5400 3750 5400 3000
Wire Wire Line
	5400 3000 7150 3000
Wire Wire Line
	7450 2400 8700 2400
Wire Wire Line
	7450 3200 8700 3200
Wire Wire Line
	7450 3500 8700 3500
Wire Wire Line
	3550 3250 4300 3250
Wire Wire Line
	4300 1350 7800 1350
Wire Wire Line
	7800 2700 7450 2700
Wire Wire Line
	3550 3450 4400 3450
Wire Wire Line
	4400 1450 7900 1450
Wire Wire Line
	7900 2800 7450 2800
Wire Wire Line
	3550 3650 4500 3650
Wire Wire Line
	4500 1550 8000 1550
Wire Wire Line
	8000 2900 7450 2900
Wire Wire Line
	3550 3850 4600 3850
Wire Wire Line
	4600 1650 8100 1650
Wire Wire Line
	8100 3000 7450 3000
Wire Wire Line
	7800 1350 7800 2700
Wire Wire Line
	7900 1450 7900 2800
Wire Wire Line
	8000 1550 8000 2900
Wire Wire Line
	8100 1650 8100 3000
Wire Wire Line
	4300 1350 4300 3250
Wire Wire Line
	4400 1450 4400 3450
Wire Wire Line
	4500 1550 4500 3650
Wire Wire Line
	4600 1650 4600 3850
NoConn ~ 7450 3100
NoConn ~ 7150 3200
NoConn ~ 7450 3300
Wire Wire Line
	6600 5950 6600 5500
Text Label 6050 5100 0    50   ~ 0
D41
Wire Wire Line
	6500 5950 6500 5100
Wire Wire Line
	6500 5100 4700 5100
Wire Wire Line
	6400 5950 6400 4300
Wire Wire Line
	6400 4300 4700 4300
Wire Wire Line
	4700 4300 4700 4400
Text Label 4850 4300 2    50   ~ 0
D40
Wire Wire Line
	4700 4900 4700 5100
Wire Wire Line
	6300 5950 6300 5000
Wire Wire Line
	6300 5000 4800 5000
Wire Wire Line
	4800 5000 4800 4900
Text Label 6200 5000 2    50   ~ 0
D39
NoConn ~ 6200 5950
NoConn ~ 6100 5950
$Comp
L power:GND #PWR?
U 1 1 60690D23
P 4000 4900
F 0 "#PWR?" H 4000 4650 50  0001 C CNN
F 1 "GND" H 4005 4727 50  0000 C CNN
F 2 "" H 4000 4900 50  0001 C CNN
F 3 "" H 4000 4900 50  0001 C CNN
	1    4000 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 4150 5700 4400
$Comp
L power:GND #PWR?
U 1 1 606960D2
P 2850 6150
F 0 "#PWR?" H 2850 5900 50  0001 C CNN
F 1 "GND" H 2855 5977 50  0000 C CNN
F 2 "" H 2850 6150 50  0001 C CNN
F 3 "" H 2850 6150 50  0001 C CNN
	1    2850 6150
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 60696C34
P 2850 6050
F 0 "#PWR?" H 2850 5900 50  0001 C CNN
F 1 "+5V" H 2865 6223 50  0000 C CNN
F 2 "" H 2850 6050 50  0001 C CNN
F 3 "" H 2850 6050 50  0001 C CNN
	1    2850 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 6150 3350 6150
Wire Wire Line
	2850 6050 3450 6050
Wire Wire Line
	3450 5550 4250 5550
Wire Wire Line
	4400 4900 4400 5150
Wire Wire Line
	4400 5150 3550 5150
Text Label 3600 5150 2    50   ~ 0
D47
Wire Wire Line
	4300 4900 4300 5250
Wire Wire Line
	4300 5250 3850 5250
Text Label 3900 5250 2    50   ~ 0
D49
Wire Wire Line
	4200 5450 3950 5450
Text Label 4150 5450 2    50   ~ 0
D51
Text Label 3900 5350 0    50   ~ 0
D53
Wire Wire Line
	4200 4900 4200 5450
Wire Wire Line
	4100 4900 4100 5350
Wire Wire Line
	4100 5350 3750 5350
Wire Wire Line
	4300 4100 3650 4100
Text Label 3700 4100 2    50   ~ 0
D48
Wire Wire Line
	4300 4100 4300 4400
Wire Wire Line
	3550 5150 3550 6350
Wire Wire Line
	3650 4100 3650 6350
Wire Wire Line
	3750 5350 3750 6350
Wire Wire Line
	3850 5250 3850 6350
Wire Wire Line
	3950 5450 3950 6350
Wire Wire Line
	4250 5550 4250 6350
Wire Wire Line
	3350 6150 3350 6350
Connection ~ 3450 6050
Wire Wire Line
	3450 6050 3450 6350
Wire Wire Line
	3450 5550 3450 6050
Wire Wire Line
	4200 4400 4200 4200
Wire Wire Line
	4200 4200 3350 4200
Wire Wire Line
	3350 4200 3350 5700
Wire Wire Line
	3350 5700 4150 5700
Wire Wire Line
	4150 5700 4150 6350
Wire Wire Line
	4100 4400 4100 4300
Wire Wire Line
	4100 4300 3250 4300
Wire Wire Line
	3250 4300 3250 5800
Wire Wire Line
	3250 5800 4050 5800
Wire Wire Line
	4050 5800 4050 6350
Text Label 3400 4200 2    50   ~ 0
D50
Text Label 3300 4300 2    50   ~ 0
D52
NoConn ~ 5600 4900
NoConn ~ 5500 4900
NoConn ~ 5400 4900
NoConn ~ 5300 4900
NoConn ~ 5200 4900
NoConn ~ 5100 4900
NoConn ~ 5000 4900
NoConn ~ 4900 4900
NoConn ~ 4600 4900
NoConn ~ 4500 4900
NoConn ~ 4000 4400
NoConn ~ 4400 4400
NoConn ~ 4500 4400
NoConn ~ 4600 4400
NoConn ~ 4800 4400
NoConn ~ 4900 4400
NoConn ~ 5000 4400
NoConn ~ 5100 4400
NoConn ~ 5200 4400
NoConn ~ 5300 4400
NoConn ~ 5400 4400
NoConn ~ 5500 4400
Wire Wire Line
	5600 3100 5600 4400
Wire Wire Line
	6700 2400 6700 4900
Wire Wire Line
	6500 2100 7700 2100
Wire Wire Line
	6500 2450 6500 2100
Wire Wire Line
	3550 2250 7650 2250
Wire Wire Line
	6600 2150 6600 2500
Wire Wire Line
	3550 2150 6600 2150
$EndSCHEMATC
