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
#include "kho_pms3003.h"

/*
 *
 */

uint8_t initPMS3003(void)
{
    uint8_t returnValue;

    returnValue = 0;
    
    initUART();

    TRISC |= 0b00100000;

    PORTC |= 0b00100000;

    return returnValue;
}

uint8_t pms3003_gather_data(void)
{
    static uint8_t dataReceived;
    static uint8_t dataIndex = 0;
    static uint8_t dataIndexfaktor = 1;
    static uint16_t CRC;
    
    uint8_t returnValue;

    returnValue = 0;
    pms3003DataReady = 0;
    
    if(receiveUARTdata(&dataReceived))
    {
        *(pms3003_frame_ptr+dataIndex+dataIndexfaktor) = dataReceived;
        if(dataIndex < 22)
            CRC += dataReceived;
        dataIndexfaktor = 1 - dataIndexfaktor;
        if(*(pms3003_frame_ptr+dataIndex+dataIndexfaktor) == 0x42 && *(pms3003_frame_ptr+dataIndex+dataIndexfaktor-1) == 0x4d && (dataIndex > 0))
        {
            dataIndex = 2;
            dataIndexfaktor = 1;
            *(pms3003_frame_ptr) = 0x42;
            *(pms3003_frame_ptr+1) = 0x4d;
            CRC = 0x42 + 0x4d;
        }
        else
        {
            if(dataIndexfaktor)
                dataIndex += 2;
            if(dataIndex == 24)
            {
                dataIndex = 0;
                dataIndexfaktor = 1;
                if((pms3003_frame.checksum) == CRC)
                {
                    pms3003DataReady = 1;
                }
                else
                {
                    pms3003DataReady = 0;
                }
                CRC = 0x00;
            }
        }
    }

    return returnValue;
    
}