/*
 * spi.h
 *
 * Created: 2015/06/16 22:52:30
 *  Author: owl
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

#define SPI_DDR		DDRB
#define	SPI_PORT	PORTB
#define SPI_SS			2
#define SPI_MOSI		3
#define SPI_MISO		4
#define SPI_SCK			5

void spi_master_init(void);
void spi_master_send(uint8_t data);
void spi_master_putarray(uint8_t* data,uint8_t len);
void spi_master_receive(uint8_t *data);
void spi_mater_transfer(uint8_t send,uint8_t *receive);

#endif /* SPI_H_ */