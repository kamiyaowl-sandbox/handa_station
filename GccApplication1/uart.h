/*
 * uart.h
 *
 * Created: 2015/06/16 22:50:29
 *  Author: owl
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>

#define BAUD 115200
#define MYUBRR F_CPU/16/BAUD-1

void uart_init(unsigned int ubrr);
void uart_send(char data);
void uart_print(char* str);

#endif /* UART_H_ */