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

#endif

#include "kho_can.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

/* Function store in global variables data from eeprom with ID and
 * class of the device*/
void set_CLASSandID(void)
{
        device_id       = EEPROM_READ(0x01);    //CAN_NODE_ID;
        can_node_class  = EEPROM_READ(0x00);    //CAN_NODE_CLASS;
}

/* Function to set CAN driver mode. Function is waiting for change. */
uint8_t set_CANMode(uint8_t mode)
{
    uint8_t return_value;           //error flag
            
    return_value = CAN_NO_ERROR;    //initially there is no error
            
    CANCON = mode;                  //fill register with mode requested
    while(!((CANSTAT & 0b11100000 ) == mode));  
                                    //wait for change
    return return_value;            //retrun error flag
}

uint8_t setBufferFilter(unsigned char *buffer, uint32_t filter, uint8_t extended)
{
    uint8_t return_value;
            
    return_value = CAN_NO_ERROR;
 
    if(extended)
    {
        *(buffer+1) = (uint8_t)((filter >> 13) & 0x000000E0); //SIDL
        *(buffer+0) = (uint8_t)((filter >> 21) & 0x000000FF); //SIDH
        *(buffer+3) = (uint8_t)((filter) & 0x000000FF); //EIDL
        *(buffer+2) = (uint8_t)((filter >> 8) & 0x000000FF); //EIDH
        extended = (uint8_t)((filter >> 16) & 0x00000003);
        *(buffer+1) |= extended;
        *(buffer+1) |= 0b00001000;
    }
    else
    {
        *(buffer+1) = (uint8_t)((filter << 5) & 0x000000E0); //SIDL
        *(buffer+0) = (uint8_t)((filter >> 3) & 0x000000FF); //SIDH
    }
    
    return return_value;
}

uint8_t setBufferMask(unsigned char *buffer, uint32_t mask, uint8_t extended)
{
    uint8_t return_value;
    //uint8_t extended;
    
    return_value = CAN_NO_ERROR;

    if(extended)
    {
        *(buffer+1) = (uint8_t)((mask >> 13) & 0x000000E0);
        *(buffer+0) = (uint8_t)((mask >> 21) & 0x000000FF);
        *(buffer+3) = (uint8_t)((mask) & 0x000000FF);
        *(buffer+2) = (uint8_t)((mask >> 8) & 0x000000FF);

        extended = (uint8_t)((mask >> 16) & 0x00000003);
        *(buffer+1) |= extended;
    }
    else
    {
        *(buffer+1) = (uint8_t)((mask << 5) & 0x000000E0); //SIDL
        *(buffer+0) = (uint8_t)((mask >> 3) & 0x000000FF); //SIDH
    }
    
    return return_value;
}

uint8_t initCAN(void)
{
    uint8_t return_value;
    
    //setup class and id of device
    set_CLASSandID();

    //set interrupts
    PIE3 = 0x00; //disable all interupts
    IPR3 = 0x00; //set all interupts on high
    PIR3 = 0x00; //clear all interrupts
    
    //set pins
    TRISB |= 0b00001000;    //set RB3 set as an input
    TRISB &= ~(0b00000100); //set RB2 set as an output
    //set internal registers
    
    set_CANMode(CAN_CONFIGURATION_MODE);

    RXB0CON = 0b00000000; //RXB0CON = 0b00100000; //RXF0 0b001... - only 11bit addressing allowed
    RXB1CON = 0b00000000; //RXF1 0b001... - only 11bit addressing allowed
    
    //CIOCON = 0x20;

    ECANCON = 0x00;         // Select legacy mode

    // Initialize CAN Timing
    BRGCON1 = 0x01; //0000 0001     //SJW=1TQ     BRP  1
    BRGCON2 = 0x91; //0001 0001     //SEG2PHTS 1    sampled once  PS1=8TQ  PropagationT 1TQ
    BRGCON3 = 0x01; //0000 0010     //PS2  6TQ
    //
    // Filters initialization for buffer 0
    //
    //tx0_busy = TX_READY;
    //tx1_busy = TX_READY;
    //tx2_busy = TX_READY;
    

    setBufferFilter((unsigned char *)&RXF0SIDH, CAN_ADDR_NM, 1);
    setBufferFilter((unsigned char *)&RXF1SIDH, CAN_ADDR_DISPLAY, 1);
    setBufferMask  ((unsigned char *)&RXM0SIDH, 0x1FFF0000   ,1);
    //
    // Filters initialization for buffer 1
    //
    setBufferFilter((unsigned char *)&RXF2SIDH, CAN_ADDR_CONFIG_REQ  | can_node_class<<8 | device_id, 1);
    setBufferFilter((unsigned char *)&RXF3SIDH, CAN_ADDR_REQUEST     | can_node_class<<8 |  device_id, 1);
    setBufferFilter((unsigned char *)&RXF4SIDH, CAN_ADDR_ACTUATOR    | can_node_class<<8 |  device_id, 1);
    setBufferFilter((unsigned char *)&RXF5SIDH, CAN_ADDR_SENSOR                                      , 1); //NM frame reception
    setBufferMask  ((unsigned char *)&RXM1SIDH, 0x1FFFFFFF,                       1);
   
#if 1 
    //Enable filters
    RXFCON0 = 0b00000000; //Enabled: RXF0, RXF1, RXF2
    RXFCON1 = 0b00000000; //other filters disabled
#endif
    //restore normal mode
    set_CANMode(CAN_NORMAL_MODE); //CAN_LOOPBACK_MODE - for tests
    //Set transmit buffer 0 & 1
    configureTXbuffer(TX0BUFFER, CAN_ADDR_SENSOR          | can_node_class<<8 |    device_id,1);
    configureTXbuffer(TX1BUFFER, CAN_ADDR_DISPLAY         | can_node_class<<8 |    device_id,1);
    configureTXbuffer(TX2BUFFER, CAN_ADDR_NM              | can_node_class<<8 |    device_id,1);

    CAN_data_received_flag = CAN_DATA_EMPTY;

    CANdriverErrors = 0x00;

    //enable interrupts

    PIE3 = 0xFF; //enable all interupts

    return_value = CAN_NO_ERROR;

    return return_value;
}

/* Function to configure trensmiter buffer */
uint8_t configureTXbuffer(unsigned char *buffer, uint32_t id, uint8_t extended)
{
    uint8_t return_value;

    return_value = CAN_NO_ERROR;

    while((*(buffer+0)) & 0b00001000);

    if(extended)
    {
        *(buffer+2) = (uint8_t)((id >> 13) & 0x000000E0);
        *(buffer+1) = (uint8_t)((id >> 21) & 0x000000FF);

        *(buffer+4) = (uint8_t)((id) & 0x000000FF);
        *(buffer+3) = (uint8_t)((id >> 8) & 0x000000FF);
        extended = (uint8_t)((id >> 16) & 0x00000003);
        *(buffer+2) |= extended;
        *(buffer+2) |= 0b00001000;
    }
    else
    {
        *(buffer+2) = (uint8_t)((id << 5) & 0x000000E0);
        *(buffer+1) = (uint8_t)((id >> 3) & 0x000000FF);
    }
    return return_value;
}

uint8_t sendTXbuffer(unsigned char *buffer, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                    uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t dlc)
{
    uint8_t return_value;

    return_value = CAN_NO_ERROR;

    while((*(buffer+0)) & 0b00001000);

    *(buffer+5) = dlc;
    *(buffer+6) = d0;
    *(buffer+7) = d1;
    *(buffer+8) = d2;
    *(buffer+9) = d3;
    *(buffer+10) = d4;
    *(buffer+11) = d5;
    *(buffer+12) = d6;
    *(buffer+13) = d7;

    *(buffer+0) |= 0b00001000; //Set the buffer to transmit

    return return_value;
}

uint32_t makeAddressFromRegisters(_CANbuffer *data)
{
    uint32_t return_value = 0;
    if(data->CAN_RXBxSIDL && 0b00001000)
    {
        return_value |= (data->CAN_RXBxSIDH);
        return_value = return_value << 3;
        return_value |= ((data->CAN_RXBxSIDL) >> 5) & 0x07;
        return_value = return_value << 2;
        return_value |= (data->CAN_RXBxSIDL) & 0x03;
        return_value = return_value << 8;
        return_value |= (data->CAN_RXBxEIDH);
        return_value = return_value << 8;
        return_value |= (data->CAN_RXBxEIDL);
    }
    else
    {
        return_value = (data->CAN_RXBxSIDH)<<3 | (data->CAN_RXBxSIDL) >> 5;
    }
    return return_value;
}

void doCANbuffer(_CANbuffer *data)
{
    if(CAN_data_received_flag == CAN_DATA_EMPTY)
    {
        CAN_data_received.CAN_address = makeAddressFromRegisters(data);
        CAN_data_received.CAN_DLC = data->CAN_RXBxDLC;
        CAN_data_received.CAN_data[0] = data->CAN_RXBxD0;
        CAN_data_received.CAN_data[1] = data->CAN_RXBxD1;
        CAN_data_received.CAN_data[2] = data->CAN_RXBxD2;
        CAN_data_received.CAN_data[3] = data->CAN_RXBxD3;
        CAN_data_received.CAN_data[4] = data->CAN_RXBxD4;
        CAN_data_received.CAN_data[5] = data->CAN_RXBxD5;
        CAN_data_received.CAN_data[6] = data->CAN_RXBxD6;
        CAN_data_received.CAN_data[7] = data->CAN_RXBxD7;
        CAN_data_received_flag = CAN_DATA_FULL;
    }
}

void doERRORS(void)
{
    if(CANdriverErrors != 0x00)
    {
        configureTXbuffer(TX2BUFFER, CAN_ADDR_ERRORS         | can_node_class<<8 |    device_id,1);
        sendTXbuffer(TX2BUFFER, ((CANdriverErrors >> 6)& 0x03),((CANdriverErrors >> 5)& 0x01),
                ((CANdriverErrors >> 3)& 0x03),((CANdriverErrors >> 1)& 0x03),
                ((CANdriverErrors)& 0x01),TXERRCNT,RXERRCNT,0xFF,0x07);
        CANdriverErrors = 0x00;
    }
}

void doNM(uint8_t netNotNeeded)
{
    static uint8_t frameIndex = 0;
    static uint8_t awakeReason = 1;

    configureTXbuffer(TX2BUFFER, CAN_ADDR_NM         | can_node_class<<8 |    device_id,1);
    sendTXbuffer(TX2BUFFER, awakeReason,frameIndex++,0x00,0x00,0x00,0x00,0x00,0x00,0x02);
}
