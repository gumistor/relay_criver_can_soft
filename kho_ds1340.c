#if defined(__XC)
    #include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>        /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>    /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "kho_ds1340.h"
#include "kho_i2c.h"

//This function initialize DS1340 module
uint8_t DS1340_init(void)
{
    uint8_t storedConfigData, returnValue, index;

    returnValue = 0;
    //init sensor as working
    sensor_readout_start = READOUT_ON;
    //restore from EEPROM configuration data
    storedConfigData = EEPROM_READ(0x02);
    //restore from EEPROM timeout value
    displayNoTimeTimeout_value = EEPROM_READ(0x03);
    displayTime = DISPLAY_TIME;
    //read current config from ds1340
    DS1340_getConfig();
    //compare if value is equal to stored in eeprom
    if(DS1440_Config_byte != storedConfigData)
    {
        //if value in eeprom is different than in ds1340
        //then value from EEPROM is set to ds1340 and
        //to current config variable
        DS1440_Config_byte = storedConfigData;
        DS1340_setConfig(DS1440_Config_byte, displayNoTimeTimeout_value);
    }
    //read oscilator value
    DS1340_getOscilator();
    //check if oscilator is on
    if(!(DS1440_Oscilator_byte && 0x80))
    {
        //if not, then set the oscilator on and store
        //status in ds1340
        DS1440_Oscilator_byte &= 0x7F;
        DS1340_setOscilator(DS1440_Oscilator_byte);
        //it simply turn on the ds1340 in case it is switched off
    }
    //read and set alarms from eeprom the alarm table to ram, 
    //without writing to eeprom
    for(index=0; index<MAX_ALARM_VALUES; index++)
    {
        DS1340_setAlarm(index,    EEPROM_READ(((2+index)*8)), 
                                    EEPROM_READ(((2+index)*8)+1), 
                                    EEPROM_READ(((2+index)*8)+2), 
                                    EEPROM_READ(((2+index)*8)+3), 
                                    EEPROM_READ(((2+index)*8)+4), 
                                    EEPROM_READ(((2+index)*8)+5), 
                                    EEPROM_READ(((2+index)*8)+6),
                                    EEPROM_READ(((2+index)*8)+7), 
                NOT_SET_EEPROM);
    }
    //init temp value
    tempAlarmValue.action   = 0xFF;
    tempAlarmValue.day      = 0xFF;
    tempAlarmValue.hour     = 0xFF;
    tempAlarmValue.minute   = 0xFF;
    tempAlarmValue.month    = 0xFF;
    tempAlarmValue.second   = 0xFF;
    tempAlarmValue.week     = 0xFF;
    tempAlarmValue.year     = 0xFF;
    //init index to go through        
    indexAlarmValue = MAX_ALARM_VALUES;
        
    return returnValue;
}
//The function to retrive a config byte from the ds1340
void DS1340_getConfig(void) {
    uint8_t DS1340_SET_ADDRESS[2] = {DS1340_WRITE, 0x07};
    uint8_t DS1340_REQUEST_DATA[1] = {DS1340_READ};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};
    
    //set ds1340 eeprom address pointer to 07
    I2C_SendAndReceive(DS1340_SET_ADDRESS,0x02, &DS1340_DATA_RECEIVED, 0x00);
    //request data read
    I2C_SendAndReceive(DS1340_REQUEST_DATA,0x01, &DS1340_DATA_RECEIVED, 0x01);
    //store value in memory
    DS1440_Config_byte = DS1340_DATA_RECEIVED[0];
}
//The function to set config of ds1340
void DS1340_setConfig(uint8_t config, uint8_t timeout) {
    uint8_t DS1340_SET_DATA_ADDRESS[3] = {DS1340_WRITE, 0x07, 0x00};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};

    DS1340_SET_DATA_ADDRESS[2] = config;
    //store configuration byte in EEPROM 
    EEPROM_WRITE(0x02, config);
    //store timeout value in EEPROM 
    EEPROM_WRITE(0x03, timeout);
    //send new config byte to ds1340
    I2C_SendAndReceive(DS1340_SET_DATA_ADDRESS,0x03, 
            &DS1340_DATA_RECEIVED, 0x00);
    //setup new timeout value
    displayNoTimeTimeout_value = timeout;
}
//get oscilator value from dc1340
void DS1340_getOscilator(void) {
    uint8_t DS1340_SET_ADDRESS[2] = {DS1340_WRITE, 0x09};
    uint8_t DS1340_REQUEST_DATA[1] = {DS1340_READ};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};
    //send data addres 09 
    I2C_SendAndReceive(DS1340_SET_ADDRESS,0x02, &DS1340_DATA_RECEIVED, 0x00);
    //request read of byte
    I2C_SendAndReceive(DS1340_REQUEST_DATA,0x01, &DS1340_DATA_RECEIVED, 0x01);
    //put data into memory
    DS1440_Oscilator_byte = DS1340_DATA_RECEIVED[0];
}
//set oscilator value in dc1340
void DS1340_setOscilator(uint8_t config) {
    uint8_t DS1340_SET_DATA_ADDRESS[3] = {DS1340_WRITE, 0x00, 0x00};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};

    DS1340_SET_DATA_ADDRESS[2] = config;
    //send data to the coresponding memory location - different than read
    I2C_SendAndReceive(DS1340_SET_DATA_ADDRESS,0x03, 
            &DS1340_DATA_RECEIVED, 0x00);
}
//function to read time from the ds1340 device
void DS1340_getTime(void) {
    uint8_t DS1340_SET_ADDRESS[2] = {DS1340_WRITE, 0x00}; //init address
    uint8_t DS1340_REQUEST_DATA[1] = {DS1340_READ};       //read request
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};             //data received
    //setup initial address
    I2C_SendAndReceive(DS1340_SET_ADDRESS,0x02, &DS1340_DATA_RECEIVED, 0x00);
    //read the data into structure
    I2C_SendAndReceive(DS1340_REQUEST_DATA,0x01, 
            (uint8_t *)&timeDateStucture, 0x07);
    //align data to get real values
    timeDateStucture.second     &= 0x7F;
    timeDateStucture.minute     &= 0x7F;
    timeDateStucture.hour       &= 0x3F;
    timeDateStucture.week       &= 0x07;
    timeDateStucture.day        &= 0x3F;
    timeDateStucture.month      &= 0x1F;
    timeDateStucture.year       &= 0xFF;
    timeDateStucture.action     &= NO_ACTION;
    //action is not used in case of time variable and is filled with no action
    
    //setup bcd values of time and date
    second_lsn  = timeDateStucture.second&0x0F;
    second_msn  = timeDateStucture.second>>4&0x0F;
    minute_lsn  = timeDateStucture.minute&0x0F;
    minute_msn  = timeDateStucture.minute>>4&0x0F;
    hour_lsn    = timeDateStucture.hour&0x0F;
    hour_msn    = timeDateStucture.hour>>4&0x0F;
    year_lsn    =(timeDateStucture.year) & 0x0F;
    year_msn    =(timeDateStucture.year >> 4) & 0x0F;
    month_lsn   =(timeDateStucture.month) & (0x0F);
    month_msn   =(timeDateStucture.month >> 4) & 0x0F;
    day_lsn     =(timeDateStucture.day) & (0x0F);
    day_msn     =(timeDateStucture.day >> 4) & 0x0F;
    week_lsn    =(timeDateStucture.week) & 0x0F;
}
//This function changes time in ds1340
void DS1340_setTime(uint8_t hour, uint8_t minute, uint8_t second) {
    uint8_t DS1340_SET_DATA[5] = {DS1340_WRITE, 0x00, 0x00, 0x00, 0x00};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};
    //align data to be sent
    DS1340_SET_DATA[2] = 0x7F & second;
    DS1340_SET_DATA[3] = 0x7F & minute;
    DS1340_SET_DATA[4] = 0x3F & hour;
    //send data 
    I2C_SendAndReceive(DS1340_SET_DATA,0x05, &DS1340_DATA_RECEIVED, 0x00);
}
//This function chages date in ds1340
void DS1340_setDate(uint8_t week, uint8_t day, uint8_t month, uint8_t year) {
    uint8_t DS1340_SET_DATA[6] = {DS1340_WRITE, 0x03, 0x00, 0x00, 0x00, 0x00};
    uint8_t DS1340_DATA_RECEIVED[1] = {0xFF};
    //align data to be sent
    DS1340_SET_DATA[2] = 0x07 & week;
    DS1340_SET_DATA[3] = 0x3F & day;
    DS1340_SET_DATA[4] = 0x1F & month;
    DS1340_SET_DATA[5] = year;
    //send data
    I2C_SendAndReceive(DS1340_SET_DATA,0x06, &DS1340_DATA_RECEIVED, 0x00);
}
//This function is calculating if given time/date vector match
//current time and date, values 0xFF are skipped and treated as passed.
//True is returned, when all values different than 0xFF match. If there
//is no other than 0xFF values, then it is set to False.
uint8_t DS1340_checkTime(uint8_t hour, uint8_t minute, uint8_t second,
        uint8_t week , uint8_t day, uint8_t month, uint8_t year)
{
    uint8_t match, not_match;
    uint8_t return_value;
    
    return_value = 0;
    match = 0;
    not_match = 0;

    if(hour != 0xFF)
        if(timeDateStucture.hour != hour)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(minute != 0xFF)
        if(timeDateStucture.minute != minute)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(second != 0xFF)
        if(timeDateStucture.second != second)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(week != 0xFF)
        if(timeDateStucture.week != week)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(day != 0xFF)
        if(timeDateStucture.day != day)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(month != 0xFF)
        if(timeDateStucture.month != month)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(year != 0xFF)
        if(timeDateStucture.year != year)
        {
            not_match++;
        }
        else
        {
            match++;
        }
    if(match > 0 && not_match == 0)
        return_value = 1;
    return return_value;
}
//This function is setting new alarm's values pointed by id value.
//It is possible to write also data into eeprom, if not, then change can
//be considered as temporary - previous value will be restored after reset
void DS1340_setAlarm(uint8_t id, uint8_t hour, uint8_t minute, uint8_t second,
        uint8_t week , uint8_t day, uint8_t month, uint8_t year, 
        uint8_t action, uint8_t updateEE)
{
    //store data to the table of alarms
    alarmList[id].action = action;
    alarmList[id].year = year;
    alarmList[id].month = month;
    alarmList[id].day = day;
    alarmList[id].week = week;
    alarmList[id].second = second;
    alarmList[id].minute = minute;
    alarmList[id].hour = hour;
    //in case flag is set, then the values are also stored in eeprom
    if(updateEE)
    {
        EEPROM_WRITE(((2+id)*8),hour); 
        EEPROM_WRITE(((2+id)*8)+1,minute);
        EEPROM_WRITE(((2+id)*8)+2,second); 
        EEPROM_WRITE(((2+id)*8)+3,week); 
        EEPROM_WRITE(((2+id)*8)+4,day); 
        EEPROM_WRITE(((2+id)*8)+5,month);
        EEPROM_WRITE(((2+id)*8)+6,year);
        EEPROM_WRITE(((2+id)*8)+7,action);
    }
}
//This function when called is matching all stored alarms with 
//current date and time. It returns action value when alarm is
//equal to current time.
uint8_t DS1340_matchTime(void)
{
    uint8_t index, returnValue;
    //set default return value as no action
    //this value will change when time match alarm entry
    returnValue = NO_ACTION;
    
    for(index=0; index<MAX_ALARM_VALUES; index++)
    {
        //checks all alarms and compare it with current time,
        //in case of matching alarm and time, the action value is set
        //as return value
        if(DS1340_checkTime(alarmList[index].hour, alarmList[index].minute, 
                alarmList[index].second, alarmList[index].week , 
                alarmList[index].day, alarmList[index].month, 
                alarmList[index].year))
        {
            //retur action to be performed
            returnValue = alarmList[index].action;
        }
    }
    //Value returned is eqal to action to be performed only if 
    //action should take place 
    return returnValue;
}
