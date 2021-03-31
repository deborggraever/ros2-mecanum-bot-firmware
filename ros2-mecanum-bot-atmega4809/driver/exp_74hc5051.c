/*
 * exp_74hc5051.c
 *
 * Created: 24/03/2021 23:16:23
 *  Author: randy
 */ 

#include <mcu/mcu_port.h>
#include <driver/exp_74hc5051.h>

void exp_74h5051_init()
{
	PORTD_init_pin(1, PORT_DIR_IN, false);	// A
	PORTD_init_pin(2, PORT_DIR_OUT, false);	// EN
	PORTD_init_pin(0, PORT_DIR_OUT, false);	// S0
	PORTC_init_pin(7, PORT_DIR_OUT, false); // S1
	PORTC_init_pin(6, PORT_DIR_OUT, false);	// S2
}

void exp_74h5051_enable()
{
	PORTD_set_pin_level(2, false);
}

void exp_74h5051_disable()
{
	PORTD_set_pin_level(2, true);
}

uint8_t exp_74h5051_get_value(exp_74h5051_channel_t channel)
{
	switch (channel) {
		case EXP_74H5051_CH4:
		case EXP_74H5051_CH5:
		case EXP_74H5051_CH6:
		case EXP_74H5051_CH7:
			PORTD_set_pin_level(0, true);
			break;
		case EXP_74H5051_CH0:
		case EXP_74H5051_CH1:
		case EXP_74H5051_CH2:
		case EXP_74H5051_CH3:
		default:
			PORTD_set_pin_level(0, false);
			break;
	}
	
	switch (channel) {
		case EXP_74H5051_CH2:
		case EXP_74H5051_CH3:
		case EXP_74H5051_CH6:
		case EXP_74H5051_CH7:
			PORTC_set_pin_level(7, true);
			break;
		case EXP_74H5051_CH0:
		case EXP_74H5051_CH1:
		case EXP_74H5051_CH4:
		case EXP_74H5051_CH5:
		default:
			PORTC_set_pin_level(7, false);
			break;
	}
	
	switch (channel) {
		case EXP_74H5051_CH1:
		case EXP_74H5051_CH3:
		case EXP_74H5051_CH5:
		case EXP_74H5051_CH7:
			PORTC_set_pin_level(6, true);
			break;
		case EXP_74H5051_CH0:
		case EXP_74H5051_CH2:
		case EXP_74H5051_CH4:
		case EXP_74H5051_CH6:
		default:
			PORTC_set_pin_level(6, false);
			break;
	}
	
	return 0;
}