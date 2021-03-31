/*
 * motor_driver.c
 *
 * Created: 24/03/2021 21:41:33
 *  Author: randy
 */ 

#include <avr/io.h>
#include <mcu/mcu_port.h>
#include <driver/motor_driver.h>

void motor_driver_init()
{
	// Initialize FL motor
	PORTC_init_pin(0, PORT_DIR_OUT, false);	// PWM
	PORTC_init_pin(1, PORT_DIR_OUT, false);	// DIR
#ifdef L298N
	PORTB_init_pin(2, PORT_DIR_OUT, false);	// L298N ALT
#endif
	TCB2.CCMPL = MOTOR_MAX_DUTY;
	TCB2.CTRLA |= TCB_ENABLE_bm;
	TCB2.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB2.CTRLB |= TCB_CCMPEN_bm;
	TCB2.CTRLB |= TCB_CNTMODE_PWM8_gc;
	
	// Initialize FR motor
	PORTB_init_pin(5, PORT_DIR_OUT, false);	// PWM
	PORTB_init_pin(4, PORT_DIR_OUT, false);	// DIR
	#ifdef L298N
	PORTB_init_pin(3, PORT_DIR_OUT, false);	// L298N ALT
	#endif
	TCB3.CCMPL = MOTOR_MAX_DUTY;
	TCB3.CTRLA |= TCB_ENABLE_bm;
	TCB3.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB3.CTRLB |= TCB_CCMPEN_bm;
	TCB3.CTRLB |= TCB_CNTMODE_PWM8_gc;
	
	// Initialize RL motor
	PORTA_init_pin(3, PORT_DIR_OUT, false);	// PWM
	PORTA_init_pin(4, PORT_DIR_OUT, false);	// DIR
	#ifdef L298N
	PORTA_init_pin(6, PORT_DIR_OUT, false);	// L298N ALT
	#endif
	TCB1.CCMPL = MOTOR_MAX_DUTY;
	TCB1.CTRLA |= TCB_ENABLE_bm;
	TCB1.CTRLA |= TCB_CLKSEL_CLKDIV1_gc;
	TCB1.CTRLB |= TCB_CCMPEN_bm;
	TCB1.CTRLB |= TCB_CNTMODE_PWM8_gc;
	
	// Initialize RR motor
	PORTA_init_pin(2, PORT_DIR_OUT, false);	// PWM
	PORTA_init_pin(1, PORT_DIR_OUT, false);	// DIR
	#ifdef L298N
	PORTA_init_pin(5, PORT_DIR_OUT, false);	// L298N ALT
	#endif
	TCB0.CCMPL = MOTOR_MAX_DUTY;
	TCB0.CTRLA |= TCB_ENABLE_bm;
	TCB0.CTRLA |= TCB_CLKSEL_CLKDIV2_gc;
	TCB0.CTRLB |= TCB_CCMPEN_bm;
	TCB0.CTRLB |= TCB_CNTMODE_PWM8_gc;
}

void motor_enable(motor_channel_t channels)
{
	if ((channels & MOTOR_FL) == MOTOR_FL) {
		TCB2.CTRLA |= TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_FR) == MOTOR_FR) {
		TCB3.CTRLA |= TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_RL) == MOTOR_RL) {
		TCB1.CTRLA |= TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_RR) == MOTOR_RR) {
		TCB0.CTRLA |= TCB_ENABLE_bm;
	}
}

void motor_disable(motor_channel_t channels)
{
	if ((channels & MOTOR_FL) == MOTOR_FL) {
		TCB2.CTRLA &= ~TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_FR) == MOTOR_FR) {
		TCB3.CTRLA &= ~TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_RL) == MOTOR_RL) {
		TCB1.CTRLA &= ~TCB_ENABLE_bm;
	}
	if ((channels & MOTOR_RR) == MOTOR_RR) {
		TCB0.CTRLA &= ~TCB_ENABLE_bm;
	}
}

void motor_set_direction(motor_channel_t channels, motor_direction_t dir)
{
	if ((channels & MOTOR_FL) == MOTOR_FL) {
		switch (dir) {
			case MOTOR_DIR_FWD:
				PORTC_set_pin_level(1, true);
				#ifdef L298N
				PORTB_set_pin_level(2, false);
				#endif
				break;
			case MOTOR_DIR_BWD:
				PORTC_set_pin_level(1, false);
				#ifdef L298N
				PORTB_set_pin_level(2, true);
				#endif
				break;
			default:
				PORTC_set_pin_level(1, false);
				#ifdef L298N
				PORTB_set_pin_level(2, false);
				#endif
				break;
		}
	}
	if ((channels & MOTOR_FR) == MOTOR_FR) {
		switch (dir) {
			case MOTOR_DIR_FWD:
				PORTB_set_pin_level(4, true);
				#ifdef L298N
				PORTB_set_pin_level(3, false);
				#endif
				break;
			case MOTOR_DIR_BWD:
				PORTB_set_pin_level(4, false);
				#ifdef L298N
				PORTB_set_pin_level(3, true);
				#endif
				break;
			default:
				PORTB_set_pin_level(4, false);
				#ifdef L298N
				PORTB_set_pin_level(3, false);
				#endif
				break;
		}
	}
	if ((channels & MOTOR_RL) == MOTOR_RL) {
		switch (dir) {
			case MOTOR_DIR_FWD:
				PORTA_set_pin_level(4, true);
				#ifdef L298N
				PORTA_set_pin_level(6, false);
				#endif
				break;
			case MOTOR_DIR_BWD:
				PORTA_set_pin_level(4, false);
				#ifdef L298N
				PORTA_set_pin_level(6, true);
				#endif
				break;
			default:
				PORTA_set_pin_level(4, false);
				#ifdef L298N
				PORTA_set_pin_level(6, false);
				#endif
				break;
		}
	}
	if ((channels & MOTOR_RR) == MOTOR_RR) {
		switch (dir) {
			case MOTOR_DIR_FWD:
				PORTA_set_pin_level(1, true);
				#ifdef L298N
				PORTA_set_pin_level(5, false);
				#endif
				break;
			case MOTOR_DIR_BWD:
				PORTA_set_pin_level(1, false);
				#ifdef L298N
				PORTA_set_pin_level(5, true);
				#endif
				break;
			default:
				PORTA_set_pin_level(1, false);
				#ifdef L298N
				PORTA_set_pin_level(5, false);
				#endif
				break;
		}
	}
}

void motor_set_duty(motor_channel_t channels, uint16_t duty)
{
	if (duty > MOTOR_MAX_DUTY) {
		duty = MOTOR_MAX_DUTY;
	}
	if ((channels & MOTOR_FL) == MOTOR_FL) {
		TCB2.CCMPL = MOTOR_MAX_DUTY;
		TCB2.CCMPH = duty;
		TCB2.CNT   = 0;
	}
	if ((channels & MOTOR_FR) == MOTOR_FR) {
		TCB3.CCMPL = MOTOR_MAX_DUTY;
		TCB3.CCMPH = duty;
		TCB3.CNT   = 0;
	}
	if ((channels & MOTOR_RL) == MOTOR_RL) {
		TCB1.CCMPL = MOTOR_MAX_DUTY;
		TCB1.CCMPH = duty;
		TCB1.CNT   = 0;
	}
	if ((channels & MOTOR_RR) == MOTOR_RR) {
		TCB0.CCMPL = MOTOR_MAX_DUTY;
		TCB0.CCMPH = duty;
		TCB0.CNT   = 0;
	}
}

void motor_brake(motor_channel_t channels)
{
	motor_set_direction(channels, MOTOR_DIR_NONE);
	motor_set_duty(channels, 0);
}

void motor_stop(motor_channel_t channels)
{
	motor_set_direction(channels, MOTOR_DIR_NONE);
	motor_set_duty(channels, 0);
}