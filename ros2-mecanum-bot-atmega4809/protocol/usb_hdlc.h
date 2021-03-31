/*
 * usb_hdlc.h
 *
 * Created: 25/03/2021 11:42:48
 *  Author: randy
 */ 


#ifndef USB_HDLC_H_
#define USB_HDLC_H_

#include <stdint.h>
#include <stdio.h>

#define HDLC_FRAME_BOUNDRY_FLAG     0x7E
#define HDLC_ESCAPE_FLAG            0x7D
#define HDLC_ESCAPE_XOR             0x20
#define HDLC_CRC_INIT_VALUE         0xFFFF
#define HDLC_FRAME_MAX_SIZE			100
#define HDLC_RESPONSE_ACK			0xA0
#define HDLC_RESPONSE_NACK			0xA1
#define HDLC_CRC_ERROR				0xE1
#define HDLC_BUFFER_OVERFLOW		0xE2

typedef void (*usb_hdlc_frame_callback)(uint8_t* data, size_t length);

void usb_hdlc_set_frame_callback(usb_hdlc_frame_callback cb);
void usb_hdlc_encode_byte(uint8_t data);
void usb_hdlc_decode_byte(uint8_t data);
void usb_hdlc_write(void* buffer, size_t length);
void usb_hdlc_read();
void usb_hdlc_ack();
void usb_hdlc_nack(uint8_t code);

#endif /* USB_HDLC_H_ */