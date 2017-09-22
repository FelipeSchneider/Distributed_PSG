EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:lm2665
LIBS:Placa_Acelerometro-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date "27 jul 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ADXL335 U1
U 1 1 56E167E1
P 6725 3250
F 0 "U1" H 7075 3650 60  0000 C CNN
F 1 "ADXL335" H 7125 2850 60  0000 C CNN
F 2 "Placa_principal_ADXL:ADXL335_correto" H 6325 3200 60  0001 C CNN
F 3 "" H 6325 3200 60  0000 C CNN
	1    6725 3250
	1    0    0    -1  
$EndComp
Text Label 7450 3100 0    60   ~ 0
ADC_A1
Text Label 6725 2450 0    60   ~ 0
ADCA_REF
NoConn ~ 6875 2700
NoConn ~ 6575 2700
NoConn ~ 6125 3100
NoConn ~ 6125 3400
NoConn ~ 7325 3200
NoConn ~ 7325 3400
$Comp
L GND-RESCUE-Placa_Xmega128 #PWR01
U 1 1 56E167EB
P 6675 3975
F 0 "#PWR01" H 6675 3975 30  0001 C CNN
F 1 "GND" H 6675 3905 30  0001 C CNN
F 2 "" H 6675 3975 60  0000 C CNN
F 3 "" H 6675 3975 60  0000 C CNN
	1    6675 3975
	1    0    0    -1  
$EndComp
$Comp
L GND-RESCUE-Placa_Xmega128 #PWR02
U 1 1 56E167EC
P 6000 3400
F 0 "#PWR02" H 6000 3400 30  0001 C CNN
F 1 "GND" H 6000 3330 30  0001 C CNN
F 2 "" H 6000 3400 60  0000 C CNN
F 3 "" H 6000 3400 60  0000 C CNN
	1    6000 3400
	1    0    0    -1  
$EndComp
Text Label 7450 3300 0    60   ~ 0
ADC_A2
Text Label 7000 3875 0    60   ~ 0
ADC_A3
NoConn ~ 6125 3200
Wire Wire Line
	6725 2450 6725 2575
Wire Wire Line
	6675 2575 6775 2575
Wire Wire Line
	6775 2575 6775 2700
Wire Wire Line
	6675 2700 6675 2575
Connection ~ 6725 2575
Wire Wire Line
	6675 3800 6675 3975
Wire Wire Line
	6775 3900 6775 3800
Wire Wire Line
	6575 3900 6775 3900
Connection ~ 6675 3900
Wire Wire Line
	6575 3800 6575 3900
Wire Wire Line
	6000 3300 6125 3300
Wire Wire Line
	7450 3100 7325 3100
Wire Wire Line
	7450 3300 7325 3300
Wire Wire Line
	7000 3875 6875 3875
Wire Wire Line
	6875 3875 6875 3800
Wire Wire Line
	6000 3300 6000 3400
$Comp
L CONN_01X05 P1
U 1 1 56E1699A
P 4750 3200
F 0 "P1" H 4750 3500 50  0000 C CNN
F 1 "CONN_01X05" V 4850 3200 50  0000 C CNN
F 2 "Placa_principal:5-pin-50mil" H 4750 3200 50  0000 C CNN
F 3 "" H 4750 3200 50  0000 C CNN
	1    4750 3200
	-1   0    0    1   
$EndComp
Text Label 5050 3400 0    60   ~ 0
ADCA_REF
Wire Wire Line
	5050 3400 4950 3400
$Comp
L GND-RESCUE-Placa_Xmega128 #PWR03
U 1 1 56E16B24
P 5000 3325
F 0 "#PWR03" H 5000 3325 30  0001 C CNN
F 1 "GND" H 5000 3255 30  0001 C CNN
F 2 "" H 5000 3325 60  0000 C CNN
F 3 "" H 5000 3325 60  0000 C CNN
	1    5000 3325
	1    0    0    -1  
$EndComp
Wire Wire Line
	5000 3325 5000 3300
Wire Wire Line
	5000 3300 4950 3300
Text Label 5050 3200 0    60   ~ 0
ADC_A1
Text Label 5050 3100 0    60   ~ 0
ADC_A2
Text Label 5050 3000 0    60   ~ 0
ADC_A3
Wire Wire Line
	5050 3000 4950 3000
Wire Wire Line
	5050 3100 4950 3100
Wire Wire Line
	5050 3200 4950 3200
$EndSCHEMATC
