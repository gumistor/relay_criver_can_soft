/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

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
#include "kho_relays.h"

#endif

//Set up PORTS to enable pairs of relays
uint8_t pairStatus(uint8_t pair, uint8_t onOff)
{
    uint8_t returnValue; 
    //setup return value to default
    returnValue = 0; 
    //check if pair is in scope
    if(pair < NO_OF_RELAYS)
    {
        if(onOff == 0)
        {
            //check current status acording to current PORTs setup
            //return if something is not ok
            if((relayPORTAtable[pair]!=0x00) && 
                    ((PORTA & relayPORTAtable[pair]) == 0) ||
               (relayPORTBtable[pair]!=0x00) && 
                    ((PORTB & relayPORTBtable[pair]) == 0) ||
               (relayPORTCtable[pair]!=0x00) && 
                    ((PORTC & relayPORTCtable[pair]) == 0)
                    )
            {
                returnValue = 1;
            }
            //change all values
            LATA |= relayPORTAtable[pair];
            LATB |= relayPORTBtable[pair];
            LATC |= relayPORTCtable[pair];
        }
        else
        {
            //check current status acording to current PORTs setup
            //return if something is not ok
            if((relayPORTAtable[pair]!=0x00) && 
                    ((PORTA & relayPORTAtable[pair]) == relayPORTAtable[pair]) ||
               (relayPORTBtable[pair]!=0x00) && 
                    ((PORTB & relayPORTBtable[pair]) == relayPORTBtable[pair]) ||
               (relayPORTCtable[pair]!=0x00) && 
                    ((PORTC & relayPORTCtable[pair]) == relayPORTCtable[pair])
                    )
            {
                returnValue = 1;
            }
            //change all values
            LATA &= ~relayPORTAtable[pair];
            LATB &= ~relayPORTBtable[pair];
            LATC &= ~relayPORTCtable[pair];
        }
    }
    return returnValue;    
}

//Function to initialize all relays
uint8_t relaysInit(void)
{
    uint8_t relayIndex, returnValue;
    //asign return value with default
    returnValue = 0;
    //initialize all relays with default values
    for(relayIndex = 0; relayIndex<NO_OF_RELAYS; relayIndex++)
    {
        //turn off indexed device
        pairStatus(relayIndex,0x00);                   
        //set current status as off of indexed device
        relayCurrentStatus[relayIndex] = DEVICE_OFF;
        //set temporaray status as off of indexed device
        relayTemporaryStatus[relayIndex] = DEVICE_OFF;
        //set timer to 0 of indexed device
        relayTemporaryTimer[relayIndex]  = 0x0000;
    }
    return returnValue;
}

uint8_t evaluateCommand(uint8_t devID, uint8_t devCommand, uint16_t devTimer)
{
    uint8_t returnValue;
    //setup retrun value to default 
    returnValue = 0;
    //check if device is in the scope
    if((devID >> 4) < NO_OF_RELAYS)
    {
        //check commands
        switch(devCommand & 0x0E)
        {
            case DEVICE_SWITCH: //switch requested
                relayCurrentStatus[devID >> 4] = devCommand & 0x01;
                //reset temporary status
                relayTemporaryStatus[devID >> 4] = DEVICE_OFF;
                //reset temporary timer
                relayTemporaryTimer[devID >> 4]  = 0x0000;
                //any inhibition is stopped after switch request
            break;
            case DEVICE_TIME:   //timer requested
                if(relayTemporaryTimer[devID >> 4] == 0)
                {
                    //set temporary value
                    relayTemporaryStatus[devID >> 4] = devCommand & 0x01;
                    //set temporary timer
                    relayTemporaryTimer[devID >> 4]  = devTimer<<1;
                }
            break;
            default:
            break;
        }
    }
    return returnValue;
}

//function to evaluate every needed tick
uint8_t doRelays(void)
{
    uint8_t returnValue;
    uint8_t relayIndex;
    //setup return value to default
    returnValue = 0;
    //check status of all relays
    for(relayIndex = 0; relayIndex<NO_OF_RELAYS; relayIndex++)
    {
        //if timier is enabled, then decreates it and set temporary status
        //in case of timeout or no timer set to current state
        if(relayTemporaryTimer[relayIndex] > 0)
        {
            relayTemporaryTimer[relayIndex]--;
            returnValue = pairStatus(relayIndex, relayTemporaryStatus[relayIndex]);
        }
        else
        {
            returnValue = pairStatus(relayIndex, relayCurrentStatus[relayIndex]);
            relayTemporaryStatus[relayIndex] = 0;
        }
    }
    return returnValue;
}


