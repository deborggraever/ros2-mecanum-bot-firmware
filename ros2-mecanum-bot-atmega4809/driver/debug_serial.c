/*
 * debug_serial.c
 *
 * Created: 22/03/2021 23:22:30
 *  Author: randy
 */ 

#include <mcu/mcu_clock.h>
#include <mcu/mcu_port.h>
#include <driver/debug_serial.h>
#include <string.h>

#define DEBUG_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

void debug_serial_init()
{
	// Configure pins
	PORTB_init_pin(0, PORT_DIR_OUT, false);	// TX
	PORTB_init_pin(1, PORT_DIR_IN, false);	// RX
	
	// Set baud rate
	USART3.BAUD = DEBUG_BAUD_RATE(9600);
	
	// Set mode
	USART3.CTRLB = 0 << USART_MPCM_bp			// Multi-processor Communication Mode: disabled
		| 0 << USART_ODME_bp					// Open Drain Mode Enable: disabled
		| 1 << USART_RXEN_bp					// Receiver Enable: enabled
		| 1 << USART_TXEN_bp					// Transmitter Enable: enabled
		| 0 << USART_SFDEN_bp					// Start Frame Detection Enable: disabled
		| USART_RXMODE_NORMAL_gc;				// Normal mode
	USART3.CTRLC = USART_CMODE_ASYNCHRONOUS_gc	// Synchronous Mode
		| USART_CHSIZE_8BIT_gc					// Character size: 8 bit
		| USART_PMODE_DISABLED_gc				// No Parity
		| USART_SBMODE_1BIT_gc;					// 1 stop bit
}

void debug_serial_tx_enable()
{
	USART3.CTRLB = USART_TXEN_bm;
	PORTB_set_pin_dir(0, PORT_DIR_OUT);
}

void debug_serial_tx_disable()
{
	USART3.CTRLB &= ~USART_TXEN_bm;
}

bool debug_serial_tx_ready()
{
	return (USART3.STATUS & USART_DREIF_bm);
}

void debug_serial_write_byte(const uint8_t data)
{
	while (!(USART3.STATUS & USART_DREIF_bm))
	;
	USART3.TXDATAL = data;
}

void debug_serial_write_text(const char* text)
{
	uint8_t len = strlen(text);
	for (uint8_t i = 0; i < len; i++) {
		debug_serial_write_byte(text[i]);
	}
}

void debug_serial_write_line(const char* text)
{
	debug_serial_write_text(text);
	debug_serial_write_byte('\r');
	debug_serial_write_byte('\n');
}


bool debug_serial_rx_ready()
{
	return (USART3.STATUS & USART_RXCIF_bm);
}

uint8_t debug_serial_read_byte()
{
	while (!(USART3.STATUS & USART_RXCIF_bm))
	;
	return USART3.RXDATAL;
}
