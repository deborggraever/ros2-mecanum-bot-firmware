/*
 * usb_hdlc.c
 *
 * Created: 25/03/2021 11:42:59
 *  Author: randy
 */ 

#include <protocol/usb_hdlc.h>
#include <driver/usb_serial.h>
#include <util/crc16.h>
#include <stdbool.h>

typedef struct {
	usb_hdlc_frame_callback frame_callback;
	uint8_t                 frame_buffer[HDLC_FRAME_MAX_SIZE];
	uint8_t                 frame_offset;
	uint16_t                frame_checksum;
	bool                    escape_byte;
} usb_hdlc_rx_state_t;

usb_hdlc_rx_state_t _usb_hdlc_rx_state = {0};

void usb_hdlc_set_frame_callback(usb_hdlc_frame_callback cb)
{
	_usb_hdlc_rx_state.frame_callback = cb;
}

void usb_hdlc_encode_byte(uint8_t data)
{
	if ((data == HDLC_ESCAPE_FLAG) || (data == HDLC_FRAME_BOUNDRY_FLAG)) {
		usb_write(HDLC_ESCAPE_FLAG);
		data ^= HDLC_ESCAPE_XOR;
	}
	usb_write(data);
}

void usb_hdlc_decode_byte(uint8_t data)
{
	if (data == HDLC_FRAME_BOUNDRY_FLAG) {
		if (_usb_hdlc_rx_state.escape_byte) {
			_usb_hdlc_rx_state.escape_byte = false;
		}
		else if (_usb_hdlc_rx_state.frame_offset >= 2 && _usb_hdlc_rx_state.frame_checksum == ((_usb_hdlc_rx_state.frame_buffer[_usb_hdlc_rx_state.frame_offset - 1] << 8) | _usb_hdlc_rx_state.frame_buffer[_usb_hdlc_rx_state.frame_offset - 2])) {
			usb_hdlc_ack();
			if (_usb_hdlc_rx_state.frame_callback != NULL) {
				_usb_hdlc_rx_state.frame_callback(_usb_hdlc_rx_state.frame_buffer, _usb_hdlc_rx_state.frame_offset - 2);
			}
		}
		else if (_usb_hdlc_rx_state.frame_offset >= 2) {
			usb_hdlc_nack(HDLC_CRC_ERROR);
		}
		_usb_hdlc_rx_state.frame_offset = 0;
		_usb_hdlc_rx_state.frame_checksum = HDLC_CRC_INIT_VALUE;
		return;
	}
	if (data == HDLC_ESCAPE_FLAG) {
		_usb_hdlc_rx_state.escape_byte = true;
		return;
	}
	
	if (_usb_hdlc_rx_state.escape_byte) {
		_usb_hdlc_rx_state.escape_byte = false;
		data ^= HDLC_ESCAPE_XOR;
	}
	
	_usb_hdlc_rx_state.frame_buffer[_usb_hdlc_rx_state.frame_offset] = data;
	if (_usb_hdlc_rx_state.frame_offset >= 2) {
		_usb_hdlc_rx_state.frame_checksum = _crc_ccitt_update(_usb_hdlc_rx_state.frame_checksum, _usb_hdlc_rx_state.frame_buffer[_usb_hdlc_rx_state.frame_offset - 2]);
	}
	_usb_hdlc_rx_state.frame_offset++;
	
	if (_usb_hdlc_rx_state.frame_offset >= HDLC_FRAME_MAX_SIZE) {
		_usb_hdlc_rx_state.frame_offset = 0;
		_usb_hdlc_rx_state.frame_checksum = HDLC_CRC_INIT_VALUE;
		usb_hdlc_nack(HDLC_BUFFER_OVERFLOW);
	}
}

void usb_hdlc_write(void* buffer, size_t length)
{
	uint16_t checksum = HDLC_CRC_INIT_VALUE;
	uint8_t* data = (uint8_t*)buffer;
	
	usb_write(HDLC_FRAME_BOUNDRY_FLAG);
	while (length--) {
		checksum = _crc_ccitt_update(checksum, *data);
		usb_hdlc_encode_byte(*data);
		data++;
	}
	usb_hdlc_encode_byte(checksum & 0xFF);
	usb_hdlc_encode_byte((checksum >> 8) & 0xFF);
	usb_write(HDLC_FRAME_BOUNDRY_FLAG);
}

void usb_hdlc_read()
{
	while (usb_rx_ready()) {
		usb_hdlc_decode_byte(usb_read());
	}
}

void usb_hdlc_ack()
{
	uint8_t data = HDLC_RESPONSE_ACK;
	usb_hdlc_write(&data, 1);
}

void usb_hdlc_nack(uint8_t code)
{
	uint8_t buffer[2];
	buffer[0] = HDLC_RESPONSE_NACK;
	buffer[1] = code;
	usb_hdlc_write(buffer, 2);
}