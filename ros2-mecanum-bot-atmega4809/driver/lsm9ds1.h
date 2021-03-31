/*
 * lsm9ds1.h
 *
 * Created: 24/03/2021 13:00:57
 *  Author: randy
 */ 


#ifndef LSM9DS1_H_
#define LSM9DS1_H_

#include <stdbool.h>
#include <stdio.h>
#include <driver/i2c_types.h>

#ifndef LSM9DS1_I2C_TIMEOUT
#	define LSM9DS1_I2C_TIMEOUT 10000
#endif

//#define LSM9DS1_ADDR_XG				0x6A
//#define LSM9DS1_ADDR_M				0x1C
#define LSM9DS1_ADDR_XG					0x6B
#define LSM9DS1_ADDR_M					0x1E

#define LSM9DS1_WHO_AM_I_XG				0x68
#define LSM9DS1_WHO_AM_I_M				0x3D

#define LSM9DS1_REG_ACT_THS				0x04
#define LSM9DS1_REG_ACT_DUR				0x05
#define LSM9DS1_REG_INT_GEN_CFG_XL		0x06
#define LSM9DS1_REG_INT_GEN_THS_X_XL	0x07
#define LSM9DS1_REG_INT_GEN_THS_Y_XL	0x08
#define LSM9DS1_REG_INT_GEN_THS_Z_XL	0x09
#define LSM9DS1_REG_INT_GEN_DUR_XL		0x0A
#define LSM9DS1_REG_REFERENCE_G			0x0B
#define LSM9DS1_REG_INT1_CTRL			0x0C
#define LSM9DS1_REG_INT2_CTRL			0x0D
#define LSM9DS1_REG_WHO_AM_I			0x0F
#define LSM9DS1_REG_CTRL_REG1_G			0x10
#define LSM9DS1_REG_CTRL_REG2_G			0x11
#define LSM9DS1_REG_CTRL_REG3_G			0x12
#define LSM9DS1_REG_ORIENT_CFG_G		0x13
#define LSM9DS1_REG_INT_GEN_SRC_G		0x14
#define LSM9DS1_REG_OUT_TEMP_L			0x15
#define LSM9DS1_REG_OUT_TEMP_H			0x16
#define LSM9DS1_REG_STATUS_REG			0x17
#define LSM9DS1_REG_OUT_X_L_G			0x18
#define LSM9DS1_REG_OUT_X_H_G			0x19
#define LSM9DS1_REG_OUT_Y_L_G			0x1A
#define LSM9DS1_REG_OUT_Y_H_G			0x1B
#define LSM9DS1_REG_OUT_Z_L_G			0x1C
#define LSM9DS1_REG_OUT_Z_H_G			0x1D
#define LSM9DS1_REG_CTRL_REG4			0x1E
#define LSM9DS1_REG_CTRL_REG5_XL		0x1F
#define LSM9DS1_REG_CTRL_REG6_XL		0x20
#define LSM9DS1_REG_CTRL_REG7_XL		0x21
#define LSM9DS1_REG_CTRL_REG8			0x22
#define LSM9DS1_REG_CTRL_REG9			0x23
#define LSM9DS1_REG_CTRL_REG10			0x24
#define LSM9DS1_REG_INT_GEN_SRC_XL		0x26
//#define LSM9DS1_REG_STATUS_REG			0x27
#define LSM9DS1_REG_OUT_X_L_XL			0x28
#define LSM9DS1_REG_OUT_X_H_XL			0x29
#define LSM9DS1_REG_OUT_Y_L_XL			0x2A
#define LSM9DS1_REG_OUT_Y_H_XL			0x2B
#define LSM9DS1_REG_OUT_Z_L_XL			0x2C
#define LSM9DS1_REG_OUT_Z_H_XL			0x2D
#define LSM9DS1_REG_FIFO_CTRL			0x2E
#define LSM9DS1_REG_FIFO_SRC			0x2F
#define LSM9DS1_REG_INT_GEN_CFG_G		0x30
#define LSM9DS1_REG_INT_GEN_THS_X_L_G	0x31
#define LSM9DS1_REG_INT_GEN_THS_X_H_G	0x32
#define LSM9DS1_REG_INT_GEN_THS_Y_L_G	0x33
#define LSM9DS1_REG_INT_GEN_THS_Y_H_G	0x34
#define LSM9DS1_REG_INT_GEN_THS_Z_L_G	0x35
#define LSM9DS1_REG_INT_GEN_THS_Z_H_G	0x36
#define LSM9DS1_REG_INT_GEN_DUR_G		0x37

#define LSM9DS1_REG_OFFSET_X_REG_L_M	0x05
#define LSM9DS1_REG_OFFSET_X_REG_H_M	0x06
#define LSM9DS1_REG_OFFSET_Y_REG_L_M	0x07
#define LSM9DS1_REG_OFFSET_Y_REG_H_M	0x08
#define LSM9DS1_REG_OFFSET_Z_REG_L_M	0x09
#define LSM9DS1_REG_OFFSET_Z_REG_H_M	0x0A
#define LSM9DS1_REG_CTRL_REG1_M			0x20
#define LSM9DS1_REG_CTRL_REG2_M			0x21
#define LSM9DS1_REG_CTRL_REG3_M			0x22
#define LSM9DS1_REG_CTRL_REG4_M			0x23
#define LSM9DS1_REG_CTRL_REG5_M			0x24
#define LSM9DS1_REG_STATUS_REG_M		0x27
#define LSM9DS1_REG_OUT_X_L_M			0x28
#define LSM9DS1_REG_OUT_X_H_M			0x29
#define LSM9DS1_REG_OUT_Y_L_M			0x2A
#define LSM9DS1_REG_OUT_Y_H_M			0x2B
#define LSM9DS1_REG_OUT_Z_L_M			0x2C
#define LSM9DS1_REG_OUT_Z_H_M			0x2D

typedef enum {
	LSM9DS1_AXIS_X = 0,
	LSM9DS1_AXIS_Y,
	LSM9DS1_AXIS_Z
} lsm9ds1_axis_t;

typedef struct {
	int16_t axis[3];
} lsm9ds1_raw_vector_t;

bool lsm9ds1_is_ready();
void lsm9ds1_reset();
void lsm9ds1_init();
void lsm9ds1_init_accel();
void lsm9ds1_init_gyro();
void lsm9ds1_init_mag();
void lsm9ds1_calibrate();
bool lsm9ds1_accel_available();
bool lsm9ds1_gyro_available();
bool lsm9ds1_mag_available();
void lsm9ds1_read_raw_accel(lsm9ds1_raw_vector_t* data);
void lsm9ds1_read_raw_gyro(lsm9ds1_raw_vector_t* data);
void lsm9ds1_read_raw_mag(lsm9ds1_raw_vector_t* data);
i2c_error_t lsm9ds1_set_register(i2c_address_t address, uint8_t reg, uint8_t data);
i2c_error_t lsm9ds1_get_register(i2c_address_t address, uint8_t reg, uint8_t* data);
i2c_error_t lsm9ds1_get_data(i2c_address_t address, uint8_t reg, void* data, size_t len);

#endif /* LSM9DS1_H_ */