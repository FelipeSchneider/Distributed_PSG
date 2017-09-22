
/*
 * AD_conv.h
 *
 * Created: 18-Mar-16 15:33:08
 *  Author: Felipe Schneider
 */ 


#ifndef AD_CONV_H_
#define AD_CONV_H_

	#define ADC_LM35_1_CHANNEL					ADC_CH_MUXPOS_PIN1_gc		//ADC A mux
	#define ADC_LM35_2_CHANNEL					ADC_CH_MUXPOS_PIN2_gc		//ADC A mux
	#define ADC_XOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN11_gc		//ADC A mux
	#define ADC_YOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN4_gc		//ADC A mux

	#define ADC_ZOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN5_gc		//ADC B mux
	#define ADC_LDR_CHANNEL						ADC_CH_MUXPOS_PIN14_gc		//ADC B mux
	#define ADC_BAT_CHANNEL						ADC_CH_MUXPOS_PIN15_gc		//ADC B mux
	#define ADC_SOUND_1_CHANNEL					ADC_CH_MUXPOS_PIN1_gc		//ADC B mux

	void IniAdConversor(void);
#endif /* AD_CONV_H_ */