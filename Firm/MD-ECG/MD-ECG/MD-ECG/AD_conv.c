/*
 * AD_conv.c
 *
 * Created: 18-Mar-16 15:07:57
 *  Author: Felipe Schneider
 */ 

#include "defines_global.h"
#include "AD_conv.h"
#include "Variables.h"
#include "data_SD.h"

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

float g_volts_bat = 420;

// This uC has 2 ADC's, each have 4 channels, what makes possible to start 8 convertions almost at the same time
// also each channel have it's on result address and interrupt handler 
void IniAdConversor(void){
	PORTA_DIRCLR = 0xFF;									// All pins in portA are inputs
	PORTB_DIRCLR = 0xFF;									// All pins in portB are inputs
	ADCA_PRESCALER = ADC_PRESCALER_DIV512_gc;				// CLKper/512 = 32e6/512 = 62.5kHz 
	ADCB_PRESCALER = ADC_PRESCALER_DIV512_gc;				// CLKper/512 = 32e6/512 = 62.5kHz
	//the ADC clock must be that small in order to make the channels independent...
	ADCA_REFCTRL = ADC_REFSEL_AREFA_gc;						//Both ADCs references are the pin A0
	ADCB_REFCTRL = ADC_REFSEL_AREFA_gc;
	
	ADCA_INTFLAGS = 0x0F;									// Clear the ADCs interrupt flags
	ADCB_INTFLAGS = 0x0F;									// Clear the ADCs interrupt flags
	
	//Defining the channel's for each variable for channel A
	ADCA_CH0_MUXCTRL = ADC_BAT;									//Freq sampling 1Hz					
	ADCA_CH1_MUXCTRL = ADC_XOUT_ADXL_CHANNEL;					//Freq sampling 40Hz	
	ADCA_CH2_MUXCTRL = ADC_YOUT_ADXL_CHANNEL;					//Freq sampling 40Hz	
	ADCA_CH3_MUXCTRL = ADC_ZOUT_ADXL_CHANNEL;					//Freq sampling 40Hz
	//for channel B	
	ADCB_CH0_MUXCTRL = ADC_X_2_OUT_ADXL_CHANNEL;				//Freq sampling 40Hz	
	ADCB_CH1_MUXCTRL = ADC_Y_2_OUT_ADXL_CHANNEL;				//Freq sampling 40Hz	
	ADCB_CH2_MUXCTRL = ADC_Z_2_OUT_ADXL_CHANNEL;				//Freq sampling 40Hz	

	
	//Defining the channel's interrupt priority for channel A
	ADCA_CH0_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	ADCA_CH1_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	ADCA_CH2_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	ADCA_CH3_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	//for channel B
	ADCB_CH0_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	ADCB_CH1_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	ADCB_CH2_INTCTRL |= ADC_CH_INTLVL_LO_gc;
	
	ADCA_CTRLA |= ADC_ENABLE_bm;								   //enables channel A
	ADCB_CTRLA |= ADC_ENABLE_bm;								   //channel B
	
	//All channels are single ended
	ADCA_CH0_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA_CH1_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA_CH2_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCA_CH3_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	
	ADCB_CH0_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCB_CH1_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
	ADCB_CH2_CTRL |= ADC_CH_INPUTMODE_SINGLEENDED_gc;
}

// BAT_DIV
ISR(ADCA_CH0_vect){
	//Nas configuraçoes atuais o valor de 4,2 volts na bateria equivale a uma conversao de 1716
	//já o valor minimo de 3,4 volts equivale a 1426.
	float bat_val_agora;
	static float bat_val_anterior;
	volatile static uint16_t bat_amostra;

	bat_amostra = ADCA_CH0_RES;
		
	//filtro digital
	bat_val_agora = (0.07*bat_amostra + bat_val_anterior*0.93);
		
	g_volts_bat = VALOR_A_TENSAO * bat_val_agora + VALOR_B_TENSAO;
	bat_val_anterior = bat_val_agora;
	if(gc_bat == 0){
		WriteOnBuffer(gf_buffer_indicator, BAT_VAL_OFFSET, ((uint16_t)(g_volts_bat*100)));
		gc_bat = 1;
		//TODO:: ADICIONAR UM POWER DOWN CASO A BATERIA FIQUE FRACA DE MAIS!
	}
}

// ADC_XOUT_ADXL_CHANNEL
ISR(ADCA_CH1_vect){
	g_acc_x = ADCA_CH1_RES;
	WriteOnBuffer(gf_buffer_indicator, ACC_X_OFFSET + gc_acc, g_acc_x);
	ADCA_CH2_CTRL |= ADC_CH_START_bm; //initiate the acc Y conversion
}

// ADC_YOUT_ADXL_CHANNEL
ISR(ADCA_CH2_vect){
	g_acc_y = ADCA_CH2_RES;
	WriteOnBuffer(gf_buffer_indicator, ACC_Y_OFFSET + gc_acc, g_acc_y);
	ADCA_CH3_CTRL |= ADC_CH_START_bm; //initiate the acc Z conversion
}

// ADC_ZOUT_ADXL_CHANNEL
ISR(ADCA_CH3_vect){
	g_acc_z = ADCA_CH3_RES;
	WriteOnBuffer(gf_buffer_indicator, ACC_Z_OFFSET + gc_acc, g_acc_z);
	gc_acc = + gc_acc + 2;
}

// ADC_X_2_OUT_ADXL_CHANNEL AND X3
ISR(ADCB_CH0_vect){
	if(f_select_acc == 0){
		g_acc_x_2 = ADCB_CH0_RES;
		WriteOnBuffer(gf_buffer_indicator, ACC_X_2_OFFSET + gc_acc_2, g_acc_x_2);
	}
	else{
		g_acc_x_3 = ADCB_CH0_RES;
		WriteOnBuffer(gf_buffer_indicator, ACC_X_3_OFFSET + gc_acc_3, g_acc_x_3);
	}
	ADCB_CH1_CTRL |= ADC_CH_START_bm; 
}

// ADC_Y_2_OUT_ADXL_CHANNEL AND Y3
ISR(ADCB_CH1_vect){
	if(f_select_acc == 0){
	g_acc_y_2 = ADCB_CH1_RES;
	WriteOnBuffer(gf_buffer_indicator, ACC_Y_2_OFFSET + gc_acc_2, g_acc_y_2);
	}
	else{
		g_acc_y_3 = ADCB_CH1_RES;
		WriteOnBuffer(gf_buffer_indicator, ACC_Y_3_OFFSET + gc_acc_3, g_acc_y_3);
	}
	ADCB_CH2_CTRL |= ADC_CH_START_bm; 
}	


//ADC_Z_2_OUT_ADXL_CHANNEL AND Z3
ISR(ADCB_CH2_vect){
	if(f_select_acc == 0){
		g_acc_z_2 = ADCB_CH2_RES;
		WriteOnBuffer(gf_buffer_indicator, ACC_Z_2_OFFSET + gc_acc_2, g_acc_z_2);
		ADCB_CH0_CTRL |= ADC_CH_START_bm; //starts a new round of conversions, but now for the third acc
		f_select_acc = 1;
		gc_acc_2 = + gc_acc_2 + 2;
	}
	else{
		g_acc_z_3 = ADCB_CH2_RES;
		WriteOnBuffer(gf_buffer_indicator, ACC_Z_3_OFFSET + gc_acc_3, g_acc_z_3);
		f_select_acc = 0;	//finish the round of conversions.
		gc_acc_3 = + gc_acc_3 + 2;
	}
}
