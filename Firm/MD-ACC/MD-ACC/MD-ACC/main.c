/*
 * MD-ACC.c
 *
 * Created: 11/06/2016 17:55:29
 * Author : Felipe
 */ 


#include "defines_global.h"
#include "Variables.h"
#include "uSD_SPI.h"
#include "ff.h"
#include "Initial_Set.h"
#include "AD_conv.h"
#include "data_SD.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

uint8_t g_buffer_uSD[BUFFER_SIZE], g_buffer_uSD_2[BUFFER_SIZE];	//buffers that will save data into µSD
uint8_t gf_buffer_indicator;		//indicates which buffer is active, if 1 we are using the g_buffer_uSD
uint8_t gf_start_transmission;	//indicates that we must start a transmission to µSD

uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
uint8_t gc_bat;			//counts how many samples of ADC battery values happened and were saved

uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
uint8_t gc_acc;			//counts how many samples of accelerometer values happened and were saved

uint8_t g_error;

uint8_t g_patient_number;	//number of the patient, goes up to 9 and depends of the pre-existing file

int main(void)
{
   	static uint16_t gc_hour = 0;	//this counter will determine the moment when one hour is reached, then a new file will be created - counting until 3600
   	IniPllClock();
   	IniLeds();
   	IniSincPin();
//	PORTC_OUTSET = LED2;
   	_delay_ms(200);
   	ConfigSpi_C();		//µSD SPI
   	IniAdConversor();
	   
   	g_buffer_uSD[0] = HEADER_1;
   	g_buffer_uSD[1] = HEADER_2;
   	g_buffer_uSD_2[0] = HEADER_1;
   	g_buffer_uSD_2[1] = HEADER_2;  
	g_error = OpenSD();
/*	if (g_error == 0)	PORTC_OUTSET = LED2;*/
	g_error = StartRecording();
/*	if (g_error == 0)	PORTC_OUTSET = LED3;*/
	
	uint8_t time_init = 0;
	while (time_init < 20){
		if((PORTD_IN & SINC_PIN) == 0){
			time_init = 0;
		} 
		else{
			time_init++;
		}
		_delay_ms(100);
	}
	PORTD_PIN0CTRL = 0;	//remove the pull down to save power
	PORTC_OUTSET = LED2; 
	ini_timer_40hz();
	on_timer_40hz();
	EnableIntLevels();
	sei();
//	PORTC_OUTCLR = LED2;
	uint8_t c_min = 0;
	static uint8_t c_hour_name = 1;
    while (1) 
    {
		 PORTE_OUTCLR = PIN0_bm;		//do nothing, just make sure that it does not skip this part because of optimizations
		 
		 if (gf_start_transmission){	//if its time to start the transmission:
			 gf_start_transmission = 0;
			 _delay_ms(1);	//just to make sure that all buffers are ready
			 PORTC_OUTCLR = LED2 | LED3;
			 c_min++;
			 if(c_min>59){
				 c_min = 0;
				 PORTC_OUTSET = LED3;
			 }
			 gc_hour++;
			 MakeLog();
			 if(gc_hour >= 3600){
				 PORTE_OUTSET = PIN6_bm;
				 c_hour_name++;
				 MakeNewFile(c_hour_name);
				 gc_hour = 0;
			 }
			 
		 }//close if gf_start_transmission
		 
    }
}

ISR(TCC1_OVF_vect){		//40Hz, time to sample the accelerometers
	static uint8_t f_zerar_buffers = 0;
	static uint8_t c_acc_local = 0;
	static uint8_t c_bat_local = 0;
//	PORTC_OUTTGL = LED3;
	if (f_zerar_buffers){	//this makes sure that all conversions will be finished before the buffers are changed
		//and also that the buffers are changed before the 401th sample
		f_zerar_buffers = 0;
		gf_start_transmission = 1;
		gf_buffer_indicator = gf_buffer_indicator^0x01;	//change the buffer indicator
		CleanGlobalCounters();
	}
	
	c_acc_local++;
	c_bat_local++;
	
	ADCA_CH1_CTRL |= ADC_CH_START_bm; // ADC ZOUT
	
	if (c_bat_local >= 4)	{
		ADCA_CH0_CTRL |= ADC_CH_START_bm; // ADC BATTERY
		c_bat_local = 0;
	}
	
	if (c_acc_local >= 40)	{
		f_zerar_buffers = 1;
		c_acc_local = 0;
	}
}

