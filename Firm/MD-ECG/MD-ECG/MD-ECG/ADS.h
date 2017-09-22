/*
 * COM_ADS.h
 *
 * Created: 03/12/2015 13:42:32
 *  Author: Felipe Schneider
 */ 


#ifndef COM_ADS_H_
#define COM_ADS_H_

#define ADS_READ	0x80				//mask to be add to the ADS first byte. It makes the difference btw a read or write command
#define ADS_WRITE	0x00

	#include "defines_global.h"
	#include <avr/io.h>
	#include <avr/interrupt.h>
	
	void ConfigSpi_F(void);
	void ReconfigSpi_F(void);
	void InitADS(void);
	
	inline unsigned char SendSpiByte_F(unsigned char dataOut)
	{
		unsigned char temp = 0;
		SPIF_DATA = dataOut;
		while(!(SPIF_STATUS & SPI_IF_bm)){};
		temp = SPIF_DATA;
		return(temp);
	}

	inline void StartSpi_F(void)
	{
		cli();
		PORTF_OUTCLR = SS_ADS;						//habilita slave
	}

	inline void StopSpi_F(void)
	{
		PORTF_OUTSET = SS_ADS;							//Desabilita slave
		sei();
	}
	uint8_t RandomAcessADS(uint8_t Address, uint8_t Info, uint8_t Action);
	void ChunkAcessADS(uint8_t Base_add, uint8_t size, uint8_t *data_vector, uint8_t Action);


#endif /* COM_ADS_H_ */