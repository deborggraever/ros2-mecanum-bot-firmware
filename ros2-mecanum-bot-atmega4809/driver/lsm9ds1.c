/*
 * lsm9ds1.c
 *
 * Created: 24/03/2021 13:01:07
 *  Author: randy
 */ 

#include <driver/lsm9ds1.h>
#include <driver/i2c_driver.h>

typedef struct {
	uint8_t dummy;
} lsm9ds1_state;

typedef struct {
	size_t len;
	char * data;
} lsm9ds1_buffer_t;

lsm9ds1_state _lsm9ds1_state = {0};

i2c_operations_t lsm9ds1_set_register_completed_callback(void* payload);
i2c_operations_t lsm9ds1_get_register_completed_callback(void* payload);
i2c_operations_t lsm9ds1_get_data_completed_callback(void* payload);

bool lsm9ds1_is_ready()
{
	uint8_t dev_id_xg;
	uint8_t dev_id_m;
	lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_WHO_AM_I, &dev_id_xg);
	lsm9ds1_get_register(LSM9DS1_ADDR_M, LSM9DS1_REG_WHO_AM_I, &dev_id_m);
	return (dev_id_xg == LSM9DS1_WHO_AM_I_XG && dev_id_m == LSM9DS1_WHO_AM_I_M);
}

void lsm9ds1_reset()
{
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG8, 0x05);
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG2_M, 0x0C);
}

void lsm9ds1_init()
{
	lsm9ds1_init_accel();
	lsm9ds1_init_gyro();
	lsm9ds1_init_mag();
}

void lsm9ds1_init_accel()
{
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG5_XL, 0x38);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG6_XL, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG7_XL, 0x00);
}

void lsm9ds1_init_gyro()
{
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG1_G, 0xC0);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG2_G, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG3_G, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG4, 0x38);
}

void lsm9ds1_init_mag()
{
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG1_M, 0x1C);
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG2_M, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG3_M, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG4_M, 0x00);
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_CTRL_REG5_M, 0x00);
}

void lsm9ds1_calibrate()
{
	uint8_t fifo_state;
	
	// Enable FIFO
	lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG9, &fifo_state);
	fifo_state |= (1 << 1);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG9, fifo_state);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_FIFO_CTRL, (((1 & 0x07) << 5) | 0x1F));
	
	uint8_t samples_data = 0;
	uint8_t samples = 0;
	int32_t tmp_a_bias[3] = {0,0,0};
	int32_t tmp_g_bias[3] = {0,0,0};
	lsm9ds1_raw_vector_t a_vec = {0};
	lsm9ds1_raw_vector_t g_vec = {0};
	while (samples <= 0x1F) {
		lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_FIFO_SRC, &samples_data);
		samples = samples_data & 0x3F;
	}
	for (uint8_t i = 0; i < samples; i++) {
		lsm9ds1_read_raw_accel(&a_vec);
		tmp_a_bias[LSM9DS1_AXIS_X] += a_vec.axis[LSM9DS1_AXIS_X];
		tmp_a_bias[LSM9DS1_AXIS_Y] += a_vec.axis[LSM9DS1_AXIS_Y];
		tmp_a_bias[LSM9DS1_AXIS_Z] += a_vec.axis[LSM9DS1_AXIS_Z];
		lsm9ds1_read_raw_gyro(&g_vec);
		tmp_g_bias[LSM9DS1_AXIS_X] += g_vec.axis[LSM9DS1_AXIS_X];
		tmp_g_bias[LSM9DS1_AXIS_Y] += g_vec.axis[LSM9DS1_AXIS_Y];
		tmp_g_bias[LSM9DS1_AXIS_Z] += g_vec.axis[LSM9DS1_AXIS_Z];
	}
	
	// Disable FIFO
	lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG9, &fifo_state);
	fifo_state &= ~(1 << 1);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_CTRL_REG9, fifo_state);
	lsm9ds1_set_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_FIFO_CTRL, ((0 & 0x07) << 5) | (0 & 0x1F));
	
	int16_t m_min[3] = { INT16_MAX, INT16_MAX, INT16_MAX };
	int16_t m_max[3] = { INT16_MIN, INT16_MIN, INT16_MIN };
	lsm9ds1_raw_vector_t m_vec;
	for (uint8_t i = 0; i < 128; i++) {
		while (!lsm9ds1_mag_available())
			;
		lsm9ds1_read_raw_mag(&m_vec);
		if (m_vec.axis[LSM9DS1_AXIS_X] < m_min[LSM9DS1_AXIS_X]) m_min[LSM9DS1_AXIS_X] = m_vec.axis[LSM9DS1_AXIS_X];
		if (m_vec.axis[LSM9DS1_AXIS_X] > m_max[LSM9DS1_AXIS_X]) m_max[LSM9DS1_AXIS_X] = m_vec.axis[LSM9DS1_AXIS_X];
		if (m_vec.axis[LSM9DS1_AXIS_Y] < m_min[LSM9DS1_AXIS_Y]) m_min[LSM9DS1_AXIS_Y] = m_vec.axis[LSM9DS1_AXIS_Y];
		if (m_vec.axis[LSM9DS1_AXIS_Y] > m_max[LSM9DS1_AXIS_Y]) m_max[LSM9DS1_AXIS_Y] = m_vec.axis[LSM9DS1_AXIS_Y];
		if (m_vec.axis[LSM9DS1_AXIS_Z] < m_min[LSM9DS1_AXIS_Z]) m_min[LSM9DS1_AXIS_Z] = m_vec.axis[LSM9DS1_AXIS_Z];
		if (m_vec.axis[LSM9DS1_AXIS_Z] > m_max[LSM9DS1_AXIS_Z]) m_max[LSM9DS1_AXIS_Z] = m_vec.axis[LSM9DS1_AXIS_Z];
	}
	int16_t m_bias[3];
	m_bias[LSM9DS1_AXIS_X] = (m_max[LSM9DS1_AXIS_X] + m_min[LSM9DS1_AXIS_X]) / 2;
	m_bias[LSM9DS1_AXIS_Y] = (m_max[LSM9DS1_AXIS_Y] + m_min[LSM9DS1_AXIS_Y]) / 2;
	m_bias[LSM9DS1_AXIS_Z] = (m_max[LSM9DS1_AXIS_Z] + m_min[LSM9DS1_AXIS_Z]) / 2;
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_X_REG_L_M, (uint16_t)(m_bias[LSM9DS1_AXIS_X] & 0x00FF));
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_X_REG_H_M, (uint16_t)((m_bias[LSM9DS1_AXIS_X] & 0xFF00) >> 8));
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_Y_REG_L_M, (uint16_t)(m_bias[LSM9DS1_AXIS_Y] & 0x00FF));
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_Y_REG_H_M, (uint16_t)((m_bias[LSM9DS1_AXIS_Y] & 0xFF00) >> 8));
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_Z_REG_L_M, (uint16_t)(m_bias[LSM9DS1_AXIS_Z] & 0x00FF));
	lsm9ds1_set_register(LSM9DS1_ADDR_M, LSM9DS1_REG_OFFSET_Z_REG_H_M, (uint16_t)((m_bias[LSM9DS1_AXIS_Z] & 0xFF00) >> 8));
}

bool lsm9ds1_accel_available()
{
	uint8_t status;
	lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_STATUS_REG, &status);
	return (status & 0x01);
}

bool lsm9ds1_gyro_available()
{
	uint8_t status;
	lsm9ds1_get_register(LSM9DS1_ADDR_XG, LSM9DS1_REG_STATUS_REG, &status);
	return (status & 0x02);
}

bool lsm9ds1_mag_available()
{
	uint8_t status;
	lsm9ds1_get_register(LSM9DS1_ADDR_M, LSM9DS1_REG_STATUS_REG_M, &status);
	return (status & 0x08);
}

void lsm9ds1_read_raw_accel(lsm9ds1_raw_vector_t* data)
{
	uint8_t read_buffer[6];
	lsm9ds1_get_data(LSM9DS1_ADDR_XG, LSM9DS1_REG_OUT_X_L_XL, read_buffer, 6);
	data->axis[LSM9DS1_AXIS_X] = (int16_t)(((uint16_t)read_buffer[1] << 8) | (uint16_t)read_buffer[0]);
	data->axis[LSM9DS1_AXIS_Y] = (int16_t)(((uint16_t)read_buffer[3] << 8) | (uint16_t)read_buffer[2]);
	data->axis[LSM9DS1_AXIS_Z] = (int16_t)(((uint16_t)read_buffer[5] << 8) | (uint16_t)read_buffer[4]);
}

void lsm9ds1_read_raw_gyro(lsm9ds1_raw_vector_t* data)
{
	uint8_t read_buffer[6];
	lsm9ds1_get_data(LSM9DS1_ADDR_XG, LSM9DS1_REG_OUT_X_L_G, read_buffer, 6);
	data->axis[LSM9DS1_AXIS_X] = (int16_t)(((uint16_t)read_buffer[1] << 8) | (uint16_t)read_buffer[0]);
	data->axis[LSM9DS1_AXIS_Y] = (int16_t)(((uint16_t)read_buffer[3] << 8) | (uint16_t)read_buffer[2]);
	data->axis[LSM9DS1_AXIS_Z] = (int16_t)(((uint16_t)read_buffer[5] << 8) | (uint16_t)read_buffer[4]);
}

void lsm9ds1_read_raw_mag(lsm9ds1_raw_vector_t* data)
{
	uint8_t read_buffer[6];
	lsm9ds1_get_data(LSM9DS1_ADDR_M, LSM9DS1_REG_OUT_X_L_M, read_buffer, 6);
	data->axis[LSM9DS1_AXIS_X] = (int16_t)(((uint16_t)read_buffer[1] << 8) | (uint16_t)read_buffer[0]);
	data->axis[LSM9DS1_AXIS_Y] = (int16_t)(((uint16_t)read_buffer[3] << 8) | (uint16_t)read_buffer[2]);
	data->axis[LSM9DS1_AXIS_Z] = (int16_t)(((uint16_t)read_buffer[5] << 8) | (uint16_t)read_buffer[4]);
}

i2c_error_t lsm9ds1_set_register(i2c_address_t address, uint8_t reg, uint8_t data)
{
	uint16_t timeout;
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_open(address) && --timeout)
		;
	if (!timeout) {
		return I2C_BUSY;
	}
	
	i2c_set_data_complete_callback(lsm9ds1_set_register_completed_callback, &data);
	i2c_set_buffer(&reg, 1);
	i2c_set_address_nack_callback(i2c_cb_restart_write, NULL);
	i2c_write();
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_close() && --timeout)
		;
	if (!timeout) {
		return I2C_FAIL;
	}
	
	return I2C_OK;
}

i2c_error_t lsm9ds1_get_register(i2c_address_t address, uint8_t reg, uint8_t* data)
{
	uint16_t timeout;
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_open(address) && --timeout)
		;
	if (!timeout) {
		return I2C_BUSY;
	}
	
	i2c_set_data_complete_callback(lsm9ds1_get_register_completed_callback, (void*)data);
	i2c_set_buffer(&reg, 1);
	i2c_set_address_nack_callback(i2c_cb_restart_write, NULL);
	i2c_write();
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_close() && --timeout)
		;
	if (!timeout) {
		return I2C_FAIL;
	}
	
	return I2C_OK;
}

i2c_error_t lsm9ds1_get_data(i2c_address_t address, uint8_t reg, void* data, size_t len)
{
	uint16_t timeout;
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_open(address) && --timeout)
	;
	if (!timeout) {
		return I2C_BUSY;
	}
	
	lsm9ds1_buffer_t buffer;
	buffer.data = data;
	buffer.len  = len;
	
	i2c_set_data_complete_callback(lsm9ds1_get_data_completed_callback, &buffer);
	i2c_set_buffer(&reg, 1);
	i2c_set_address_nack_callback(i2c_cb_restart_write, NULL);
	i2c_write();
	
	timeout = LSM9DS1_I2C_TIMEOUT;
	while (I2C_BUSY == i2c_close() && --timeout)
	;
	if (!timeout) {
		return I2C_FAIL;
	}
	
	return I2C_OK;
}

i2c_operations_t lsm9ds1_set_register_completed_callback(void* payload)
{
	i2c_set_buffer(payload, 1);
	i2c_set_data_complete_callback(NULL, NULL);
	return i2c_continue;
}

i2c_operations_t lsm9ds1_get_register_completed_callback(void* payload)
{
	i2c_set_buffer(payload, 1);
	i2c_set_data_complete_callback(NULL, NULL);
	return i2c_restart_read;
}

i2c_operations_t lsm9ds1_get_data_completed_callback(void* payload)
{
	i2c_set_buffer(((lsm9ds1_buffer_t*)payload)->data, ((lsm9ds1_buffer_t*)payload)->len);
	i2c_set_data_complete_callback(NULL, NULL);
	return i2c_restart_read;
}
