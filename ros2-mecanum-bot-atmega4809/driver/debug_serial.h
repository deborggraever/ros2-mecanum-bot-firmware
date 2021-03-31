/*
 * debug_serial.h
 *
 * Created: 22/03/2021 23:20:49
 *  Author: randy
 */ 


#ifndef DEBUG_SERIAL_H_
#define DEBUG_SERIAL_H_

#include <stdbool.h>
#include <stdint.h>

void debug_serial_init();
void debug_serial_tx_enable();
void debug_serial_tx_disable();
bool debug_serial_tx_ready();
void debug_serial_write_byte(const uint8_t data);
void debug_serial_write_text(const char* text);
void debug_serial_write_line(const char* text);
bool debug_serial_rx_ready();
uint8_t debug_serial_read_byte();

#endif /* DEBUG_SERIAL_H_ */