/* 
 * File:   sensorsFunctions.h
 * Author: Krzysztof
 *
 * Created on 29 stycze? 2016, 20:09
 */

#ifndef SENSORSFUNCTIONS_H
#define	SENSORSFUNCTIONS_H

#ifdef	__cplusplus
extern "C" {
#endif

//Address of DS1340.
#define DS1340_I2C_ID        0x68 << 1
//Definition of write and read bytes
//for I2C purpose
#define DS1340_WRITE    DS1340_I2C_ID | 0x00
#define DS1340_READ     DS1340_I2C_ID | 0x01


#define READOUT_SLEEP   0x00
#define READOUT_ON      0x01
#define READOUT_OFF     0x02
#define READOUT_ERROR   0x03

//Definition of values that should be presented
//in display frame
#define DISPLAY_TIME    0x00    //show time
#define DISPLAY_DATE    0x01    //show data 
#define DISPLAY_WEEK    0x02    //show day name/value
    
//Definition of number of alarms that can be used
//By default it is set to 10 and is limited by EEPROM size
#define MAX_ALARM_VALUES    0x0A
//Value set, when no action is needed
#define NO_ACTION       0xFF
//Definition to store or not alarm data to eeprom
#define NOT_SET_EEPROM  0
#define SET_EEPROM      1
    
//structure representing time and date
struct timeDateStucture
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t week;
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t action;
} tempAlarmValue, timeDateStucture, alarmList[MAX_ALARM_VALUES];
//value containing current cofiguration byte
volatile uint8_t DS1440_Config_byte;
//value containing current oscilator value
volatile uint8_t DS1440_Oscilator_byte;
//value containing current display status
volatile uint8_t displayTime;
//value counter
volatile uint8_t displayNoTimeTimeout;
//value containing current counter value
volatile uint8_t displayNoTimeTimeout_value = 5;
//sensor ready flag
volatile uint8_t sensor_readout_start = READOUT_ON;
//index of current pointed alarm
volatile uint8_t indexAlarmValue;
//bcd values of time and date
volatile uint8_t second_lsn;
volatile uint8_t second_msn;
volatile uint8_t minute_lsn;
volatile uint8_t minute_msn;
volatile uint8_t hour_lsn;
volatile uint8_t hour_msn;
volatile uint8_t year_lsn;
volatile uint8_t year_msn;
volatile uint8_t month_lsn;
volatile uint8_t month_msn;
volatile uint8_t day_lsn;
volatile uint8_t day_msn;
volatile uint8_t week_lsn;

//initialize ds1340 module
uint8_t DS1340_init(void);
//get and set configuration bites from device
void DS1340_getConfig(void);
void DS1340_setConfig(uint8_t, uint8_t);
//get and set oscillator activities 
void DS1340_getOscilator(void);
void DS1340_setOscilator(uint8_t);
//get current time and date
void DS1340_getTime(void);
//set current time
void DS1340_setTime(uint8_t, uint8_t, uint8_t);
//set current date
void DS1340_setDate(uint8_t, uint8_t, uint8_t, uint8_t);
//function to check values with time
uint8_t DS1340_checkTime(uint8_t, uint8_t, uint8_t, 
        uint8_t, uint8_t, uint8_t, uint8_t);
//function to match all alarms with current time
uint8_t DS1340_matchTime(void);
//function to set up alarm
void DS1340_setAlarm(uint8_t, uint8_t, uint8_t, uint8_t,
        uint8_t , uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* SENSORSFUNCTIONS_H */

//volatile uint8_t currentAlarmValue;