/*
 * uSD_SPI.c
 *
 * Created: 18-Mar-16 13:34:13
 *  Author: Felipe Schneider
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uSD_SPI.h"
#include "defines_global.h"

void ConfigSpi_C(){
	PORTC_DIRSET = SS_uSD|SCK_uSD|MOSI_uSD;			//as output: MOSI, SCK, SS
	PORTC_OUTSET = SS_uSD;							//Slave is deactivate
	PORTC_DIRCLR = MISO_uSD;						//Miso as input				
	PORTC_PIN6CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for te MISO
	SPIC_CTRL = SPI_MASTER_bm | SPI_ENABLE_bm | SPI_PRESCALER_DIV16_gc;		//Activates the SPI and makes the SCLK = IC clock/4 (8MHz if the CI is running at 32)	
}
void ReconfigSpi_C(){
	SPIC_CTRL |= SPI_CLK2X_bm; 			//Doubles the SPI freq.
}
	
//inline functions, declared in the .h file
void StartSpi_C();
unsigned char SendSpiByte_C(unsigned char dataOut);
void StopSpi_C();
