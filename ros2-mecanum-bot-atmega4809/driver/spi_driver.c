/*
 * spi_driver.c
 *
 * Created: 25/03/2021 9:04:25
 *  Author: randy
 */ 

#include <mcu/mcu_port.h>
#include <driver/spi_driver.h>

void spi_init()
{
	// Configure pins
	PORTE_init_pin(0, PORT_DIR_OUT, false);	// MOSI
	PORTE_init_pin(1, PORT_DIR_IN, false);	// MISO
	PORTE_init_pin(2, PORT_DIR_OUT, false);	// SCK
	PORTMUX.TWISPIROUTEA |= PORTMUX_SPI01_bm;
	
	// Set mode
	SPI0.CTRLA = 0 << SPI_CLK2X_bp	// Enable Double Speed: disabled
		| 0 << SPI_DORD_bp			// Data Order Setting: disabled
		| 1 << SPI_ENABLE_bp		// Enable Module: enabled
		| 1 << SPI_MASTER_bp		// SPI module in master mode
		| SPI_PRESC_DIV4_gc;		// System Clock / 4
}

void spi_enable()
{
	SPI0.CTRLA |= SPI_ENABLE_bm;
}

void spi_disable()
{
	SPI0.CTRLA &= ~SPI_ENABLE_bm;
}

uint8_t spi_exchange_byte(uint8_t data)
{
	SPI0.DATA = data;
	while (!(SPI0.INTFLAGS & SPI_RXCIF_bm))
		;
	return SPI0.DATA;
}

void spi_write(void* buffer, size_t length)
{
	uint8_t* data = (uint8_t*)buffer;
	while (length--) {
		spi_exchange_byte(*data);
		data++;
	}
}

void spi_read(void* buffer, size_t length)
{
	uint8_t* data = (uint8_t*)buffer;
	while (length--) {
		*data = spi_exchange_byte(0);
		data++;
	}
}
