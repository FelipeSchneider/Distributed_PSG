/*
 * CFile1.c
 *
 * Created: 20/03/2015 09:15:04
 *  Author: Felipe
 */ 

#include "Initial_Set.h"
#include "defines_global.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

////////////////////////////////////CLOCK INITIALIZATION//////////////////////////////////////////////
#ifdef XT_CLOCK
void IniXtClock(void){
	OSC_XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC_CTRL |= OSC_XOSCEN_bm;
	while(!(OSC_STATUS & OSC_XOSCRDY_bm)); //Wait for clock stabilization
	CCP = CCP_IOREG_gc;
	CLK_CTRL = CLK_SCLKSEL_XOSC_gc;
}
#endif

#ifdef PLL_CLOCK
//sets the PLL to work with an external XT and multiplies its freq. by 2
void IniPllClock(void){
	OSC_XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	OSC_CTRL |= OSC_XOSCEN_bm;
	while(!(OSC_STATUS & OSC_XOSCRDY_bm));		//Wait for clock stabilization
	
	OSC_PLLCTRL = OSC_PLLSRC_XOSC_gc | 0x02;	// defines the PLL source and its multiplier (2) 
	OSC_CTRL |= OSC_PLLEN_bm;
	while(!(OSC_STATUS & OSC_PLLRDY_bm));
	CCP = CCP_IOREG_gc;
	CLK_CTRL = CLK_SCLKSEL_PLL_gc;
}
#endif

#ifdef RC_32_CLOCK
// Results in a clock of 32Mhz
void IniRc32Clock(void){
	OSC_CTRL |= OSC_RC32MEN_bm;					// Active the internal oscilator  
	while(!(OSC_STATUS & OSC_RC32MRDY_bm));		// Wait for clock stabilization
		
	OSC_PLLCTRL = OSC_PLLSRC_RC32M_gc | 0x04;   // defines the PLL source and its multiplier (4)
	OSC_CTRL |= OSC_PLLEN_bm;				    // active the PLL
	while(!(OSC_STATUS & OSC_PLLRDY_bm));	    // Wait for stabilization
	CCP = CCP_IOREG_gc;						    // Special register must be set in this case
	CLK_CTRL = CLK_SCLKSEL_PLL_gc;			    // Changes the clock source
}
#endif
//////////////////////////////////////// END OF CLOCK INIT ///////////////////////////////////////////


// for baud rate look up at ATxmega128a1u manual, pg 290 and 291
#ifdef SERIAL_D0
void IniSerial_D0(void){
	USARTD0_BAUDCTRLA = 131;				// Setting the baudrate (BSEL) 131 for 115200
	USARTD0_BAUDCTRLB = 0b11010000;			// Setting the baudrate (BSCALE) -3 for 115200
	USARTD0_CTRLC = (USART_CMODE_ASYNCHRONOUS_gc) | (USART_PMODE_DISABLED_gc) | (USART_CHSIZE_8BIT_gc);	//Uart mode
	//Observar a opçao de multiplexaçao de pinos REMAP pg 152 manual
	PORTD_DIRSET = PIN3_bm;				//PIN D3 (TX) is an output
	PORTD_DIRCLR = PIN2_bm;				//PIN D2 (RX) is an input
	USARTD0_CTRLB = USART_TXEN_bm | USART_RXEN_bm;	//Enables TX and RX
//	USARTD0_CTRLA = 0b01010100;			//INT RX -> level baixo e no fim do byte, INT TX ->  level baixo, INT DATA VAZIO -> desligado
}
void Send_ByteSerial_D0(uint8_t byte_to_send){
	USARTD0_DATA = byte_to_send;
}

uint8_t Get_ByteSerial_D0(void){
	return(USARTD0_DATA);
}

#endif

//TODO:: ajust the timers init

void ini_timer_400hz(void){
	TCF1_CTRLA = TC_CLKSEL_OFF_gc;
	TCF1_CTRLB = TC_BYTEM_NORMAL_gc | TC_BYTEM_NORMAL_gc; //buffer em PER(PER define o TOP)
	TCF1_PER = 10000;	//10000 para 400Hz
	TCF1_INTCTRLA = TC_OVFINTLVL_HI_gc;
	TCF1_CTRLFSET = 1;
}

void on_timer_400hz(void){
	TCF1_CTRLA = TC_CLKSEL_DIV8_gc;
	TCF1_INTCTRLA = 3;
}

void off_timer_400hz(void){
	TCF1_CTRLA = TC_CLKSEL_OFF_gc;
}

	
void IniLeds(void){
	PORTK_DIRSET = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm;	//make the led pins outputs
}

void IniBotSinc(void){
	PORTK_DIRCLR = PIN0_bm;
	PORTK_PIN0CTRL = PORT_OPC_PULLUP_gc;
	PORTJ_DIRSET = 0x0F;	//make the J0 to J3 outputs
}

void EnableIntLevels(void){
	PMIC_CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; //Enable all levels of interrupts
}

#ifdef SPI_D0
	void ConfigSpi_D(void){
		PORTD_DIRSET = PIN5_bm|PIN4_bm|PIN7_bm;	//saidas para MOSI, SCK, SS
		PORTD_OUTSET = PIN4_bm;							//desabilita o slave
		PORTD_DIRCLR = PIN6_bm;							//entrada MISO
		PORTD_PIN6CTRL = PORT_OPC_PULLUP_gc;
		SPID_CTRL = SPI_MASTER_bm | SPI_ENABLE_bm;		//JA ATIVANDO A SPI AQUI e definindo o clock como o Clockper/4
	}
	void ReconfigSpi_D(void){
		SPID_CTRL |= SPI_CLK2X_bm; 			//drobra a freq da SPI
	}

	void StartSpi_D(void){
		cli();
		PORTD_OUTCLR = PIN4_bm;						//habilita slave
	}

	unsigned char SendSpiByte_D(unsigned char dataOut){
		unsigned char temp = 0;
		SPID_DATA = dataOut;
		while(!(SPID_STATUS & SPI_IF_bm)){};
		temp = SPID_DATA;
		return(temp);
	}

	void StopSpi_D(void){
		PORTD_OUTSET = PIN4_bm;							//Desabilita slave
		sei();
	}
#endif