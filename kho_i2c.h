/* 
 * File:   kho_i2c.h
 * Author: Krzysztof
 *
 * Created on 13 grudzie? 2016, 22:40
 */

#ifndef KHO_I2C_H
#define	KHO_I2C_H

#ifdef	__cplusplus
extern "C" {
#endif

void initI2C (void) ;
void I2C_SendAndReceive(uint8_t *, uint8_t, uint8_t *, uint8_t);

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_I2C_H */

