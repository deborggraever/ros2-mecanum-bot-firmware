/*
 * usb_serial.h
 *
 * Created: 22/03/2021 21:53:35
 *  Author: randy
 */ 


#ifndef USB_SERIAL_H_
#define USB_SERIAL_H_

#include <stdbool.h>
#include <stdint.h>

void usb_init();
void usb_enable();
void usb_enable_rx();
void usb_enable_tx();
void usb_disable();
bool usb_rx_ready();
bool usb_tx_ready();
bool usb_tx_busy();
uint8_t usb_read();
void usb_write(const uint8_t data);

#endif /* USB_SERIAL_H_ */