/*
 * COM_ADS.c
 *
 * Created: 03/12/2015 13:37:50
 *  Author: Felipe Schneider
 */ 

#include "Initial_Set.h"
#include "defines_global.h"
#include "ADS.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void ConfigSpi_F(void)
{
	PORTF_DIRSET = SS_ADS|SCK_ADS|MOSI_ADS;			//as output: MOSI, SCK, SS
	PORTF_OUTSET = SS_ADS;							//Slave is deactivate
	PORTF_DIRCLR = MISO_ADS | DRDYB_ADS | ALARM_ADS ;						//MISO and DRDYB as input
	PORTF_PIN6CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for the MISO
	PORTF_PIN3CTRL = PORT_OPC_PULLUP_gc;
	SPIF_CTRL = SPI_PRESCALER_DIV64_gc | SPI_MODE_3_gc;
	SPIF_CTRL |= SPI_MASTER_bm | SPI_ENABLE_bm;		//Activates the SPI and makes the SCLK = IC clock/4 (8MHz if the CI is running at 32)
}
void ReconfigSpi_F(void){
	SPIF_CTRL |= SPI_CLK2X_bm; 			//drobra a freq da SPI
}

//inline functions, declared in the .h file
void StartSpi_F();
unsigned char SendSpiByte_F(unsigned char dataOut);
void StopSpi_F();

//Function that sends or returns a byte of information to or from the ADS1293
uint8_t RandomAcessADS(uint8_t Address, uint8_t Info, uint8_t Action){
	uint8_t aux;
	aux = Action | Address;
	StartSpi_F();
	SendSpiByte_F(aux);
	aux = SendSpiByte_F(Info);
	StopSpi_F();
	return(aux);
}

//Function that sends or returns a chunck of information to or from the ADS1293
void ChunkAcessADS(uint8_t Base_add, uint8_t size, uint8_t *data_vector, uint8_t Action){
	uint8_t i;
	StartSpi_F();
	if(Action == ADS_READ){
		SendSpiByte_F(Base_add | ADS_READ);
		for(i=0;i<size;i++){
			*data_vector = SendSpiByte_F(0xff);
			data_vector = data_vector + 1;
		}
	}
	else{	//se a ação for de escrita
		SendSpiByte_F(Base_add | ADS_WRITE);
		for(i=0;i<size;i++){
			SendSpiByte_F(*data_vector);
			data_vector = data_vector + 1;
		}
	}
	StopSpi_F();
}

//Initial settings for ADS1293 in the main board
void InitADS(void){
	//address, information, action
	RandomAcessADS(0x00,0x00,WRITE_ADS);		// parar as conversoes, DEVEMOS FAZER ISTO ANTES DE QQ COISA
	RandomAcessADS(0x12,0x02,WRITE_ADS);		// desabilita o oscilador interno - isto deve ser feito antes de habilitar o oscilador externo
	RandomAcessADS(0x01,0x0A,WRITE_ADS);	// Channel 1+ -> IN1			Channel 1- -> IN2
	RandomAcessADS(0x02,0x00,WRITE_ADS);		// Disconecta o canal 2
	RandomAcessADS(0x03,0x00,WRITE_ADS);		// Disconecta o canal 3
	RandomAcessADS(0x0A,0x03,WRITE_ADS);		// Common mode formado pelo IN1 e IN2
	RandomAcessADS(0x0C,0x73,WRITE_ADS);		// Right-leg drive conectado no pino IN3 -- será o terra do sistema (meio da testa), high bandwidth and high cap drive
	RandomAcessADS(0x12,0x04,WRITE_ADS);		// habilita oscilador
//TODO:: alterar para 0x09
	RandomAcessADS(0x13,0x3F,WRITE_ADS);		// frequencia maxima de operação para todos os canais
	RandomAcessADS(0x22,0x10,WRITE_ADS);		// Configures the R3 decimation rate as 16 for channel 1. 
//	RandomAcessADS(0x23,0x10,WRITE_ADS);		// Configures the R3 decimation rate as 16 for channel 2
	RandomAcessADS(0x27,0x08,WRITE_ADS);		// Configures the DRDYB source to channel 1 (or fastest channel).
	RandomAcessADS(0x2F,0x10,WRITE_ADS);		// Enables channel 1 for loop read-back mode.
	RandomAcessADS(0x00,0x01,WRITE_ADS);		// Starts data conversion.
}