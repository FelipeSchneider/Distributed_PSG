/*
 * uSD_SPI.h
 *
 * Created: 18-Mar-16 13:35:26
 *  Author: Felipe Schneider
 */ 


#ifndef USD_SPI_H_
#define USD_SPI_H_

#include "defines_global.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// TODO:: ACERTAR AS BIBLIOTECAS SPI
void ConfigSpi_C(void);
void ReconfigSpi_C(void);
inline unsigned char SendSpiByte_C(unsigned char dataOut)
{
	unsigned char temp = 0;
	SPIC_DATA = dataOut;
	while(!(SPIC_STATUS & SPI_IF_bm)){};
	temp = SPIC_DATA;
	return(temp);
}

inline void StartSpi_C(void)
{
	cli();
	//TODO:: vERIFICAR SE EU PRECISO PARA COM AS INTERRUPÇÕES, E SE EU POSSO FAZER ISSO!
	PORTC_OUTCLR = SS_uSD;						//habilita slave
}

inline void StopSpi_C(void)
{
	PORTC_OUTSET = SS_uSD;							//Desabilita slave
	sei();
}

#endif /* USD_SPI_H_ */