/*
 * MD-ECG.c
 *
 * Created: 03/06/2016 11:16:26
 * Author : Felipe Schneider
 */ 

#include "defines_global.h"
#include "Variables.h"
#include "ADS.h"
#include "uSD_SPI.h"
#include "ff.h"
#include "Initial_Set.h"
#include "AD_conv.h"
#include "data_SD.h"

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <math.h>

uint8_t g_buffer_uSD[BUFFER_SIZE], g_buffer_uSD_2[BUFFER_SIZE];	//buffers that will save data into µSD
uint8_t gf_buffer_indicator;		//indicates which buffer is active, if 1 we are using the g_buffer_uSD
uint8_t gf_start_transmission;	//indicates that we must start a transmission to µSD


uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
uint8_t gc_bat;			//counts how many samples of ADC battery values happened and were saved

uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
uint8_t gc_acc;			//counts how many samples of accelerometer values happened and were saved
uint16_t g_acc_x_2, g_acc_y_2, g_acc_z_2; //accelerometer results
uint8_t gc_acc_2;			//counts how many samples of accelerometer values happened and were saved
uint16_t g_acc_x_3, g_acc_y_3, g_acc_z_3; //accelerometer results
uint8_t gc_acc_3;			//counts how many samples of accelerometer values happened and were saved
uint8_t f_select_acc=0;

uint8_t g_ads_ecg_1, g_ads_ecg_2, g_ads_ecg_3; //bytes from ADS, 1 is the most significant
uint16_t gc_ecg;			//counts how many samples of ADS EOG values happened and were saved

uint8_t g_error;

uint8_t g_patient_number = 1;	//number of the patient, goes up to 9 and depends of the pre-existing file
int main(void)
{
	static uint16_t gc_hour = 0;	//this counter will determine the moment when one hour is reached, then a new file will be created - counting until 3600
	IniRc32Clock();
	IniLeds();
	IniSincPin();
	_delay_ms(200);
	ConfigSpi_F();		//ADS SPI
	ConfigSpi_C();		//µSD SPI
	InitADS();			//setting the initial configurations for ADS
	IniAdConversor();
	g_buffer_uSD[0] = HEADER_1;
	g_buffer_uSD[1] = HEADER_2;
	g_buffer_uSD_2[0] = HEADER_1;
	g_buffer_uSD_2[1] = HEADER_2;
	
// 	uint8_t c_keep = 0;
// 	while (c_keep < 5){	//waits for the sinc. bot be pressed.
// 		if((PORTE_IN & PIN7_bm)==0){
// 			c_keep = 0;
// 		}
// 		else{
// 			c_keep++;
// 		}
// 		_delay_ms(100);
// 	}
//	PORTE_OUTSET = LED2;
	g_error = OpenSD();
	g_error = StartRecording();
	
	uint8_t time_init = 0;
	while (time_init < 20){
		if((PORTE_IN & SINC_PIN) == 0){
			time_init = 0;
		} 
		else{
			time_init++;
		}
		_delay_ms(100);
	}
	PORTE_PIN7CTRL = 0;	//remove the pull down to save power
	PORTE_OUTSET = LED2;
// 	ini_timer_400hz();
// 	on_timer_400hz();
 	EnableIntLevels();
	sei();
	
	//Enable the DRDYB interruption
	PORTF_DIRCLR = PIN2_bm;
	PORTF_PIN2CTRL = 0x1A;
	PORTF_INT0MASK = PIN2_bm;	
	PORTF_INTCTRL =  0x02;
	uint8_t c_min = 0;
	static uint8_t c_hour_name = 1;
    while(1)
    {
	    PORTK_OUTSET = PIN5_bm;		//do nothing, just make sure that it does not skip this part because of optimizations
	    
// 		if((PORTF_IN & PIN2_bm)==0){
// 			 PORTE_OUTSET = PIN5_bm;	//TODO:: for debug only
// 		}
// 		else{
// 			PORTE_OUTCLR = PIN5_bm;	//TODO:: for debug only
// 		}
// 			    
	    if (gf_start_transmission){	//if its time to start the transmission:
		    gf_start_transmission = 0;
		    _delay_ms(1);	//just to make sure that all buffers are ready
		    PORTE_OUTCLR = LED3 | LED2;
			c_min++;
			if(c_min>59){
				c_min = 0;
				PORTE_OUTSET = LED3;	//TODO:: for debug only
			}
		    gc_hour++;
			MakeLog();
		    if(gc_hour >= 3600){
			    PORTE_OUTSET = LED2;
			    c_hour_name++;
			    MakeNewFile(c_hour_name);
			    gc_hour = 0;
		    }
		    
	    }//close if gf_start_transmission
    }//close while 1
}
//TCF1_OVF_vect PORTF_INT0_vect
ISR(TCF1_OVF_vect){		//400HZ		EOG SOURCE TIMER (drift from 400Hz)
	
}

ISR(PORTF_INT0_vect){		//400HZ		EOG SOURCE DRDYB
	uint8_t ads_buffer[3];
	
	static uint8_t f_zerar_buffers = 0;
	static uint16_t c_ecg_local = 0;
	static uint8_t c_acc_local = 0;
	static uint8_t c_acc_2_local = 0;
	static uint8_t c_bat_local = 0;
	
	if (f_zerar_buffers){	//this makes sure that all conversions will be finished before the buffers are changed
		//and also that the buffers are changed before the 401th sample
		f_zerar_buffers = 0;
		gf_start_transmission = 1;
		gf_buffer_indicator = gf_buffer_indicator^0x01;	//change the buffer indicator
		CleanGlobalCounters();
	}
	
	c_ecg_local++;
	c_acc_local++;
	c_acc_2_local++;
	c_bat_local++;
	
	ChunkAcessADS(0x50, 3, &ads_buffer[0], ADS_READ);
	if (gf_buffer_indicator){
		g_buffer_uSD[ECG_OFFSET + gc_ecg] = ads_buffer[0];
		g_buffer_uSD[ECG_OFFSET + gc_ecg+1] = ads_buffer[1];
		g_buffer_uSD[ECG_OFFSET + gc_ecg+2] = ads_buffer[2];
	}
	else{
		g_buffer_uSD_2[ECG_OFFSET + gc_ecg] = ads_buffer[0];
		g_buffer_uSD_2[ECG_OFFSET + gc_ecg+1] = ads_buffer[1];
		g_buffer_uSD_2[ECG_OFFSET + gc_ecg+2] = ads_buffer[2];
	}
	gc_ecg +=3;
	
	if (c_acc_local == 10)	{
		ADCA_CH1_CTRL |= ADC_CH_START_bm; // ADC ZOUT
		//the end of the conversion on X channel will start the conversion on Y channel
		//and this end will start the conversion on Z channel
		c_acc_local = 0;
	}
	
	if (c_acc_2_local == 10)	{
		ADCB_CH0_CTRL |= ADC_CH_START_bm; // ADC ZOUT
		//the end of the conversion on X channel will start the conversion on Y channel
		//and this end will start the conversion on Z channel, and in sequence the convertion on acc 3
		c_acc_2_local = 0;
	}
	
	if (c_bat_local >= 40)	{
		ADCA_CH0_CTRL |= ADC_CH_START_bm; // ADC BATTERY
		c_bat_local = 0;
	}
	
	if (c_ecg_local >= 400)	{
		f_zerar_buffers = 1;
		c_ecg_local = 0;
	}
}