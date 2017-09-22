/*
 * data_SD.c
 *
 * Created: 18-Mar-16 14:33:45
 *  Author: Felipe Schneider
 */ 


#ifndef DATA_SD_H_
#define DATA_SD_H_

uint8_t ler_imagem(char* nome, uint8_t image_handler);
uint8_t StartRecording(void);
uint8_t OpenSD(void);
uint8_t CloseSD(void);
void OpenFile(char *path);
void WriteOnBuffer(uint8_t buffer_indicator, uint16_t offset, uint16_t data);
void CleanGlobalCounters(void);
void MakeLog(void);
void MakeNewFile(uint8_t hour_name);

#endif /* DATA_SD_H_ */