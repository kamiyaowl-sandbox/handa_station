/*
 * uart.c
 *
 * Created: 2015/06/16 22:49:22
 *  Author: owl
 */ 

#include "uart.h"

void uart_init(unsigned int ubrr)
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (3<<UCSZ00);
}

void uart_send(char data)
{
	while ( !( UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

void uart_print(char* str){
	while(*str != '\0'){
		uart_send(*str);
		++str;
	}
}

