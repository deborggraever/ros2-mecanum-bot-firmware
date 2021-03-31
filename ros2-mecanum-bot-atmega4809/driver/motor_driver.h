/*
 * motor_driver.h
 *
 * Created: 24/03/2021 21:40:49
 *  Author: randy
 */ 


#ifndef MOTOR_DRIVER_H_
#define MOTOR_DRIVER_H_

#include <stdint.h>

#define MOTOR_MAX_DUTY 255
#define L298N

typedef enum {
	MOTOR_DIR_NONE = 0,
	MOTOR_DIR_FWD,
	MOTOR_DIR_BWD,
} motor_direction_t;

typedef enum {
	MOTOR_FL = 1,
	MOTOR_FR = 2,
	MOTOR_RL = 4,
	MOTOR_RR = 8,
} motor_channel_t;

void motor_driver_init();
void motor_enable(motor_channel_t channels);
void motor_disable(motor_channel_t channels);
void motor_set_direction(motor_channel_t channels, motor_direction_t dir);
void motor_set_duty(motor_channel_t channels, uint16_t duty);
void motor_brake(motor_channel_t channels);
void motor_stop(motor_channel_t channels);

#endif /* MOTOR_DRIVER_H_ */