/*
 * Variables.h
 *
 * Created: 29/03/2016 17:34:00
 *  Author: Win7
 */ 


#ifndef VARIABLES_H_
#define VARIABLES_H_
#include <avr/io.h>

//Todas as amostras dever ser organizadas em um único vetor a fim de serem transmitidas para o µSD
//assim sendo, devo definir offsets para o inicio de cada variavel dentro do vetor g_buffer_uSD
#define HEADER_OFFSET		0
#define EOG_OFFSET			2
#define AUDIO_OFFSET		1202
#define ACC_X_OFFSET		1402
#define ACC_Y_OFFSET		1482
#define ACC_Z_OFFSET		1562
#define LUM_OPT_OFFSET		1642
#define LUM_LDR_OFFSET		1662
#define BAT_VAL_OFFSET		1682
#define UMID_HDC_OFFSET		1684
#define TEMP_HDC_OFFSET		1686
#define TEMP_LM_1_OFFSET	1688
#define TEMP_LM_2_OFFSET	1690
#define CRC_OFFSET			1692
#define BUFFER_SIZE			1694

extern uint8_t g_buffer_uSD[BUFFER_SIZE], g_buffer_uSD_2[BUFFER_SIZE];	//buffers that will save data into µSD
extern uint8_t gf_buffer_indicator;		//indicates which buffer is active, if 1 we are using the g_buffer_uSD
extern uint8_t gf_start_transmission;	//indicates that we must start a transmission to µSD

 
extern float g_hdc_humidity, g_hdc_temp; //this is only for debug, it will not be save on µSD
extern uint16_t g_hdc_hum_result, g_hdc_temp_result; //this will be saved on µSD
extern uint16_t g_lm35_ad, g_lm35_ad_2;	//temperature returned from ADC channel of LM35

extern uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
extern uint8_t gc_bat;			//counts how many samples of ADC battery values happened and were saved

extern float g_opt_lum;			//this is only for debug, it will not be save on µSD
extern uint16_t g_opt_result;	//this will be saved on µSD
extern uint8_t gc_opt;			//counts how many samples of OPT values happened and were saved
extern uint16_t g_ldr_ad;		//this will be saved on µSD
extern uint8_t gc_ldr;			//counts how many samples of LDR values happened and were saved


extern uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
extern uint8_t gc_acc;			//counts how many samples of accelerometer values happened and were saved

extern uint16_t g_audio_ad;		//audio results
extern uint8_t gc_audio;		//counts how many samples of audio values happened and were saved

extern uint8_t g_ads_eog_1, g_ads_eog_2, g_ads_eog_3; //bytes from ADS, 1 is the most significant
extern uint16_t gc_eog;			//counts how many samples of ADS EOG values happened and were saved

extern uint8_t g_error;

extern uint8_t g_patient_number;	//number of the patient, goes up to 9 and depends of the pre-existing file

#endif /* VARIABLES_H_ */