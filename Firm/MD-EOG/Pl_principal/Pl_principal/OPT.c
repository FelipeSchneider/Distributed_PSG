/*
 * OPT.c
 *
 * Created: 18-Mar-16 18:32:09
 * Author: Felipe Schneider
 * OPT address is:   1000100X where X is the W/R bit
 * OPT IS ON PORTF
 */ 

#include "OPT.h"
#include "defines_global.h"
#include "Variables.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

void IniI2C_OPT(void){
	PORTF_DIRCLR = INT_OPT;
	PORTF_PIN0CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for the SDA
	PORTF_PIN1CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for the SCL
	PORTF_PIN2CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups for the INT
	TWIF_MASTER_CTRLB = TWI_MASTER_SMEN_bm;	//enables the Quick command and smart mode  TWI_MASTER_QCEN_bm |
	
	TWIF_MASTER_BAUD = 35;	//35 FOR 400kHz and 0x98 for 100kHz TWI Frequency
	TWIF_MASTER_CTRLA = TWI_MASTER_ENABLE_bm;//| TWI_MASTER_WIEN_bp | TWI_MASTER_RIEN_bm;
	TWIF_MASTER_STATUS |= TWI_MASTER_BUSSTATE_IDLE_gc;
	TWIF_MASTER_CTRLC = 0x00;
}

void OPTStart(void){
	TWIF_MASTER_CTRLC = 0x01;	//starts
}

void OPTStop(void){
	TWIF_MASTER_CTRLC =  TWI_MASTER_CMD_STOP_gc; //TWI_MASTER_ACKACT_bm |
	//TWI_MASTER_ACKACT_bm
}

void OPTAddressWrite(void){
	TWIF_MASTER_ADDR = OPT_I2CADDR | OPT_WRITE;			//Send Slave Add Write Bit
	while(!(TWIF_MASTER_STATUS & TWI_MASTER_WIF_bm));	//Wait for Write Int Flag High
}

void OPTAddressRead(void){
	TWIF_MASTER_ADDR = OPT_I2CADDR | OPT_READ;
	TWIF_MASTER_CTRLB |= TWI_MASTER_SMEN_bm;	//enables the smart mode, that sends an ACK after receive a msg
	//while(!(TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm));	//this while must be done when reading
}

void OPTData(uint8_t data_to_OPT){
	TWIF_MASTER_DATA = data_to_OPT;
	while(!((TWIF_MASTER_STATUS & TWI_MASTER_WIF_bm) || (TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm)));
	//it must wait until a byte has been transmitted or received
}

void OPTConfig(uint16_t configurations){
	uint8_t low_byte, high_byte;
	high_byte = (uint8_t)((configurations & 0xff00) >> 8);
	low_byte = (uint8_t)(configurations & 0x00ff);
	
	OPTStart();
	OPTAddressWrite();
	OPTData(CONFIG_REG);
	OPTData(high_byte);
	OPTData(low_byte);
	OPTStop();
}

uint32_t OPTReadIluminance_lux(void){	//returns the lux value*1000 for a better precision.
	uint8_t result1, result2, exponent;
	uint16_t result;
	float lux_float;
	uint32_t lux_32;
	OPTAddressWrite();
	OPTData(RESULT_REG);
	OPTStop();
	OPTAddressRead();
	while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
	result1 = TWIF_MASTER_DATA; //read data
	TWIF_MASTER_CTRLC = TWI_MASTER_CMD_STOP_gc;	//when the next byte is received send an ACK and a STOP action
	while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
	result2 = TWIF_MASTER_DATA;		//read data
	result = (uint16_t)(0x0fff & (uint16_t)(result1<<8|result2));
	exponent = (result1 & 0xf0) >> 4;
	lux_float = ((float)(result * 0.01) * pow(2, exponent));	//convertion for lux units
	lux_32 = (uint32_t)lux_float*1000;
	
	return lux_32;
}

uint16_t OPTReadIluminance(void){	//returns the lux registers
	uint8_t result1, result2;
	uint16_t result;
	OPTAddressWrite();
	OPTData(RESULT_REG);
	OPTStop();
	OPTAddressRead();
	while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
	result1 = TWIF_MASTER_DATA; //read data
	TWIF_MASTER_CTRLC = TWI_MASTER_CMD_STOP_gc;	//when the next byte is received send an ACK and a STOP action
	while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm)); //waits for the reception
	result2 = TWIF_MASTER_DATA;		//read data
	result = ((uint16_t)(result1<<8|result2));
	return result;
}

uint16_t OPTReadID(void){ //it can be used to verify the operationality of the OPT, it must return 0x5449
		uint8_t byte_1, byte_2;
		uint16_t Manuf_ID;
		OPTAddressWrite();
		OPTData(MANUFACTUREID_REG);
		OPTStop();
		OPTAddressRead();
		TWIF_MASTER_CTRLC = TWI_MASTER_CMD_RECVTRANS_gc;
		while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm));
		byte_1 = TWIF_MASTER_DATA;
		TWIF_MASTER_CTRLC = TWI_MASTER_CMD_STOP_gc;
		while(!(TWIF_MASTER_STATUS & TWI_MASTER_RIF_bm));
		byte_2 = TWIF_MASTER_DATA;
		//OPTStop();
		Manuf_ID = (uint16_t)(byte_1<<8) | byte_2;
		
		return Manuf_ID;
}