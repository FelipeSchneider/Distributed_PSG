/*
 * Pl_principal.c
 *
 * Main routine of the main board of my PSG project.
 * The files MMC, diskio, ffconf, ff, integer belongs to the FatFS library
 *
 * Created: 16-Mar-16 09:56:53
 *  Author: Felipe Schneider
 */ 



#include "defines_global.h"
#include "Variables.h"
#include "ADS.h"
#include "HDC.h"
#include "OPT.h"
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
uint8_t gf_buffer_indicator = 0;	//indicates which buffer is active, if 1 we are using the g_buffer_uSD
uint8_t gf_start_transmission = 0;	//indicates that we must start a transmission to µSD

float g_hdc_humidity, g_hdc_temp;				//this is only for debug, it will not be save on µSD
uint16_t g_hdc_hum_result, g_hdc_temp_result;	//this will be saved on µSD
uint16_t g_lm35_ad, g_lm35_ad_2;;				//temperature returned from adc channel of LM35

uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
uint8_t gc_bat = 0;		//counts how many samples of adc battery values happened and were saved

float g_opt_lum;		//this is only for debug, it will not be save on µSD
uint16_t g_opt_result;	//this will be saved on µSD
uint8_t gc_opt = 0;		//counts how many samples of OPT values happened and were saved
uint16_t g_ldr_ad;		//this will be saved on µSD
uint8_t gc_ldr = 0;		//counts how many samples of ldr values happened and were saved

uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
uint8_t gc_acc = 0;		//counts how many samples of accelerometer values happened and were saved

uint16_t g_audio_ad;	//accelerometer results
uint8_t gc_audio = 0;	//counts how many samples of accelerometer values happened and were saved

uint8_t g_ads_eog_1, g_ads_eog_2, g_ads_eog_3; //bytes from ads, 1 is the most significant
uint16_t gc_eog = 0;	//counts how many samples of ADS EOG values happened and were saved

uint8_t g_error = 0;

uint8_t g_patient_number = 1;	//number of the patient, goes up to 9 and depends of the pre-existing file
/* LIST OF ERRORS
	error = 1; OPT with none or wrong manuffecture ID
*/

int main(void)
{
	uint16_t opt_id;
	static uint16_t gc_hour = 0;	//this counter will determine the moment when one hour is reached, then a new file will be created - counting until 3600
	IniRc32Clock();
	IniLeds();
	IniBotSinc();
	_delay_ms(200);
	ConfigSpi_F();		//ADS SPI
	ConfigSpi_E();		//uSD SPI
	InitADS();			//setting the initial configurations for ADS
	IniSerial_D0();		//for debug only
	IniI2C_HDC();
	HDCStart();
	ConfigHDC();
	IniAdConversor();
	g_buffer_uSD[0] = HEADER_1;
	g_buffer_uSD[1] = HEADER_2;
	g_buffer_uSD_2[0] = HEADER_1;
	g_buffer_uSD_2[1] = HEADER_2;
	
	IniI2C_OPT();
	opt_id = OPTReadID(); //it can be used to verify the operationality of the OPT, it must return 0x5449
	if (opt_id != 0x5449){
		g_error = 1;
		PORTK_OUTSET = PIN1_bm;
		//TODO:: fazer algo com este erro.
	}
	OPTConfig(DEFAULT_CONFIG_100);
	
	g_error = OpenSD();
	g_error = StartRecording();
 
	uint8_t c_keep = 0;
	while (c_keep < 20){	//waits for the sinc. bot be pressed.
		if((PORTK_IN & PIN0_bm)==1){
			c_keep = 0;
			PORTJ_OUTCLR = 0x0F;
		}
		else{
			c_keep++;
			PORTJ_OUTSET = 0x0F;
		}
		_delay_ms(100);
	}
	PORTK_OUTSET = PIN2_bm;

	EnableIntLevels();
// 	ini_timer_400hz();
//	on_timer_400hz();
	sei();	
	
	//Enable the DRDYB interruption
	PORTF_DIRCLR = PIN3_bm;
	PORTF_PIN3CTRL = 0x1A;
	PORTF_INT0MASK = PIN3_bm;
	PORTF_INTCTRL =  0x02;
	uint8_t c_min = 0;
    while(1)
    {
		static uint8_t c_hour_name = 1;
		PORTK_OUTTGL = PIN5_bm;		//do nothing, just make sure that it does not skip this part because of optimizations
		
// 		if((PORTF_IN & PIN3_bm)>=1){
// 			 PORTK_OUTSET = PIN4_bm;
// 		}
// 		else{
// 			PORTK_OUTCLR = PIN4_bm;
// 		}
		
		if (gf_start_transmission){	//if its time to start the transmission:
			gf_start_transmission = 0;
			c_min++;
			//TODO:: incluir aqui a rotina de criar um novo arquivo por hora
			_delay_ms(1);	//just to make sure that all buffers are ready
			PORTK_OUTCLR = PIN1_bm| PIN2_bm;	//TODO:: for debug only
			PORTJ_OUTCLR = 0x0F;
			if(c_min>59){
				c_min = 0;
				PORTK_OUTSET = PIN1_bm;	//TODO:: for debug only
			}
			gc_hour++;
			MakeLog();
			if(gc_hour >= 3600){
				PORTK_OUTSET = PIN2_bm;	
				c_hour_name++;
				MakeNewFile(c_hour_name);
				gc_hour = 0;
			}
					
		}//close if gf_start_transmission    
	}//close while 1
}//close main

ISR(TCF1_OVF_vect){
//	PORTK_OUTTGL = PIN4_bm;
}

ISR(PORTF_INT0_vect){		//400HZ		EOG SOURCE
	static uint8_t f_zerar_buffers = 0;
	static uint8_t c_sound_local = 0;
	static uint8_t c_acc_local = 0;
	static uint8_t c_light_local = 0;
	static uint8_t c_bat_local = 0;
	static uint16_t c_temp_local = 0;
	static uint16_t c_eog_local = 0;
	uint16_t opt_result, hdc_humidity, hdc_temp;
	uint8_t ads_buffer[3];
//	PORTK_OUTTGL = PIN4_bm;
	if (f_zerar_buffers){	//this makes sure that all conversions will be finished before the buffers are changed
		//and also that the buffers are changed before the 401th sample
		f_zerar_buffers = 0;
		gf_start_transmission = 1;
		gf_buffer_indicator = gf_buffer_indicator^0x01;	//change the buffer indicator
		CleanGlobalCounters();
	}
	
	c_sound_local++;
	c_acc_local++;
	c_light_local++;
	c_bat_local++;
	c_temp_local++;
	c_eog_local++;
	
//TODO:: falta fazer a gravação do EOG conforme os dados lidos do ADS;
	//EOG
	ChunkAcessADS(0x50, 3, &ads_buffer[0], ADS_READ);
	if (gf_buffer_indicator){
		g_buffer_uSD[EOG_OFFSET + gc_eog] = ads_buffer[0];
		g_buffer_uSD[EOG_OFFSET + gc_eog+1] = ads_buffer[1];
		g_buffer_uSD[EOG_OFFSET + gc_eog+2] = ads_buffer[2];
	}
	else{
		g_buffer_uSD_2[EOG_OFFSET + gc_eog] = ads_buffer[0];
		g_buffer_uSD_2[EOG_OFFSET + gc_eog+1] = ads_buffer[1];
		g_buffer_uSD_2[EOG_OFFSET + gc_eog+2] = ads_buffer[2];
	}
	gc_eog +=3;
	
	if (c_sound_local == 4)	{
		ADCB_CH3_CTRL |= ADC_CH_START_bm; //SOUND 1
		c_sound_local = 0;
	}
	
	if (c_acc_local == 10)	{
		ADCA_CH2_CTRL |= ADC_CH_START_bm; // ADC ZOUT
		//the end of the conversion on Z channel will start the conversion on Y channel
		//and this end will start the conversion on X channel
		c_acc_local = 0;
	}
	
	if (c_light_local == 40)	{
		ADCB_CH1_CTRL |= ADC_CH_START_bm; // ADC LDR
		opt_result = OPTReadIluminance();
		WriteOnBuffer(gf_buffer_indicator, (LUM_OPT_OFFSET+ gc_opt), opt_result);
		gc_opt = gc_opt + 2;
		c_light_local = 0;
	}
	
	if (c_bat_local >= 40)	{
		ADCB_CH2_CTRL |= ADC_CH_START_bm; // ADC BATTERY
		c_bat_local = 0;
	}
	
	if (c_temp_local == 394){	//The HDC takes 6.5ms to convert humidity and temperature, so we must trig it a while before the data is ready
		HDCTrig();
	}
	
	if (c_temp_local >= 400)	{
		ADCA_CH0_CTRL |= ADC_CH_START_bm; // LM35 2
		c_temp_local = 0;
		//The HDC always perform a temperature measurement  first (if the temperature measurement is active).
// 		hdc_temp = HDCReadData();
// 		hdc_humidity = HDCReadData();	//Do not change this order!
		uint8_t aux1,aux2;
		HDCAddressRead();
		while(!(TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
		aux1 = TWIE_MASTER_DATA; //read data
		while(!(TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
		aux2 = TWIE_MASTER_DATA;		//read data
		hdc_temp = ((uint16_t)(aux1<<8|aux2));
		
		while(!(TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
		aux1 = TWIE_MASTER_DATA; //read data
		TWIE_MASTER_CTRLC = TWI_MASTER_CMD_STOP_gc;	//when the next byte is received send an ACK and a STOP action
		while(!(TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
		aux2 = TWIE_MASTER_DATA;
		hdc_humidity = ((uint16_t)(aux1<<8|aux2));
//		HDCStop();
		WriteOnBuffer(gf_buffer_indicator, TEMP_HDC_OFFSET, hdc_temp);
		WriteOnBuffer(gf_buffer_indicator, UMID_HDC_OFFSET, hdc_humidity);
		
	}
	
	if (c_eog_local >= 400)	{
		f_zerar_buffers = 1;
		c_eog_local = 0;
	}
	
}//close the interrupt


