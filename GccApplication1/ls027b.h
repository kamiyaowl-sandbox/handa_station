/*
* ls027b.h
*
* Created: 2015/06/16 22:55:18
*  Author: owl
*/


#ifndef LS027B_H_
#define LS027B_H_

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "spi_master.h"

void ls027b_disp(uint8_t is_disp);
void ls027b_init(void);
uint8_t ls027b_vcom();
void ls027b_clear();
void ls027b_showimg();
void ls027b_update(uint8_t* data) ;

#endif /* LS027B_H_ */