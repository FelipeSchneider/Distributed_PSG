/*
 * HDC.c
 *
 * Created: 18-Mar-16 18:31:04
 *  Author: Felipe Schneider
 */ 

//TODO:: TESTAR a biblioteca do HDC1000

#include "HDC.h"
#include "defines_global.h"
#include "Variables.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//endereço do HDC1050: 0x80
	
void IniI2C_HDC(void){
	PORTE_DIRCLR = DRDY_HDC;
	PORTE_PIN0CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups
	PORTE_PIN1CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups
	PORTE_PIN2CTRL = PORT_OPC_PULLUP_gc;			//activates the pull ups
	
	TWIE_MASTER_CTRLB = TWI_MASTER_SMEN_bm;	//Habilita o Quick command e smart mode
	
	TWIE_MASTER_BAUD = 0x35;//35;	//35 FOR 400kHz and 0x98 for 100kHz TWI Frequency
	TWIE_MASTER_CTRLA = TWI_MASTER_ENABLE_bm; //| TWI_MASTER_WIEN_bp | TWI_MASTER_RIEN_bm;
	TWIE_MASTER_STATUS |= TWI_MASTER_BUSSTATE_IDLE_gc;
	TWIE_MASTER_CTRLC = 0x00;
}

void HDCStart(void){
	//TWIE_MASTER_CTRLC &= 0xFC;	//mask
	TWIE_MASTER_CTRLC = 0x01;	//starts
}

void HDCStop(void){
	TWIE_MASTER_CTRLC = TWI_MASTER_CMD_STOP_gc; //TWI_MASTER_ACKACT_bm 
}


void HDCAddressWrite(void){
	TWIE_MASTER_ADDR = HDC_I2CADDR | HDC_WRITE;			//Send Slave Add Write Bit
	while(!(TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm));	//Wait for Write Int Flag High
}

void HDCAddressRead(void){
	TWIE_MASTER_ADDR = HDC_I2CADDR | HDC_READ;
	TWIE_MASTER_CTRLB |= TWI_MASTER_SMEN_bm;	//enables the smart mode, that sends an ACK after receive a msg 
	//while(!((TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm) || (TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)));
}

void HDCData(uint8_t data_to_HDC){
	TWIE_MASTER_DATA = data_to_HDC;
	while(!((TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm) || (TWIE_MASTER_STATUS & TWI_MASTER_RIF_bm)));
}


void ConfigHDC(void){
	uint16_t config;
	config = HDC_CONFIG_TEMP_HUM | HDC_CONFIG_TRES_14 | HDC_CONFIG_HRES_14;
	HDCStart();
	HDCAddressWrite();
	HDCData(HDC_CONFIG_REG);
	HDCData(hi8(config));
	HDCData(lo8(config));
	HDCStop();
}

//The HDC always perform a temperature measurement  first (if the temperature measurement is active).
void HDCTrig(void){
	HDCStart();
	HDCAddressWrite();
	HDCData(HDC_TEMP_REG);
//	HDCStop();
}
	
uint16_t HDCReadData(void){
	uint16_t result;
	uint8_t aux1,aux2;
//	HDCStart();
	HDCAddressRead();
	aux1 = TWIE_MASTER_DATA;
	HDCAddressRead();
	aux2 = TWIE_MASTER_DATA;
	result = (aux1<<8)|aux2;
	return result;
	//HDCStop();
}



/*
uint16_t ReadHDCTemperature(void){
	uint16_t result;
	uint8_t aux1,aux2;
// 	HDCAddressWrite();
// 	HDCData(HDC_TEMP_REG);
	HDCAddressRead();
	aux1 = TWIE_MASTER_DATA;
	HDCAddressRead();
	aux2 = TWIE_MASTER_DATA;
// 	temp = (aux1<<8)+aux2;
// 	temp /= 65536;
// 	temp *= 165;
// 	temp -= 40;
	result = (aux1<<8)|aux2;
	return result;
}*/

/*
uint16_t ReadHDCHumidity(void){
	uint16_t result;
	uint8_t aux1,aux2;
// 	HDCAddressWrite();
// 	HDCData(HDC_HUMID_REG);
	HDCAddressRead();
	while(!(TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm));
	aux1 = TWIE_MASTER_DATA;
	HDCAddressRead();
	while(!(TWIE_MASTER_STATUS & TWI_MASTER_WIF_bm));
	aux2 = TWIE_MASTER_DATA;
// 	humid = (aux1<<8)+aux2;
// 	humid /= 65536;
// 	humid *= 100;
	result = (aux1<<8)|aux2;
	return result;
}*/