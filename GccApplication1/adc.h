/*
 * adc.h
 *
 * Created: 2015/06/16 23:00:37
 *  Author: owl
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

void adc_init();
uint16_t adc_read(uint8_t ch);

#endif /* ADC_H_ */