/*
 * HDC.h
 *
 * Created: 18-Mar-16 18:31:24
 *  Author: Felipe Schneider
 */ 


#ifndef HDC_H_
#define HDC_H_
#include <avr/io.h>
#define HDC_I2CADDR       0x80	//for the both pins tied to ground (observe that here it is consider an 8bit register)
#define HDC_TEMP_REG          0x00
#define HDC_HUMID_REG         0x01
#define HDC_CONFIG_REG        0x02
#define HDC_CONFIG_RST    (1 << 15)
#define HDC_CONFIG_HEAT   (1 << 13)
#define HDC_CONFIG_TEMP_HUM   (1 << 12)
#define HDC_CONFIG_BATT   (1 << 11)
#define HDC_CONFIG_TRES_14  0
#define HDC_CONFIG_TRES_11  (1 << 10)
#define HDC_CONFIG_HRES_14  0
#define HDC_CONFIG_HRES_11  (1 << 8)
#define HDC_CONFIG_HRES_8   (1 << 9)

#define HDC_SERIAL1       0xFB
#define HDC_SERIAL2       0xFC
#define HDC_SERIAL3       0xFD
#define HDC_MANUFID       0xFE
#define HDC_DEVICEID      0xFF		//0x1050

#define HDC_WRITE	0x00
#define HDC_READ	0x01

void IniI2C_HDC(void);
void HDCStart(void);
void HDCStop(void);
void HDCAddressWrite(void);
void HDCAddressRead(void);
void HDCData(uint8_t data_to_HDC);
void ConfigHDC(void);
void HDCTrig(void);
uint16_t HDCReadData(void);
// float ReadHDCTemperature(void);
// float ReadHDCHumidity(void);
#endif /* HDC_H_ */