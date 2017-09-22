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

void ConfigSpi_E(){
	PORTE_DIRSET = SS_uSD|SCK_uSD|MOSI_uSD;			//as output: MOSI, SCK, SS
	PORTE_OUTSET = SS_uSD;							//Slave is deactivate
	PORTE_DIRCLR = MISO_uSD;						//Miso as input				
	PORTE_PIN6CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for te MISO
	SPIE_CTRL = SPI_MASTER_bm | SPI_ENABLE_bm;		//Activates the SPI and makes the SCLK = IC clock/4 (8MHz if the CI is running at 32)	
}
void ReconfigSpi_E(){
	SPIE_CTRL |= SPI_CLK2X_bm; 			//Doubles the SPI freq.
}
	
//inline functions, declared in the .h file
void StartSpi_E();
unsigned char SendSpiByte_E(unsigned char dataOut);
void StopSpi_E();
