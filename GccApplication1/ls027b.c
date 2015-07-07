/*
 * ls027b.c
 *
 * Created: 2015/06/16 22:55:00
 *  Author: owl
 */ 
#include "ls027b.h"
#include "ImageData.h"

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
	send_buf[0] |= ls027b_vcom() | (1 << M2);
	send_buf[1] = 0x0;
	spi_master_putarray(send_buf,2);
	//Wait
	_delay_ms(1);
	LS027B_PORT &= ~(1 << LS027B_CS);
}

void ls027b_showimg() {
	for(uint8_t j = 0 ; j < 240 ; ++j){
		
		send_buf[0] = 0x0;
		send_buf[0] |= ls027b_vcom() | (1 << M0);
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

void ls027b_update(uint8_t* data) {
	for(uint8_t j = 0 ; j < 240 ; ++j){
		
		send_buf[0] = 0x0;
		send_buf[0] |= ls027b_vcom() | (1 << M0);
		send_buf[1] = 0x1 + j;
		
		for(uint8_t i = 0 ; i < 50 ; ++i){
			uint16_t x = i << 3;
			send_buf[2 + i] = 
				((data[x + 0] < j) ? 0x01 : 0x00) |
				((data[x + 1] < j) ? 0x02 : 0x00) |
				((data[x + 2] < j) ? 0x04 : 0x00) |
				((data[x + 3] < j) ? 0x08 : 0x00) |
				((data[x + 4] < j) ? 0x10 : 0x00) |
				((data[x + 5] < j) ? 0x20 : 0x00) |
				((data[x + 6] < j) ? 0x40 : 0x00) |
				((data[x + 7] < j) ? 0x80 : 0x00);
		}
		
		send_buf[52] = 0x0;
		send_buf[53] = 0x0;
		LS027B_PORT |= (1 << LS027B_CS);
		spi_master_putarray(send_buf,54);
		LS027B_PORT &= ~(1 << LS027B_CS);
	}
}
