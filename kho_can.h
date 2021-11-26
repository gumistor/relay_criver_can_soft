/* 
 * File:   kho_can.h
 * Author: Krzysztof
 *
 * Created on 9 czerwiec 2016, 21:13
 */

#ifndef KHO_CAN_H
#define	KHO_CAN_H

#ifdef	__cplusplus
extern "C" {
#endif

/* ADDRESS CLASS definition*/
#define CAN_ADDR_CRITICAL       0x00000000  //ciritacal frames
#define CAN_ADDR_SENSOR         0x02000000  //sensors readout
#define CAN_ADDR_ACTUATOR       0x04000000  //acutators requests
#define CAN_ADDR_DISPLAY        0x06000000  //data to display
#define CAN_ADDR_CONFIG_REQ     0x08000000  //configuration request
#define CAN_ADDR_CONFIG_RES     0x09000000  //configuration response
#define CAN_ADDR_ERRORS         0x10000000  //errors codes
#define CAN_ADDR_REQUEST        0x0EFF0000  //diagnostics request
#define CAN_ADDR_RESPONSE       0x0FFF0000  //diagnostics response
#define CAN_ADDR_BOOT           0x1FFF0000  //bootloader mode request
#define CAN_ADDR_BOOT_RES       0x1FF00000  //bootloader mode request
#define CAN_ADDR_NM             0x10FF0000  //network management frame

    /* Modes of CAN driver */
#define CAN_CONFIGURATION_MODE  0b10000000
#define CAN_LISTEN_ONLY_MODE    0b01100000
#define CAN_LOOPBACK_MODE       0b01000000
#define CAN_DISABLE_MODE        0b00100000
#define CAN_NORMAL_MODE         0b00000000

    /* error flags values */
#define CAN_NO_ERROR    0

    /* receiver flag values */
#define CAN_DATA_EMPTY     0x00
#define CAN_DATA_FULL      0x01
    
    /* TX buffers addresses */
#define TX0BUFFER   (unsigned char *)&TXB0CON
#define TX1BUFFER   (unsigned char *)&TXB1CON
#define TX2BUFFER   (unsigned char *)&TXB2CON

/* structure of receiver registers type definition
 11-bit addressing: SIDH[7:0] SIDL[7:5]
 29-bit addressing: SIDH[7:0] SIDL[7:5] SIDL[1:0] EIDH[7:0] EIDL[7:0]
 */
typedef struct
{
    uint8_t CAN_RXBxCON;    //configuration byte
    uint8_t CAN_RXBxSIDH;   //high byte of address
    uint8_t CAN_RXBxSIDL;   //low byte of address
    uint8_t CAN_RXBxEIDH;   //high byte of extended address
    uint8_t CAN_RXBxEIDL;   //low byte of extended address
    uint8_t CAN_RXBxDLC;
    uint8_t CAN_RXBxD0;
    uint8_t CAN_RXBxD1;
    uint8_t CAN_RXBxD2;
    uint8_t CAN_RXBxD3;
    uint8_t CAN_RXBxD4;
    uint8_t CAN_RXBxD5;
    uint8_t CAN_RXBxD6;
    uint8_t CAN_RXBxD7;
} _CANbuffer;

volatile uint8_t CANdriverErrors;   //register with CAN errors

/* values of CAN device, based of EEPROM */
volatile uint8_t device_id;
volatile uint8_t can_node_class;

/* data receiced flag */
volatile uint8_t CAN_data_received_flag;

/* recived data lobal buffer
 * data are stored in this variable
 * after interupt from receiver has
 * been detected. */
struct CAN_data_received
{
    uint32_t CAN_address;
    uint8_t CAN_DLC;
    uint8_t CAN_data[8];
} CAN_data_received;

/* Trasmiters buffers configurator */
uint8_t configureTXbuffer(unsigned char *, uint32_t, uint8_t);

/* Transmiters buffers release functions */
uint8_t sendTXbuffer(unsigned char *, uint8_t, uint8_t, uint8_t, uint8_t,
                    uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

uint8_t initCAN(void);          //initialize CAN driver
void doNM(uint8_t);   //function to proceed with NM
void doERRORS(void);            //function to proceed with errors
void doCANbuffer(_CANbuffer *); //function copy received data to
                                //CAN_data_received

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_CAN_H */

