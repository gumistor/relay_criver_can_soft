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
#include "kho_i2c.h"

void initI2C (void) {

    //set interrupts
    PIE1bits.SSPIE = 0x00; //disable SPP interupt
    IPR1bits.SSPIP = 0x01; //set interupt priority high
    PIR1bits.SSPIF = 0x00; //clear pendig interrupt
    //set pins
    TRISCbits.TRISC3 = 1;
    LATCbits.LATC3 = 0;
    TRISCbits.TRISC4 = 1;
    LATCbits.LATC4 = 0;
    //set internal registers
    SSPCON1 |= 0b00101000; //no colision|no overflow|serial ports enabled|unused|I2C mastermode
    SSPCON2 |= 0b00010000;  //not used|ack receiced|ack sent|
    SSPSTAT |= 0b00000000; //(Standard_speed|SMBus disabled||||||)
    SSPADD = 0x09; // clock setup Fosc/(4*(SSPADD+1)) 200k
    //enable periphelia
//    nextStepI2C = 0;
    //enable interrupts
    PIE1bits.SSPIE = 0x00;
}

void I2C_Start(void) {
    SSPCON2bits.SEN = 0x01;

    while(SSPCON2bits.SEN) ;
}


void I2C_Stop(void) {
    SSPCON2bits.PEN = 0x01;

    while(SSPCON2bits.PEN) ;
}

void I2C_Restart(void) {
    SSPCON2bits.RSEN = 0x01;

    while(SSPCON2bits.RSEN) ;
}

void I2C_Read() {
    SSPCON2bits.RCEN = 0x01;

    while(SSPCON2bits.RCEN) ;
}

void I2C_AckAfterRead(void) {
    SSPCON2bits.ACKDT = 0x00;
    SSPCON2bits.ACKEN = 0x01;

   // while(SSPCON2bits.ACKEN) ;
}

void I2C_NackAfterRead(void) {
    SSPCON2bits.ACKDT = 0x01;
    SSPCON2bits.ACKEN = 0x01;

   // while(SSPCON2bits.ACKEN) ;
}

void I2C_Write(uint8_t data) {
    SSPBUF = data;

    while(SSPSTATbits.R_NOT_W) ;
}

uint8_t I2C_WaitAck(void) {
    uint8_t returnValue;
    uint8_t waitCnt;

    waitCnt = 40;
    while(!SSPCON2bits.ACKSTAT && waitCnt--) ;

    returnValue = SSPCON2bits.ACKSTAT;
    SSPCON2bits.ACKSTAT = 0;

    return returnValue;
}

void I2C_SendAndReceive(uint8_t *data2send, uint8_t data2send_size, uint8_t *dataReceived, uint8_t dataReceived_size) {
    uint8_t index;

    I2C_Start();

    //setPortAPinOff(4);
    index = 0;
    while(data2send_size) {
        I2C_Write(data2send[index++]);

        if(I2C_WaitAck())
            break;

        data2send_size--;
    }
    index = 0;
    while(dataReceived_size) {
        I2C_Read();
        if(dataReceived_size > 1) I2C_AckAfterRead();
        else I2C_NackAfterRead();

            //    if(!I2C_WaitAck()) break;

        dataReceived[index++] = SSPBUF;

        dataReceived_size--;
    }

    I2C_Stop();
}
