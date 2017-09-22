/*
 * data_SD.c
 *
 * Created: 18-Mar-16 14:32:33
 *  Author: Felipe Schneider
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include "defines_global.h"
#include "uSD_SPI.h"
#include "diskio.h"
#include "ff.h"
#include "MMC.h"
#include "Variables.h"


FATFS fs;				 // Work area (file system object) for logical drives 
FIL f_origem;			 // File objects 
FRESULT fr;				 // FatFs function common result code 
UINT br, bw;			 // File read/write count 
uint8_t *buf_point;
uint16_t block_size, n_block, block_cont, last_block;

uint8_t OpenSD(void){
	fr = disk_initialize(SD_CARD);
	if(fr){
		//TODO:: FAZER ALGUMA COISA COM UM ERRO DE inicialização
	}
	/* Register work area for each logical drive */
	fr = f_mount(&fs, "", 0);
	if(fr){
		//TODO:: FAZER ALGUMA COISA COM UM ERRO DE mount
	}
	return fr;
}

uint8_t CloseSD(void){
	fr = f_mount(NULL, "", 0);
	if(fr){
		//TODO:: FAZER ALGUMA COISA COM UM ERRO DE FECHAMENTO
	}
	return fr;
}

void OpenFile(char *path){
	f_open(&f_origem, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
}

uint8_t StartRecording(void){	//this try to find a name of a file that does not exist (up to 9 files)
	//in the case that all 9 files exists, the file 1 will be updated.
  char file_name[] = "EOGP1H01.RAW";	
  for (uint8_t j=1;j<=9;j++)  {
	  g_patient_number = j;
	  file_name[4] = j + ZERO_ASCII;
	  g_error = f_open(&f_origem, file_name, FA_CREATE_NEW | FA_READ | FA_WRITE);
	  if (g_error != FR_EXIST) {	//in the case the file exist, it will return the error FR_EXIST
		  break;
	  }
	  
	  if(g_patient_number == 9){	//in the case all nine files exists, create a new one with the name EOGP1H01.RAW
		  file_name[4] = 1 + ZERO_ASCII;
		  g_patient_number = 1;
		  f_unlink(file_name);	//deleting file if it does exists before
		  g_error = f_open(&f_origem, file_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		  break;
	  }
  }
   //TODO:: fazer algo com estes errors
  return g_error;
}

void MakeNewFile(uint8_t hour_name){
	uint8_t dec, un;
	dec = hour_name/10;
	un = hour_name-10*dec;
	g_error = f_close(&f_origem);
	//TODO:: do something with those errors
	char file_name[] = "EOGP1H00.RAW";	//1: main board, 0: indicates the hour -> starting at 1.
	file_name[4] = g_patient_number + ZERO_ASCII;
	file_name[6] = dec+ZERO_ASCII;
	file_name[7] = un+ZERO_ASCII;
	f_unlink(file_name);	//deleting file if it does exists before
	//TODO:: fazer algo com estes errors
	g_error = f_open(&f_origem, file_name, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
}


void MakeLog(void){
	uint16_t crc_main = 0xffff;
	if (gf_buffer_indicator == 1){	//if this flag is 1 we must send the g_buffer_uSD_2
		for (uint16_t i=0;i<BUFFER_SIZE-2;i++)	{	//calculating the CRC
			crc_main = (uint16_t)(crc_main + g_buffer_uSD_2[i]);
		}
		g_buffer_uSD_2[BUFFER_SIZE-1] = lo8(crc_main);
		g_buffer_uSD_2[BUFFER_SIZE-2] = hi8(crc_main);
		
		g_error = f_lseek(&f_origem, f_size(&f_origem));
		//if(g_error) 
		//TODO:: do something with those errors
		g_error = f_write(&f_origem, g_buffer_uSD_2, BUFFER_SIZE, &br);
		//if(g_error)
		g_error = f_sync(&f_origem);
	}
	else{ //if gf_buffer_indicator is 0 we must send the g_buffer_uSD
		for (uint16_t i=0;i<BUFFER_SIZE-2;i++)	{//calculating the CRC
			crc_main = (uint16_t)(crc_main + g_buffer_uSD[i]);
		}
		g_buffer_uSD[BUFFER_SIZE-1] = lo8(crc_main);
		g_buffer_uSD[BUFFER_SIZE-2] = hi8(crc_main);
		
		g_error = f_lseek(&f_origem, f_size(&f_origem));
		//if(g_error)
		//TODO:: do something with this error
		g_error = f_write(&f_origem, g_buffer_uSD, BUFFER_SIZE, &br);
		//if(g_error)
		g_error = f_sync(&f_origem);
	}//close else	
}

void WriteOnBuffer(uint8_t buffer_indicator, uint16_t offset, uint16_t data){
	uint8_t byte_1, byte_2;
	byte_1 = (uint8_t)((data & 0xFF00)>>8);
	byte_2 = (uint8_t)(data & 0x00FF);
  
	if (buffer_indicator){
  		g_buffer_uSD[offset] = byte_1;
  		g_buffer_uSD[offset+1] = byte_2;
	}
	else{
  		g_buffer_uSD_2[offset] = byte_1;
  		g_buffer_uSD_2[offset+1] = byte_2;
	}
}  

void CleanGlobalCounters(void){
	gc_acc = 0;
	gc_eog = 0;
	gc_bat = 0;
	gc_audio = 0;
	gc_opt = 0;
	gc_ldr = 0;
}

//TODO:: fazer a adaptação da rotina do uSD para o sistema da placa principal.
/*
void write_1_sec(void){
	fr = f_lseek(&f_origem, f_size(&f_origem));
	fr = f_write(&f_origem, data_SD, BYTES_SEND_SD, &br);  / * Read a chunk of source file * /
}



void make_log(void){
  send_sd = 20;
  uint8_t erro;
  if(control_buffer_sd){
    erro = f_lseek(&f_origem, f_size(&f_origem));
    //if(erro) Tela_Erro;
    erro = f_write(&f_origem, data_SD, BYTES_SEND_SD, &br);
    //if(erro) Tela_Erro;
    erro = f_sync(&f_origem);
    //if(erro){Tela_Erro;}
    data_SD[BUFFER_SD_SIZE-1] = 0;
  }
  else{
    erro = f_lseek(&f_origem, f_size(&f_origem));
    //if(erro) Tela_Erro;
    erro = f_write(&f_origem, data_SD_b2, BYTES_SEND_SD, &br);
    //if(erro) Tela_Erro;
    erro = f_sync(&f_origem);
    //if(erro) Tela_Erro;
    data_SD_b2[BUFFER_SD_SIZE-1] = 0;
  }//fim else
}
*/