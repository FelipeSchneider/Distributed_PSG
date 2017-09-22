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
#define PLL_CLOCK						//Enables the PLL init functions (external xt necessary)
#define RC_32_CLOCK						//Enables the internal resonator init functions (32MHz with PLL with the actual settings)
#define	SERIAL_E0						//Enables the Uart D0 init functions
#define SPI_D0
#define ZERO_ASCII		48

#define	VALOR_A_TENSAO	0.001340814115672
#define	VALOR_B_TENSAO	-0.307399010115506

#define HEADER_1	0x53	//S em ascii
#define HEADER_2	0x54	//T em ascii

// Connected to port A
#define REFA_PIN	PIN0_bm
#define XOUT_ADXL	PIN1_bm
#define YOUT_ADXL	PIN2_bm
#define ZOUT_ADXL	PIN3_bm

#define BAT_DIV		PIN5_bm

// Connected to port C
#define LED2		PIN0_bm
#define LED3		PIN1_bm

#define SS_uSD		PIN4_bm
#define MOSI_uSD	PIN5_bm
#define MISO_uSD	PIN6_bm
#define SCK_uSD		PIN7_bm

// Connected to port D
#define SINC_PIN	PIN0_bm
#define RST_nRF		PIN1_bm
#define ACT_nRF		PIN2_bm
#define RDY_nRF		PIN3_bm
#define CS_nRF		PIN4_bm
#define MOSI_nRF	PIN5_bm
#define MISO_nRF	PIN6_bm
#define SCK_nRF		PIN7_bm

// Connected to port E
#define UART_RX		PIN2_bm
#define UART_TX		PIN3_bm

#endif /* DEFINES_GLOBAL_H_ */