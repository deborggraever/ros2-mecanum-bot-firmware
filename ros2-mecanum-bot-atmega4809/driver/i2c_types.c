/*
 * i2c_types.c
 *
 * Created: 24/03/2021 12:00:24
 *  Author: randy
 */ 

#include <driver/i2c_types.h>

i2c_operations_t i2c_cb_return_stop(void *p)
{
	return i2c_stop;
}

i2c_operations_t i2c_cb_return_reset(void *p)
{
	return i2c_reset_link;
}

i2c_operations_t i2c_cb_restart_write(void *p)
{
	return i2c_restart_write;
}

i2c_operations_t i2c_cb_restart_read(void *p)
{
	return i2c_restart_read;
}
