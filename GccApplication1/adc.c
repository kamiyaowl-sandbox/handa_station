/*
 * adc.c
 *
 * Created: 2015/06/16 23:00:59
 *  Author: owl
 */ 
#include "adc.h"

void adc_init(){
	//ADMUX |= (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0b0000);

	ADMUX |= (1 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0b0000);
	//ADPS[2:0] = 001:1/2 ~ 111:1/128
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (0 << ADATE) | (0 << ADIE) | (0b001);
	ADCSRB |= 0b000;
	
}
uint16_t adc_read(uint8_t ch){
	ADMUX &= ~0b1111;
	ADMUX |= (0b1111 & ch);
	ADCSRA |= (1 << ADSC);
	//wait
	while(ADCSRA & (1 << ADSC));
	return ADC;
}
