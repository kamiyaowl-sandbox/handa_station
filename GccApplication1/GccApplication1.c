/*
* GccApplication1.c
*
* Created: 2015/06/15 20:24:12
*  Author: owl
*/
#define F_CPU 8e6

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include "ImageData.h"

/* UART */
#define BAUD 115200
#define MYUBRR F_CPU/16/BAUD-1

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

/* END UART  */

/* SPI */
#define SPI_DDR		DDRB
#define	SPI_PORT	PORTB
#define SPI_SS			2
#define SPI_MOSI		3
#define SPI_MISO		4
#define SPI_SCK			5

void spi_master_init(void)
{
	PRR &= ~(1 << PRSPI);
	SPI_DDR = (1 << SPI_MOSI)|(1 << SPI_SCK)|(1 << SPI_SS);
	SPCR = (1<<SPE)|(1<<MSTR)|(1 << DORD)|(0 << SPR1)|(0<<SPR0);//DORD=1 : LSB First
}

void spi_master_send(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
}
void spi_master_putarray(uint8_t* data,uint8_t len)
{
	for(uint8_t i = 0 ; i < len ; ++i)
	spi_master_send(data[i]);
}
void spi_master_receive(uint8_t *data)
{
	SPDR = 0xFF;
	while(!(SPSR & (1<<SPIF)));
	*data = SPDR;
}
void spi_mater_transfer(uint8_t send,uint8_t *receive)
{
	SPDR = send;
	while(!(SPSR & (1<<SPIF)));
	*receive = SPDR;
}
/* END SPI*/

/* LS027B */
#define LS027B_DDR		DDRD
#define LS027B_PORT		PORTD
#define LS027B_CS		0
#define LS027B_DISP		1

#define M0	0//Data Update
#define M1	1//VCOM Invert
#define M2	2//All Clear

uint8_t vcom_status = 0x0;
uint8_t send_buf[54] = {};

void ls027b_disp(uint8_t is_disp){
	if(is_disp) LS027B_PORT |= (1 << LS027B_DISP);
	else LS027B_PORT &= ~(1 << LS027B_DISP);
}

void ls027b_init(void){
	spi_master_init();
	LS027B_DDR = 0xff;
	LS027B_PORT = 0x0;

	ls027b_disp(0x1);
}

uint8_t ls027b_vcom(){
	return (vcom_status++) & 0x80 ? (1 << M1) : (0 << M1);
}
void ls027b_clear() {
	LS027B_PORT |= (1 << LS027B_CS);
	send_buf[0] = 0x0;
	send_buf[0] |= vcom_status | (1 << M2);
	send_buf[1] = 0x0;
	spi_master_putarray(send_buf,2);
	//Wait
	_delay_ms(1);
	LS027B_PORT &= ~(1 << LS027B_CS);
}

void ls027b_showimg() {
	for(uint8_t j = 0 ; j < 240 ; ++j){
		
		send_buf[0] = 0x0;
		send_buf[0] |= vcom_status | (1 << M0);
		send_buf[1] = 0x1 + j;
		
		for(uint8_t i = 0 ; i < 50 ; ++i){
			send_buf[2 + i] = (uint8_t)pgm_read_byte(&(img_src[j][i]));
		}
		
		send_buf[52] = 0x0;
		send_buf[53] = 0x0;
		LS027B_PORT |= (1 << LS027B_CS);
		spi_master_putarray(send_buf,54);
		LS027B_PORT &= ~(1 << LS027B_CS);
	}
}

void ls027b_update(uint16_t l_data, uint16_t r_data) {
	for(uint8_t j = 0 ; j < 240 ; ++j){
		
		send_buf[0] = 0x0;
		send_buf[0] |= vcom_status | (1 << M0);
		send_buf[1] = 0x1 + j;
		
		for(uint8_t i = 0 ; i < 50 ; ++i){
			if(j < 120){
				send_buf[2 + i] = (i * 8 < l_data) ? 0xff : 0x0;
				} else {
				send_buf[2 + i] = (i * 8 < r_data) ? 0xff : 0x0;
			}
		}
		
		send_buf[52] = 0x0;
		send_buf[53] = 0x0;
		LS027B_PORT |= (1 << LS027B_CS);
		spi_master_putarray(send_buf,54);
		LS027B_PORT &= ~(1 << LS027B_CS);
	}
}

/* END LS027B */

/* ADC */
void adc_init(){
	ADMUX |= (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (0b0000);
	//ADIF:ad interrupt flag
	//ADIE:ad interrupt enable
	//ADPS[2:0] = 001:1/2 ~ 111:1/128
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (0 << ADATE) | (0 << ADIE) | (0b111);
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
/* End ADC */

int main(void)
{
	uint16_t l_data = 0x0, r_data = 0x0;
	adc_init();
	
	ls027b_init();
	ls027b_clear();
	ls027b_showimg();
	ls027b_disp(0x1);
	_delay_ms(1000);
	
	while(1)
	{
		l_data = adc_read(0x0);
		r_data = adc_read(0x1);
		
		ls027b_update(l_data,r_data);
	}
}