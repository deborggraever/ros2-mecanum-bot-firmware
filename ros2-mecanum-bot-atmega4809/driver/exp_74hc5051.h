/*
 * exp_74hc5051.h
 *
 * Created: 24/03/2021 23:14:58
 *  Author: randy
 */ 


#ifndef EXP_74HC5051_H_
#define EXP_74HC5051_H_

#include <stdint.h>

typedef enum {
	EXP_74H5051_CH0,
	EXP_74H5051_CH1,
	EXP_74H5051_CH2,
	EXP_74H5051_CH3,
	EXP_74H5051_CH4,
	EXP_74H5051_CH5,
	EXP_74H5051_CH6,
	EXP_74H5051_CH7,
} exp_74h5051_channel_t;

void exp_74h5051_init();
void exp_74h5051_enable();
void exp_74h5051_disable();
uint8_t exp_74h5051_get_value(exp_74h5051_channel_t channel);

#endif /* EXP_74HC5051_H_ */