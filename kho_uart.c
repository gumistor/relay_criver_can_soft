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

#include "kho_uart.h"

/*
 * 
 */

uint8_t initUART(void)
{
    uint8_t returnValue;

    returnValue = 0;

    TRISC |=  0b10000000;
    TRISC &= ~0b01000000;

    PIR1 |=  0b00100000;
    PIE1 &= ~0b00100000;
    IPR1 &= ~0b00100000;

    SPBRGH = 0x00;
    SPBRG  = 25;

    BAUDCON = 0x02;

    TXSTA = 0x00;
    RCSTA = 0x90;

    dataUARTreceivedFlag = 0;

    PIE1 |=  0b00100000;

    return returnValue;
}

uint8_t receiveUARTdata(uint8_t *data)
{
    uint8_t returnValue;
    returnValue = 0;
    if(dataUARTreceivedFlag)
    {
        dataUARTreceivedFlag = 0;
        returnValue = 1;
        *data = dataUARTreceived;
    }
    return returnValue;
}