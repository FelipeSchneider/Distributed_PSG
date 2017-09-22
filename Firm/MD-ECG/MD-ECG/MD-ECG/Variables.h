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
#define ECG_OFFSET			2

#define ACC_X_OFFSET		1202
#define ACC_Y_OFFSET		1282
#define ACC_Z_OFFSET		1362

#define ACC_X_2_OFFSET		1442
#define ACC_Y_2_OFFSET		1522
#define ACC_Z_2_OFFSET		1602

#define ACC_X_3_OFFSET		1682
#define ACC_Y_3_OFFSET		1762
#define ACC_Z_3_OFFSET		1842

#define BAT_VAL_OFFSET		1922
#define CRC_OFFSET			1924
#define BUFFER_SIZE			1926

extern uint8_t g_buffer_uSD[BUFFER_SIZE], g_buffer_uSD_2[BUFFER_SIZE];	//buffers that will save data into µSD
extern uint8_t gf_buffer_indicator;		//indicates which buffer is active, if 1 we are using the g_buffer_uSD
extern uint8_t gf_start_transmission;	//indicates that we must start a transmission to µSD

 
extern uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
extern uint8_t gc_bat;			//counts how many samples of ADC battery values happened and were saved

extern uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
extern uint8_t gc_acc;			//counts how many samples of accelerometer values happened and were saved
extern uint16_t g_acc_x_2, g_acc_y_2, g_acc_z_2; //accelerometer results
extern uint8_t gc_acc_2;			//counts how many samples of accelerometer values happened and were saved
extern uint16_t g_acc_x_3, g_acc_y_3, g_acc_z_3; //accelerometer results
extern uint8_t gc_acc_3;			//counts how many samples of accelerometer values happened and were saved
extern uint8_t f_select_acc;		//changes btw acc2 and acc3

extern uint8_t g_ads_ecg_1, g_ads_ecg_2, g_ads_ecg_3; //bytes from ADS, 1 is the most significant
extern uint16_t gc_ecg;			//counts how many samples of ADS EOG values happened and were saved

extern uint8_t g_error;

extern uint8_t g_patient_number;	//number of the patient, goes up to 9 and depends of the pre-existing file
#endif /* VARIABLES_H_ */