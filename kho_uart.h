/* 
 * File:   kho_uart.h
 * Author: Krzysztof
 *
 * Created on 1 luty 2017, 15:10
 */

#ifndef KHO_UART_H
#define	KHO_UART_H

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t dataUARTreceived;
uint8_t dataUARTreceivedFlag;

uint8_t initUART(void);
uint8_t receiveUARTdata(uint8_t *);


#ifdef	__cplusplus
}
#endif

#endif	/* KHO_UART_H */

