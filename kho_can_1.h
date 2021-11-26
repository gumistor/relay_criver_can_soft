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

#define CAN_NODE_ID             0x10

#define CAN_ADDR_CRITICAL       0x00
#define CAN_ADDR_SENSOR         0x100
#define CAN_ADDR_ACTUATOR       0x200
#define CAN_ADDR_DISPLAY        0x300
#define CAN_ADDR_FLOW           0x400
#define CAN_ADDR_REQUEST        0x500
#define CAN_ADDR_RESPONSE       0x600
#define CAN_ADDR_NM             0x700

#define CAN_CONFIGURATION_MODE  0b10000000
#define CAN_LISTEN_ONLY_MODE    0b01100000
#define CAN_LOOPBACK_MODE       0b01000000
#define CAN_DISABLE_MODE        0b00100000
#define CAN_NORMAL_MODE         0b00000000
    
#define CAN_NO_ERROR    0

#define NO_NEW_MSG      0
#define NEW_MSG         1
#define ERROR_MSG       3

volatile uint8_t new_can_message;
volatile uint8_t error_code;
volatile uint8_t int_reason;

uint8_t configureTX0buffer(uint32_t, uint8_t);
uint8_t configureTX1buffer(uint32_t, uint8_t);

uint8_t set_CANMode(uint8_t);
uint8_t initCAN(void);

uint8_t sendTX1buffer(uint8_t, uint8_t, uint8_t, uint8_t, 
                    uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t sendTX0buffer(uint8_t, uint8_t, uint8_t, uint8_t, 
                    uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_CAN_H */

