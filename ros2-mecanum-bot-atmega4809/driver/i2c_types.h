/*
 * i2c_types.h
 *
 * Created: 24/03/2021 11:59:48
 *  Author: randy
 */ 


#ifndef I2C_TYPES_H_
#define I2C_TYPES_H_

#include <mcu/mcu_clock.h>
#include <stdint.h>

#define TWI0_BAUD(F_SCL, T_RISE) ((((((float)F_CPU / (float)F_SCL)) - 10 - ((float)F_CPU * T_RISE / 1000000))) / 2)

typedef enum {
	I2C_OK,
	I2C_BUSY,
	I2C_FAIL
} i2c_error_t;

typedef enum {
	i2c_stop = 1,
	i2c_restart_read,
	i2c_restart_write,
	i2c_continue,
	i2c_reset_link
} i2c_operations_t;

typedef i2c_operations_t (*i2c_callback)(void *p);

typedef uint8_t i2c_address_t;

i2c_operations_t i2c_cb_return_stop(void *p);
i2c_operations_t i2c_cb_return_reset(void *p);
i2c_operations_t i2c_cb_restart_write(void *p);
i2c_operations_t i2c_cb_restart_read(void *p);

#endif /* I2C_TYPES_H_ */