/******************************************************************************/
/*Files to Include                                                            */
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

#endif

#include "kho_can.h"
#include "kho_uart.h"
#include "user.h"

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* High-priority service */

#if defined(__XC) || defined(HI_TECH_C)
void interrupt high_isr(void)
#elif defined (__18CXX)
#pragma code high_isr=0x08
#pragma interrupt high_isr
void high_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{
    INTCON &= ~(0b00110000);        //disable interrupts from timer and int0

    if(INTCON & 0b00000100)         //check TIMER0 interrupt
    {
        INTCON &= ~(0b00000100);    //clear TIMER0 flag
        systemTick++;                //update systems tick counter
        TMR0H = 0xFF;               //reset timer register
        TMR0L = SYSTEM_TICK;        //to rate 125ms
    }
    //enable interrupts
    INTCON |= 0b00100000;
}

/* Low-priority interrupt routine */

#if defined(__XC) || defined(HI_TECH_C)
void low_priority interrupt low_isr(void)
#elif defined (__18CXX)
#pragma code low_isr=0x18
#pragma interruptlow low_isr
void low_isr(void)
#else
#error "Invalid compiler selection for implemented ISR routines"
#endif
{
    PIE3 = 0x00;                                //dissable all interrupts
    if(PIR3 & 0b00000001) {                     //RX0 received
        doCANbuffer((_CANbuffer *)&RXB0CON);   //give pointer to function
        RXB0CON &= ~(0b10000000);               //erase receiver bit
        PIR3 &= ~(0b00000001);                  //clear interrupt
    }
    if(PIR3 & 0b00000010) {                     //RX1 received
        doCANbuffer((_CANbuffer *)&RXB1CON);   //give pointer to function
        RXB1CON &= ~(0b10000000);               //erase receiver bit
        PIR3 &= ~(0b00000010);                  //clear interrupt
    }
    if(PIR3 & 0b00000100) {                     //TX0 transmited
        PIR3 &= ~(0b00000100);                  //clear interrupt
    }
    if(PIR3 & 0b00001000) {                     //TX1 transmited
        PIR3 &= ~(0b00001000);                  //clear interrupt
    }
    if(PIR3 & 0b00010000) {                     //TX2 transmited
        PIR3 &= ~(0b00010000);                  //clear interrupt
    }
    if(PIR3 &       0b00100000) {               //ERROR on bus
        CANdriverErrors = COMSTAT;              //set error values to register
        PIR3 &= ~(  0b00100000);                //clear interrupt
    }
    if(PIR3 & 0b01000000) {                     //Bus activity wakeup
        PIR3 &= ~(0b01000000);                  //clear interrupt
    }
    if(PIR3 & 0b10000000) {                     //Invalid message on the bus
        PIR3 &= ~(0b10000000);                  //clear interrupt
    }
    PIE3 = 0xFF;                                //enable all interrupts
}
