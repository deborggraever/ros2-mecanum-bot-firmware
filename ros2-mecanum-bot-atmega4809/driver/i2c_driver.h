/*
 * i2c_master.h
 *
 * Created: 24/03/2021 11:59:56
 *  Author: randy
 */ 


#ifndef I2C_MASTER_H_
#define I2C_MASTER_H_

#include <driver/i2c_types.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

void i2c_init();
i2c_error_t i2c_open(i2c_address_t address);
i2c_error_t i2c_close();
i2c_error_t i2c_transmit(bool read);
i2c_error_t i2c_write();
i2c_error_t i2c_read();
i2c_error_t i2c_set_buffer(void* buffer, size_t size);
i2c_error_t i2c_write_uint8_register(i2c_address_t address, uint8_t reg, uint8_t data, uint16_t timeout);
i2c_error_t i2c_read_uint8_register(i2c_address_t address, uint8_t reg, uint8_t* data, uint16_t timeout);
void i2c_set_address(i2c_address_t address);
void i2c_set_timeout(uint8_t timeout);
void i2c_set_baud_rate(uint32_t baud);
void i2c_set_data_complete_callback(i2c_callback cb, void* payload);
void i2c_set_write_collision_callback(i2c_callback cb, void* payload);
void i2c_set_address_nack_callback(i2c_callback cb, void* payload);
void i2c_set_data_nack_callback(i2c_callback cb, void* payload);
void i2c_set_timeout_callback(i2c_callback cb, void* payload);

#endif /* I2C_MASTER_H_ */