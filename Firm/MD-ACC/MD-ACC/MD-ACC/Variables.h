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

#define ACC_X_OFFSET		2
#define ACC_Y_OFFSET		82
#define ACC_Z_OFFSET		162

#define BAT_VAL_OFFSET		242
#define CRC_OFFSET			244
#define BUFFER_SIZE			246

extern uint8_t g_buffer_uSD[BUFFER_SIZE], g_buffer_uSD_2[BUFFER_SIZE];	//buffers that will save data into µSD
extern uint8_t gf_buffer_indicator;		//indicates which buffer is active, if 1 we are using the g_buffer_uSD
extern uint8_t gf_start_transmission;	//indicates that we must start a transmission to µSD

 
extern uint8_t g_bat_ad;		//value returned by the ad conversion of the battery
extern uint8_t gc_bat;			//counts how many samples of ADC battery values happened and were saved

extern uint16_t g_acc_x, g_acc_y, g_acc_z; //accelerometer results
extern uint8_t gc_acc;			//counts how many samples of accelerometer values happened and were saved

extern uint8_t g_error;

extern uint8_t g_patient_number;	//number of the patient, goes up to 9 and depends of the pre-existing file
#endif /* VARIABLES_H_ */