/*
 * mcu.c
 *
 * Created: 23/03/2021 10:35:25
 *  Author: randy
 */ 

#include <mcu/mcu.h>
#include <avr/io.h>
#include <stdint.h>

void mcu_init()
{
	// Disable all ports
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTA + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTB + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTC + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTD + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTE + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
	for (uint8_t i = 0; i < 8; i++) {
		*((uint8_t *)&PORTF + 0x10 + i) |= 1 << PORT_PULLUPEN_bp;
	}
}