/*
* GccApplication1.c
*
* Created: 2015/06/15 20:24:12
*  Author: owl
*/
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//////////////////////////////////////////////////////////////////////////
#define DDR_7SEG	DDRD
#define PORT_7SEG	PORTD

#define DDR_UI		DDRC
#define PORT_UI		PORTC
#define PIN_UI		PINC

#define PIN_LED_LOW			PC0
#define PIN_LED_HIGH		PC1
#define PIN_COM_7SEG_LOW	PC2
#define PIN_COM_7SEG_HIGH	PC3
#define PIN_ROT_A	PC4
#define PIN_ROT_B	PC5

#define ROTARY_STOP	0
#define ROTARY_UP	1
#define ROTARY_DOWN	2

#define DDR_COMPONENT	DDRB
#define PORT_COMPONENT	PORTB
#define PIN_COMPONENT	PINB

#define PIN_BUZZER	PB0
#define PIN_SSR		PB1

const uint8_t segment_data[] ={
	0b11111100,
	0b01100000,
	0b11011010,
	0b11110010,
	0b01100110,
	0b10110110,
	0b10111110,
	0b11100100,
	0b11111110,
	0b11110110,
	0b11101110,
	0b00111110,
	0b00011010,
	0b01111010,
	0b10011110,
	0b10001110,
};


volatile uint8_t segment_num_high = 0;
volatile uint8_t segment_num_low = 0;

volatile uint8_t segment_visible = 0x0;//0...LEFT, 1...RIGHT
volatile uint16_t countdown_time = 0x0;

uint8_t rotary_old = 0;

void update_time(uint16_t sec){
	if(sec < 100){
		PORT_UI &= ~0x3;
		PORT_UI |= (1 << PIN_LED_LOW);
		segment_num_high = sec / 10;
		segment_num_low = sec % 10;
		} else {
		PORT_UI &= ~0x3;
		PORT_UI |= (1 << PIN_LED_HIGH);
		uint16_t min = sec / 60;
		segment_num_high = min / 10;
		segment_num_low = min % 10;
	}
}

uint8_t rotary_update(){
	uint8_t current = (PIN_UI >> 4) & 0x3;
	switch(current) {
		case 2:
		current = 3;
		break;
		case 3:
		current = 2;
		break;
		default:
		break;
	}
	int8_t diff = current - rotary_old;
	uint8_t result = 0;
	switch(diff) {
		case 1:
		case -3:
		result = ROTARY_UP;
		break;
		case -1:
		case 3:
		result = ROTARY_DOWN;
		break;
		default:
		break;
	}
	rotary_old = current;
	return result;
}
void init_ui(){
	DDR_COMPONENT = 0x03;
	
	DDR_UI = 0b001111;
	PORT_UI = 0b111111;
	
	DDR_7SEG = 0xff;
	
	TCCR0B = 0x03;//1/64
	TIMSK0 = 0x01;//TOIE
}

ISR(TIMER0_OVF_vect){
	//rotary
	switch(rotary_update()){
		case ROTARY_UP:
			countdown_time += countdown_time < 100 ? 10 : 60;
			break;
		case ROTARY_DOWN:
			countdown_time -= countdown_time > 10 ? 10 : countdown_time;
			break;
	}
	//7seg
	update_time(countdown_time);
	switch(segment_visible++ & 0x7){
		case 0x0:
		case 0x4:
		PORT_7SEG = 0xff;
		PORT_UI |= 0b1100;
		break;
		case 0x1:
		PORT_7SEG = ~segment_data[segment_num_high & 0xf];
		PORT_UI &= ~(1 << PIN_COM_7SEG_HIGH);
		break;
		case 0x5:
		PORT_7SEG = ~segment_data[segment_num_low & 0xf];
		PORT_UI &= ~(1 << PIN_COM_7SEG_LOW);
		break;
	}
	//SSR
	if(countdown_time) PORT_COMPONENT |= (1 << PIN_SSR);
	else PORT_COMPONENT &= ~(1 << PIN_SSR);
}

//////////////////////////////////////////////////////////////////////////

int main(void)
{
	_delay_ms(500);
	countdown_time = 60 * 31;
	
	init_ui();
	
	sei();
	///////////////////////////
	
	for(uint16_t i = 0 ; ; ++i){
		if(countdown_time > 0) --countdown_time;
		
		for(uint16_t j = 0 ; j < 1000 ; ++j){
			_delay_us(1000);
		}
	}
}