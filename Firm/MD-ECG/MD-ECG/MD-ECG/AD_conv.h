
/*
 * AD_conv.h
 *
 * Created: 18-Mar-16 15:33:08
 *  Author: Felipe Schneider
 */ 


#ifndef AD_CONV_H_
#define AD_CONV_H_

	#define ADC_BAT								ADC_CH_MUXPOS_PIN1_gc		//ADC A mux
	#define ADC_XOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN2_gc		//ADC A mux
	#define ADC_YOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN3_gc		//ADC A mux
	#define ADC_ZOUT_ADXL_CHANNEL				ADC_CH_MUXPOS_PIN4_gc		//ADC A mux

	#define ADC_X_2_OUT_ADXL_CHANNEL			ADC_CH_MUXPOS_PIN15_gc		//ADC A mux
	#define ADC_Y_2_OUT_ADXL_CHANNEL			ADC_CH_MUXPOS_PIN14_gc		//ADC A mux
	#define ADC_Z_2_OUT_ADXL_CHANNEL			ADC_CH_MUXPOS_PIN13_gc		//ADC A mux

	void IniAdConversor(void);
#endif /* AD_CONV_H_ */