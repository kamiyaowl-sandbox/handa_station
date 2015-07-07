/*
 * spi_master.c
 *
 * Created: 2015/06/16 22:52:42
 *  Author: owl
 */ 
#include "spi_master.h"

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
