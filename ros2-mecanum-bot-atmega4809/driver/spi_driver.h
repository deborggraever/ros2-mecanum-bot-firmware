/*
 * spi_driver.h
 *
 * Created: 24/03/2021 23:10:25
 *  Author: randy
 */ 


#ifndef SPI_DRIVER_H_
#define SPI_DRIVER_H_

#include <stdint.h>
#include <stdio.h>

void spi_init();
void spi_enable();
void spi_disable();
uint8_t spi_exchange_byte(uint8_t data);
void spi_write(void* buffer, size_t length);
void spi_read(void* buffer, size_t length);

#endif /* SPI_DRIVER_H_ */