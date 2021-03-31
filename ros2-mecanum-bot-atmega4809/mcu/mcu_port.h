/*
 * mcu_port.h
 *
 * Created: 23/03/2021 8:54:22
 *  Author: randy
 */ 


#ifndef MCU_PORT_H_
#define MCU_PORT_H_

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

enum PORT_PULL_MODE
{
	PORT_PULL_OFF,
	PORT_PULL_UP
};

enum PORT_DIR
{
	PORT_DIR_IN,
	PORT_DIR_OUT,
	PORT_DIR_OFF,
};

/**
 * PORT A
 */
static inline void PORTA_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTA + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
}

static inline void PORTA_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTA + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTA_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTA + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTA_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTA.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTA.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTA + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTA_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTA.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTA.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTA + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTA_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTA.OUT |= mask;
	}
	else {
		VPORTA.OUT &= ~mask;
	}
}

static inline void PORTA_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTA.OUT |= (1 << pin);
	}
	else {
		VPORTA.OUT &= ~(1 << pin);
	}
}

static inline void PORTA_toggle_port_level(const uint8_t mask)
{
	PORTA.OUTTGL = mask;
}

static inline void PORTA_toggle_pin_level(const uint8_t pin)
{
	PORTA.IN |= (1 << pin);
}

static inline uint8_t PORTA_get_port_level()
{
	return VPORTA.IN;
}

static inline bool PORTA_get_pin_level(const uint8_t pin)
{
	return VPORTA.IN & (1 << pin);
}

static inline void PORTA_write_port(const uint8_t value)
{
	VPORTA.OUT = value;
}

static inline void PORTA_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTA_set_pin_level(pin, level);
	PORTA_set_pin_dir(pin, dir);
	PORTA_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTA_set_pin_inverted(pin, false);
	PORTA_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

/**
 * PORT B
 */
static inline void PORTB_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTB + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
};

static inline void PORTB_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTB + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTB_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTB + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTB_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTB.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTB.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTB + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTB_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTB.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTB.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTB + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTB_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTB.OUT |= mask;
	}
	else {
		VPORTB.OUT &= ~mask;
	}
}

static inline void PORTB_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTB.OUT |= (1 << pin);
	}
	else {
		VPORTB.OUT &= ~(1 << pin);
	}
}

static inline void PORTB_toggle_port_level(const uint8_t mask)
{
	PORTB.OUTTGL = mask;
}

static inline void PORTB_toggle_pin_level(const uint8_t pin)
{
	PORTB.IN |= (1 << pin);
}

static inline uint8_t PORTB_get_port_level()
{
	return VPORTB.IN;
}

static inline bool PORTB_get_pin_level(const uint8_t pin)
{
	return VPORTB.IN & (1 << pin);
}

static inline void PORTB_write_port(const uint8_t value)
{
	VPORTB.OUT = value;
}

static inline void PORTB_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTB_set_pin_level(pin, level);
	PORTB_set_pin_dir(pin, dir);
	PORTB_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTB_set_pin_inverted(pin, false);
	PORTB_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

/**
 * PORT C
 */
static inline void PORTC_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTC + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
};

static inline void PORTC_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTC + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTC_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTC + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTC_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTC.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTC.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTC + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTC_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTC.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTC.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTC + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTC_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTC.OUT |= mask;
	}
	else {
		VPORTC.OUT &= ~mask;
	}
}

static inline void PORTC_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTC.OUT |= (1 << pin);
	}
	else {
		VPORTC.OUT &= ~(1 << pin);
	}
}

static inline void PORTC_toggle_port_level(const uint8_t mask)
{
	PORTC.OUTTGL = mask;
}

static inline void PORTC_toggle_pin_level(const uint8_t pin)
{
	PORTC.IN |= (1 << pin);
}

static inline uint8_t PORTC_get_port_level()
{
	return VPORTC.IN;
}

static inline bool PORTC_get_pin_level(const uint8_t pin)
{
	return VPORTC.IN & (1 << pin);
}

static inline void PORTC_write_port(const uint8_t value)
{
	VPORTC.OUT = value;
}

static inline void PORTC_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTC_set_pin_level(pin, level);
	PORTC_set_pin_dir(pin, dir);
	PORTC_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTC_set_pin_inverted(pin, false);
	PORTC_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

/**
 * PORT D
 */
static inline void PORTD_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTD + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
};

static inline void PORTD_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTD + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTD_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTD + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTD_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTD.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTD.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTD + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTD_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTD.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTD.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTD + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTD_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTD.OUT |= mask;
	}
	else {
		VPORTD.OUT &= ~mask;
	}
}

static inline void PORTD_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTD.OUT |= (1 << pin);
	}
	else {
		VPORTD.OUT &= ~(1 << pin);
	}
}

static inline void PORTD_toggle_port_level(const uint8_t mask)
{
	PORTD.OUTTGL = mask;
}

static inline void PORTD_toggle_pin_level(const uint8_t pin)
{
	PORTD.IN |= (1 << pin);
}

static inline uint8_t PORTD_get_port_level()
{
	return VPORTD.IN;
}

static inline bool PORTD_get_pin_level(const uint8_t pin)
{
	return VPORTD.IN & (1 << pin);
}

static inline void PORTD_write_port(const uint8_t value)
{
	VPORTD.OUT = value;
}

static inline void PORTD_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTD_set_pin_level(pin, level);
	PORTD_set_pin_dir(pin, dir);
	PORTD_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTD_set_pin_inverted(pin, false);
	PORTD_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

/**
 * PORT E
 */
static inline void PORTE_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTE + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
};

static inline void PORTE_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTE + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTE_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTE + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTE_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTE.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTE.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTE + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTE_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTE.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTE.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTE + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTE_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTE.OUT |= mask;
	}
	else {
		VPORTE.OUT &= ~mask;
	}
}

static inline void PORTE_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTE.OUT |= (1 << pin);
	}
	else {
		VPORTE.OUT &= ~(1 << pin);
	}
}

static inline void PORTE_toggle_port_level(const uint8_t mask)
{
	PORTE.OUTTGL = mask;
}

static inline void PORTE_toggle_pin_level(const uint8_t pin)
{
	PORTE.IN |= (1 << pin);
}

static inline uint8_t PORTE_get_port_level()
{
	return VPORTE.IN;
}

static inline bool PORTE_get_pin_level(const uint8_t pin)
{
	return VPORTE.IN & (1 << pin);
}

static inline void PORTE_write_port(const uint8_t value)
{
	VPORTE.OUT = value;
}

static inline void PORTE_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTE_set_pin_level(pin, level);
	PORTE_set_pin_dir(pin, dir);
	PORTE_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTE_set_pin_inverted(pin, false);
	PORTE_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

/**
 * PORT F
 */
static inline void PORTF_set_pin_pull_mode(const uint8_t pin, const enum PORT_PULL_MODE mode)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTF + 0x10 + pin);
	if (mode == PORT_PULL_UP) {
		*port_pin_ctrl |= PORT_PULLUPEN_bp;
	}
	else {
		*port_pin_ctrl &= ~PORT_PULLUPEN_bp;
	}
};

static inline void PORTF_set_pin_inverted(const uint8_t pin, const bool inverted)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTF + 0x10 + pin);
	if (inverted) {
		*port_pin_ctrl |= PORT_INVEN_bm;
	}
	else {
		*port_pin_ctrl &= ~PORT_INVEN_bm;
	}
}

static inline void PORTF_set_pin_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	volatile uint8_t *port_pin_ctrl = ((uint8_t *)&PORTF + 0x10 + pin);
	*port_pin_ctrl = (*port_pin_ctrl & ~PORT_ISC_gm) | isc;
}

static inline void PORTF_set_port_dir(const uint8_t mask, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTF.DIR &= ~mask;
			break;
		case PORT_DIR_OUT:
			VPORTF.DIR |= mask;
		case PORT_DIR_OFF:
			{
				for (uint8_t i = 0; i < 8; i++) {
					if (mask & 1 << i) {
						*((uint8_t *)&PORTF + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
					}
				}
			}
			break;
		default:
			break;
	}
}

static inline void PORTF_set_pin_dir(const uint8_t pin, const enum PORT_DIR dir)
{
	switch (dir) {
		case PORT_DIR_IN:
			VPORTF.DIR &= ~(1 << pin);
			break;
		case PORT_DIR_OUT:
			VPORTF.DIR |= (1 << pin);
			break;
		case PORT_DIR_OFF:
			*((uint8_t *)&PORTF + 0x10 + pin) |= 1 << PORT_PULLUPEN_bp;
			break;
		default:
			break;
	}
}

static inline void PORTF_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		VPORTF.OUT |= mask;
	}
	else {
		VPORTF.OUT &= ~mask;
	}
}

static inline void PORTF_set_pin_level(const uint8_t pin, const bool level)
{
	if (level == true) {
		VPORTF.OUT |= (1 << pin);
	}
	else {
		VPORTF.OUT &= ~(1 << pin);
	}
}

static inline void PORTF_toggle_port_level(const uint8_t mask)
{
	PORTF.OUTTGL = mask;
}

static inline void PORTF_toggle_pin_level(const uint8_t pin)
{
	PORTF.IN |= (1 << pin);
}

static inline uint8_t PORTF_get_port_level()
{
	return VPORTF.IN;
}

static inline bool PORTF_get_pin_level(const uint8_t pin)
{
	return VPORTF.IN & (1 << pin);
}

static inline void PORTF_write_port(const uint8_t value)
{
	VPORTF.OUT = value;
}

static inline void PORTF_init_pin(const uint8_t pin, const enum PORT_DIR dir, const bool level)
{
	PORTF_set_pin_level(pin, level);
	PORTF_set_pin_dir(pin, dir);
	PORTF_set_pin_pull_mode(pin, PORT_PULL_OFF);
	PORTF_set_pin_inverted(pin, false);
	PORTF_set_pin_isc(pin, PORT_ISC_INTDISABLE_gc);
}

#endif /* MCU_PORT_H_ */