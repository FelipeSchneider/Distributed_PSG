/*
 * defines_global.h
 *
 * Created: 16-Mar-16 10:03:14
 *  Author: Felipe Schneider
 */ 


#ifndef DEFINES_GLOBAL_H_
#define DEFINES_GLOBAL_H_

#define lo8(x) ((uint8_t)((x) & 0xff))
#define hi8(x) ((uint8_t)((x & 0xff00)>>8))
#define hi16(x) ((uint8_t)((x & 0xff0000)>>16))
#define hi24(x) ((uint8_t)((x & 0xff000000)>>24))

#define F_CPU			32000000UL		//CPU CLOCK (for delay lib)
//#define XT_CLOCK						//Enables the external XT init functions (btw 12 an 16MHz with the actual settings)
//#define PLL_CLOCK						//Enables the PLL init functions (external xt necessary)
#define RC_32_CLOCK						//Enables the internal resonator init functions (32MHz with PLL with the actual settings)
#define	SERIAL_D0						//Enables the Uart D0 init functions
#define ZERO_ASCII		48

#define READ_ADS	0x80
#define WRITE_ADS	0x00

#define	VALOR_A_TENSAO	0.001340814115672
#define	VALOR_B_TENSAO	-0.274399010115506

#define HEADER_1	0x53	//S em ascii
#define HEADER_2	0x54	//T em ascii

// Connected to port A
#define REFA_PIN	PIN0_bm
#define LM35_1_OUT	PIN1_bm
#define LM35_2_OUT	PIN2_bm
#define XOUT_ADXL	PIN3_bm
#define YOUT_ADXL	PIN4_bm
#define ZOUT_ADXL	PIN5_bm
#define LDR_OUT		PIN6_bm
#define BAT_DIV		PIN7_bm

// Connected to port B
#define REFB_PIN	PIN0_bm
#define SOUND_1_OUT	PIN1_bm				//probably the best place to analise the ambient sound noise 
#define SOUND_2_OUT	PIN2_bm

// Connected to port C
#define PIN_8_nRF	PIN0_bm
#define RST_nRF		PIN1_bm
#define ACT_nRF		PIN2_bm
#define RDY_nRF		PIN3_bm
#define CS_nRF		PIN4_bm
#define MOSI_nRF	PIN5_bm
#define MISO_nRF	PIN6_bm
#define SCK_nRF		PIN7_bm

// Connected to port E
#define SDA_HDC		PIN0_bm
#define SCL_HDC		PIN1_bm
#define DRDY_HDC	PIN2_bm

#define SS_uSD		PIN4_bm
#define MOSI_uSD	PIN5_bm	
#define MISO_uSD	PIN6_bm	
#define SCK_uSD		PIN7_bm	

// Connected to port F
#define SDA_OPT		PIN0_bm
#define SCL_OPT		PIN1_bm
#define INT_OPT		PIN2_bm
#define DRDYB_ADS	PIN3_bm
#define SS_ADS		PIN4_bm
#define MOSI_ADS	PIN5_bm
#define MISO_ADS	PIN6_bm
#define SCK_ADS		PIN7_bm

// Connected to port H

#define ALARM_ADS	PIN0_bm

// Connected to port K
#define SINC_BOT	PIN0_bm
#define LED_2		PIN1_bm
#define LED_3		PIN2_bm
#define LED_4		PIN3_bm
#define LED_5		PIN4_bm

#endif /* DEFINES_GLOBAL_H_ */