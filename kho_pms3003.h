/* 
 * File:   kho_pms3003.h
 * Author: Krzysztof
 *
 * Created on 1 luty 2017, 15:54
 */

#ifndef KHO_PMS3003_H
#define	KHO_PMS3003_H

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t initPMS3003(void);
uint8_t pms3003_gather_data(void);

typedef struct
{
    uint16_t start_bytes;
    uint16_t data_length;
    uint16_t pm010_concentration;
    uint16_t pm025_concentration;
    uint16_t pm100_concentration;
    uint16_t pm010_concentration_;
    uint16_t pm025_concentration_;
    uint16_t pm100_concentration_;
    uint16_t reserved1;
    uint16_t reserved2;
    uint16_t reserved3;
    uint16_t checksum;
} pms3003_frame_type;

pms3003_frame_type pms3003_frame;
uint8_t *pms3003_frame_ptr = (uint8_t *)&pms3003_frame;

uint8_t pms3003DataReady;

#ifdef	__cplusplus
}
#endif

#endif	/* KHO_PMS3003_H */


