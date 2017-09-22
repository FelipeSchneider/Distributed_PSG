/*
 * OPT.h
 *
 * Created: 18-Mar-16 18:31:58
 *  Author: Felipe Schneider
 */ 


#ifndef OPT_H_
#define OPT_H_
#include <avr/io.h>
#include <math.h>

#define OPT_I2CADDR       0x88	//for the pin tied to ground (observe that here it is consider an 8bit register)
#define OPT_WRITE	0x00
#define OPT_READ	0x01

#define RESULT_REG 0x00
#define CONFIG_REG 0x01
#define LOWLIMIT_REG 0x02
#define HIGHLIMIT_REG 0x03
#define MANUFACTUREID_REG 0x7E
#define DEVICEID_REG 0x7F

#define DEFAULT_CONFIG_800    0b1100110000010000 // 800ms
#define DEFAULT_CONFIG_800_OS 0b1100101000010000 // 800ms, one shot

#define DEFAULT_CONFIG_100    0b1100010000010000 // 100ms
#define DEFAULT_CONFIG_100_OS 0b1100001000010000 // 100ms, one shot

void IniI2C_OPT(void);
void OPTStart(void);
void OPTStop(void);
void OPTAddressWrite(void);
void OPTAddressRead(void);
void OPTData(uint8_t data_to_OPT);
void OPTConfig(uint16_t configurations);
uint32_t OPTReadIluminance_lux(void);
uint16_t OPTReadIluminance(void);
uint16_t OPTReadID(void);

#endif /* OPT_H_ */