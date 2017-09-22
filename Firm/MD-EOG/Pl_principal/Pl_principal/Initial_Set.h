/*
 * Initial_Set.h
 *
 * Created: 12/03/2016 16:00:41
 *  Author: Felipe Schneider
 */ 



#ifndef INITIAL_SET_H_
#define INITIAL_SET_H_
	
#include "defines_global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
	
#ifdef XT_CLOCK
	void IniXtClock(void);
#endif
	
#ifdef PLL_CLOCK
	void IniPllClock(void);
#endif
	
#ifdef RC_32_CLOCK
	void IniRc32Clock(void);
#endif

#ifdef SERIAL_D0
	void IniSerial_D0(void);
	void Send_ByteSerial_D0(uint8_t byte_to_send);
	uint8_t Get_ByteSerial_D0(void);
#endif
	
void EnableIntLevels(void);
void IniLeds(void);
void IniBotSinc(void);
	
#ifdef SPI_D0
	void ConfigSpi_D(void);
	void ReconfigSpi_D(void);
	void StartSpi_D(void);
	unsigned char SendSpiByte_D(unsigned char dataOut);
	void StopSpi_D(void);
#endif	

void ini_timer_400hz(void);
void on_timer_400hz(void);
void off_timer_400hz(void);

#endif /* INITIAL_SET_H_ */