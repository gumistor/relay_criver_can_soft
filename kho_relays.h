/* 
 * File:   kho_can.h
 * Author: Krzysztof
 *
 * Created on 9 czerwiec 2016, 21:13
 */

#ifndef KHO_RELAYS_H
#define	KHO_RELAYS_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "kho_dev_config.h"
    
//defines maksimum number of the relays
//acording to parameter
#if defined _AKWARIUM
#define NO_OF_RELAYS    0x04
#endif
#if defined _STORCZYKI
#define NO_OF_RELAYS    0x02
#endif
//Define max number of relays
#define MAX_NO_OF_RELAYS    0x08
//definition of relay status
#define DEVICE_ON   0x01
#define DEVICE_OFF  0x00
//definition of possible states
#define DEVICE_SWITCH 0x00
#define DEVICE_TIME   0x02

//table with pins on described ports
#if defined _AKWARIUM
uint8_t relayPORTAtable[MAX_NO_OF_RELAYS] = {0x01,0x00,0x08,0x00,0x00,0x00,0x00,0x00};
uint8_t relayPORTBtable[MAX_NO_OF_RELAYS] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t relayPORTCtable[MAX_NO_OF_RELAYS] = {0x04,0x03,0x20,0xC0,0x00,0x00,0x00,0x00};
#endif
#if defined _STORCZYKI
uint8_t relayPORTAtable[MAX_NO_OF_RELAYS] = {0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t relayPORTBtable[MAX_NO_OF_RELAYS] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
uint8_t relayPORTCtable[MAX_NO_OF_RELAYS] = {0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00};
#endif

//current status of the relay
uint8_t relayCurrentStatus[MAX_NO_OF_RELAYS];
//inhibited status of relay
uint8_t relayTemporaryStatus[MAX_NO_OF_RELAYS];
//counter with ticks to return status from relayTemporary to relayCurrrent
uint16_t relayTemporaryTimer[MAX_NO_OF_RELAYS];

//Initialize relays
uint8_t relaysInit(void);
//command to be executed
uint8_t evaluateCommand(uint8_t, uint8_t, uint16_t);
//check status and perform actions requested by command
uint8_t doRelays(void);

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_RELAYS_H */

