/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
    #include <xc.h>        /* XC8 General Include File */
#elif defined(HI_TECH_C)
    #include <htc.h>       /* HiTech General Include File */
#elif defined(__18CXX)
    #include <p18cxxx.h>   /* C18 General Include File */
#endif

#if defined(__XC) || defined(HI_TECH_C)

#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */

#endif

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */
#include "kho_ds1340.h"
#include "kho_can.h"
#include "kho_relays.h"
#include "kho_dev_config.h"

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void doRelaysStatusFrame(void)
{
    configureTXbuffer(TX0BUFFER, 0x05000000         | 
            can_node_class<<8 |    device_id,1);
    sendTXbuffer(TX0BUFFER, relayCurrentStatus[0] | relayTemporaryStatus[0]<<4, 
                            relayCurrentStatus[1] | relayTemporaryStatus[1]<<4, 
                            relayCurrentStatus[2] | relayTemporaryStatus[2]<<4, 
                            relayCurrentStatus[3] | relayTemporaryStatus[3]<<4, 
                            relayCurrentStatus[4] | relayTemporaryStatus[4]<<4, 
                            relayCurrentStatus[5] | relayTemporaryStatus[5]<<4,
                            relayCurrentStatus[6] | relayTemporaryStatus[6]<<4,
                            relayCurrentStatus[7] | relayTemporaryStatus[7]<<4,
            0x08);
}

void doGetListFrames(uint8_t index)
{
    configureTXbuffer(TX0BUFFER, 0x05000000         | 
            can_node_class<<8 |    device_id,1);
    sendTXbuffer(TX0BUFFER, 0x20, 
                            index, 
                            alarmList[index].day, 
                            alarmList[index].month, 
                            alarmList[index].year, 
                            0xFF,
                            0xFF,
                            0xFF,
            0x05);
    sendTXbuffer(TX0BUFFER, 0x20, 
                            index, 
                            alarmList[index].hour, 
                            alarmList[index].minute, 
                            alarmList[index].second, 
                            alarmList[index].week,
                            alarmList[index].action,
                            0xFF,
            0x07);
}

void doSensorFrame(void)
{
    configureTXbuffer(TX0BUFFER, CAN_ADDR_SENSOR         | 
            can_node_class<<8 |    device_id,1);
    sendTXbuffer(TX0BUFFER, 0x00, timeDateStucture.hour, 
            timeDateStucture.minute, timeDateStucture.second,
            timeDateStucture.week, timeDateStucture.day, timeDateStucture.month,
            timeDateStucture.year,0x08);
}

void doDisplayFrame(void)
{
    static uint8_t dots_value = 0x00;
    static uint8_t last_second_lsn;

    if(second_lsn & 0x01) dots_value = 0x14;
    else dots_value = 0x00;

    if(last_second_lsn != second_lsn)
    {
        configureTXbuffer(TX1BUFFER, CAN_ADDR_DISPLAY         | 
                can_node_class<<8 |    device_id,1);
        if(displayTime == DISPLAY_TIME)
        {
            sendTXbuffer(TX1BUFFER, second_lsn,second_msn,minute_lsn,minute_msn,
                hour_lsn,hour_msn,dots_value,0xFF,0x08);
        }
        else if(displayTime == DISPLAY_DATE)
        {
            sendTXbuffer(TX1BUFFER, year_lsn, year_msn, month_lsn, month_msn,
                day_lsn,day_msn,0x14,0xFE,0x08);
        }
        else
        {
            sendTXbuffer(TX1BUFFER, week_lsn, 0xFF, 0xFF,0xFF,0xFF,0xFF,0x02,
                    0xFD,0x08);
        }
    }
    last_second_lsn = second_lsn;
}


void doCAN(void)
{
    uint16_t i;
    uint8_t devId;
    uint8_t devCommand;
    
    if(CAN_data_received_flag == CAN_DATA_FULL)
    {
        switch(CAN_data_received.CAN_address & 0x1FFF0000)
        {
            case CAN_ADDR_ACTUATOR:
                //no action is needed
                if(CAN_data_received.CAN_data[0] < 0xAA)
                {
                    devId       = (CAN_data_received.CAN_data[0] & 0xF0);
                    devCommand  = (CAN_data_received.CAN_data[0] & 0x0F);
                
                    evaluateCommand(devId, devCommand,
                            CAN_data_received.CAN_data[1]<<8 | 
                            CAN_data_received.CAN_data[2]);
                }
                else if(CAN_data_received.CAN_data[0] == 0xAA)
                {
                    evaluateCommand(1 << 4, 0x03,15);
                }
                else if(CAN_data_received.CAN_data[0] == 0xBB)
                {
                    evaluateCommand(0 << 4, 0x02,3*3600);
                }
                else
                {
                    //
                }
            break;
            case CAN_ADDR_DISPLAY:
                //no action is needed
            break;
            case CAN_ADDR_SENSOR:
                if(!(CAN_data_received.CAN_address & 0x0000FFFF))
                {
                    if((CAN_data_received.CAN_data[7]!= timeDateStucture.year)
                     ||(CAN_data_received.CAN_data[6]!= timeDateStucture.month)
                     ||(CAN_data_received.CAN_data[5]!= timeDateStucture.day)
                     ||(CAN_data_received.CAN_data[4]!= timeDateStucture.week))
                    {
                        DS1340_setDate(CAN_data_received.CAN_data[4],
                                CAN_data_received.CAN_data[5],
                                CAN_data_received.CAN_data[6],
                                CAN_data_received.CAN_data[7]);
                    }   
                    
                    if((CAN_data_received.CAN_data[1]!= timeDateStucture.minute)
                     ||(CAN_data_received.CAN_data[0]!= timeDateStucture.hour))
                    {
                        DS1340_setTime(CAN_data_received.CAN_data[0],
                                CAN_data_received.CAN_data[1],
                                CAN_data_received.CAN_data[2]);
                    }
                }
            break;
            case CAN_ADDR_CONFIG_REQ:
                switch(CAN_data_received.CAN_data[0])
                    {
                    case 0x00: //setup time in DS1340 RTC
                        DS1340_setTime(CAN_data_received.CAN_data[1],
                                CAN_data_received.CAN_data[2],
                                CAN_data_received.CAN_data[3]);
                        break;
                    case 0x01: //setup date in DS1340 RTC
                        DS1340_setDate(CAN_data_received.CAN_data[1],
                                CAN_data_received.CAN_data[2],
                                CAN_data_received.CAN_data[3],
                                CAN_data_received.CAN_data[4]);
                        break;
                    case 0x02: //setup config byte of DS1340
                        DS1340_setConfig(CAN_data_received.CAN_data[1],
                                CAN_data_received.CAN_data[2]);
                        break;
                    case 0x03:  // setup oscilator to work of DS1340
                        DS1340_setOscilator(0x00);
                        break;
                    case 0x04:
                        if(displayTime == DISPLAY_TIME)
                        {
                            displayTime = DISPLAY_DATE;
                            displayNoTimeTimeout = displayNoTimeTimeout_value;
                        }
                        else if(displayTime == DISPLAY_DATE)
                        {
                            displayTime = DISPLAY_WEEK;
                            displayNoTimeTimeout = displayNoTimeTimeout_value;
                        }
                        else
                        {
                            displayTime = DISPLAY_TIME;
                        }
                        break;
                    case 0x10:
                        //DS1340_setCurrentValue(CAN_data_received.CAN_data[1]);
                        tempAlarmValue.year     = 0xFF;
                        tempAlarmValue.month    = 0xFF;
                        tempAlarmValue.day      = 0xFF;    
                        if(CAN_data_received.CAN_DLC > 0x02)
                        {
                            tempAlarmValue.day = CAN_data_received.CAN_data[2];
                            if(CAN_data_received.CAN_DLC > 0x03)
                            {
                                tempAlarmValue.month = CAN_data_received.CAN_data[3];
                                if(CAN_data_received.CAN_DLC > 0x04)
                                {
                                    tempAlarmValue.year = CAN_data_received.CAN_data[4];
                                }
                            }
                        }
                        break;
                    case 0x11:
                        //DS1340_setCurrentValue(CAN_data_received.CAN_data[1]);
                        DS1340_setAlarm(CAN_data_received.CAN_data[1], 
                                CAN_data_received.CAN_data[2], 
                                CAN_data_received.CAN_data[3], 
                                CAN_data_received.CAN_data[4],
                                CAN_data_received.CAN_data[5] , 
                                tempAlarmValue.day, tempAlarmValue.month, 
                                tempAlarmValue.year, 
                                CAN_data_received.CAN_data[6], 1);
                        tempAlarmValue.year     = 0xFF;
                        tempAlarmValue.month    = 0xFF;
                        tempAlarmValue.day      = 0xFF;    
                        break;
                    case 0x20:
                        indexAlarmValue = 0;
                        break;
                    case 0x30:
                        doRelaysStatusFrame();
                        break;
                    case 0xFF:
                        EEPROM_WRITE(0xFF,0xFF);
                        for(i=0; i<2000; i++);
                        Reset();
                        break;
                    default:    // no action
                        break;

                    }
                break;
            case CAN_ADDR_REQUEST:

            break;
            case CAN_ADDR_RESPONSE:

            break;
            case CAN_ADDR_NM:
                //nm frame received
                //clear wait to sleep frame
                //configureTXbuffer(TX2BUFFER, CAN_ADDR_CONFIG_RES         | can_node_class<<8 |    device_id,1);
                //sendTXbuffer(TX2BUFFER, 0x07, CAN_data_received.CAN_address>>24,CAN_data_received.CAN_address>>16,CAN_data_received.CAN_address>>8,CAN_data_received.CAN_address,CAN_data_received.CAN_DLC,
                //CAN_data_received.CAN_data[0],CAN_data_received.CAN_data[1],0x08);
            break;
            default:
                ;
        }
        CAN_data_received_flag = CAN_DATA_EMPTY;
    }
}

void main(void)
{
    uint16_t old_systemTick;
    uint8_t value;
    uint8_t action;
    //initialization
    ConfigureOscillator();
    InitApp();
    
    value = 0;
    action = 0;
    while(1) {

        if(READOUT_ON == sensor_readout_start)
        {
            doERRORS();
            doDisplayFrame();   //send display data on change
            
            if(systemTick - old_systemTick > 0)
            {
                //every single tick, every 125 ms
                if(systemTick & 0x0001)
                {
                   //every two signal ticks, every 250 ms
                   DS1340_getTime();   //time readout
                }
                if((systemTick & 0x0003) == 0x0002)
                {
                    action = DS1340_matchTime();
                    if(action < 0xAA)
                        evaluateCommand((action & 0xF0), (action & 0x0F),0x0000);
                    if(action == 0xAA)
                        evaluateCommand(1 << 4, 0x03,15);
                    if(action == 0xBB)
                        evaluateCommand(0 << 4, 0x02,3*3600);
                    if(doRelays())
                    {
                        doRelaysStatusFrame();
                    }
                    //every four signal ticks, every 500ms
                    doSensorFrame();    //send current time and date
                    
                    if(indexAlarmValue < MAX_ALARM_VALUES)
                    {
                        doGetListFrames(indexAlarmValue);
                        indexAlarmValue++;
                    }
                }
                if((systemTick & 0x0007) == 0x0004)
                {
                    doNM(0x01);

                    if(displayNoTimeTimeout)
                    {
                        displayNoTimeTimeout--;
                    }
                    else
                    {
                        displayTime = DISPLAY_TIME;
                    }
                }
                old_systemTick = systemTick;
            }
            else
            {
                //error, lost tick
            }
            doCAN();
        }
        else
        {
            //do nothing
        }
    }
}

