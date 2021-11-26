/* 
 * File:   kho_ds18b20.h
 * Author: Krzysztof
 *
 * Created on 4 czerwiec 2016, 11:59
 */

#ifndef KHO_DS18B20_H
#define	KHO_DS18B20_H

#ifdef	__cplusplus
extern "C" {
#endif

#define MAX_DS18D20_SENSORS 0x02

#define DS18D20_1WIRE_ID      0x00
#define DS18D20_REAL_VALUE    0x01
#define DS18D20_BCD_VALUE     0x02


#define READ_COMMAND_DS18B20    0x33
#define MATCH_ROM_DS18B20       0x55
#define CONVERT_T_DS18B20       0x44
#define READ_SCRATCH_DS18B20    0xBE

int32_t temperature_ds18d20[MAX_DS18D20_SENSORS][3];

void DS18B20_initSensors(void);

void DS18B20_calculateTemperature(void);

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_DS18B20_H */

