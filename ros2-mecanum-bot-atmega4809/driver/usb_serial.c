/*
 * usb_serial.c
 *
 * Created: 22/03/2021 21:56:21
 *  Author: randy
 */ 

#include <util/compiler.h>
#include <util/atomic.h>
#include <mcu/mcu_clock.h>
#include <mcu/mcu_port.h>
#include <driver/usb_serial.h>

#define USB_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#define USB_RX_BUFFER_SIZE	128
#define USB_TX_BUFFER_SIZE	128
#define USB_RX_BUFFER_MASK	(USB_RX_BUFFER_SIZE - 1)
#define USB_TX_BUFFER_MASK	(USB_TX_BUFFER_SIZE - 1)

#if (USB_RX_BUFFER_SIZE & USB_RX_BUFFER_MASK)
#	error RX buffer size is not a power of 2
#endif
#if (USB_TX_BUFFER_SIZE & USB_TX_BUFFER_MASK)
#	error TX buffer size is not a power of 2
#endif

static uint8_t			_usb_rx_buffer[USB_RX_BUFFER_SIZE];
static volatile uint8_t	_usb_rx_buffer_head;
static volatile uint8_t _usb_rx_buffer_tail;
static volatile uint8_t _usb_rx_elements;
static uint8_t			_usb_tx_buffer[USB_TX_BUFFER_SIZE];
static volatile uint8_t	_usb_tx_buffer_head;
static volatile uint8_t _usb_tx_buffer_tail;
static volatile uint8_t _usb_tx_elements;

ISR(USART1_RXC_vect)
{
	uint8_t data = USART1.RXDATAL;
	uint8_t head = (_usb_rx_buffer_head + 1) & USB_RX_BUFFER_MASK;
	if (head == _usb_rx_buffer_tail) {
		// Buffer overflow
	}
	else {
		_usb_rx_buffer_head = head;
		_usb_rx_buffer[head] = data;
		_usb_rx_elements++;
	}
}

ISR(USART1_DRE_vect)
{
	if (_usb_tx_elements != 0) {
		uint8_t tail = (_usb_tx_buffer_tail + 1) & USB_TX_BUFFER_MASK;
		_usb_tx_buffer_tail = tail;
		USART1.TXDATAL = _usb_tx_buffer[tail];
		_usb_tx_elements--;
	}
	if (_usb_tx_elements == 0) {
		USART1.CTRLA &= ~(1 << USART_DREIE_bp);
	}
}

void usb_init()
{
	// Configure pins
	PORTC_set_pin_dir(5, PORT_DIR_IN);
	PORTC_set_pin_pull_mode(5, PORT_PULL_OFF);
	PORTC_set_pin_level(4, false);
	PORTC_set_pin_dir(4, PORT_DIR_OUT);
	PORTMUX.USARTROUTEA |= PORTMUX_USART10_bm;
	
	// Set baud rate
	USART1.BAUD = (uint16_t)USB_BAUD_RATE(9600);
	
	USART1.CTRLA = 0 << USART_ABEIE_bp			// Auto-baud Error Interrupt Enable: disabled
		| 0 << USART_DREIE_bp					// Data Register Empty Interrupt Enable: disabled
		| 0 << USART_LBME_bp					// Loop-back Mode Enable: disabled
		| 1 << USART_RXCIE_bp					// Receive Complete Interrupt Enable: enabled
		| 0 << USART_RXSIE_bp					// Receiver Start Frame Interrupt Enable: disabled
		| 0 << USART_TXCIE_bp					// Transmit Complete Interrupt Enable: disabled
		| USART_RS485_OFF_gc;					// RS485 Mode disabled
	USART1.CTRLB = 0 << USART_MPCM_bp			// Multi-processor Communication Mode: disabled
		| 0 << USART_ODME_bp					// Open Drain Mode Enable: disabled
		| 1 << USART_RXEN_bp					// Receiver Enable: enabled
		| 1 << USART_TXEN_bp					// Transmitter Enable: enabled
		| 0 << USART_SFDEN_bp					// Start Frame Detection Enable: disabled
		| USART_RXMODE_NORMAL_gc;				// Normal mode
	USART1.CTRLC = USART_CMODE_ASYNCHRONOUS_gc	// Asynchronous Mode
		| USART_CHSIZE_8BIT_gc					// Character size: 8 bit
		| USART_PMODE_DISABLED_gc				// No Parity
		| USART_SBMODE_1BIT_gc;					// 1 stop bit
		
	_usb_rx_buffer_head = 0;
	_usb_rx_buffer_tail = 0;
	_usb_rx_elements    = 0;
	_usb_tx_buffer_head = 0;
	_usb_tx_buffer_tail = 0;
	_usb_tx_elements    = 0;
}

void usb_enable()
{
	USART1.CTRLB |= USART_RXEN_bm | USART_TXEN_bm;
}

void usb_enable_rx()
{
	USART1.CTRLB |= USART_RXEN_bm;
}

void usb_enable_tx()
{
	USART1.CTRLB |= USART_TXEN_bm;
}

void usb_disable()
{
	USART1.CTRLB &= ~(USART_RXEN_bm | USART_TXEN_bm);
}

bool usb_rx_ready()
{
	return (_usb_rx_elements != 0);
}

bool usb_tx_ready()
{
	return (_usb_tx_elements != USB_TX_BUFFER_SIZE);
}

bool usb_tx_busy()
{
	return (!(USART1.STATUS & USART_TXCIF_bm));
}

uint8_t usb_read()
{
	while (_usb_rx_elements == 0)
		;
	uint8_t tail = (_usb_rx_buffer_tail + 1) & USB_RX_BUFFER_MASK;
	_usb_rx_buffer_tail = tail;
	ENTER_CRITICAL(R);
	_usb_rx_elements--;
	LEAVE_CRITICAL(R);
	return _usb_rx_buffer[tail];
}

void usb_write(const uint8_t data)
{
	while (_usb_tx_elements == USB_TX_BUFFER_SIZE)
		;
	uint8_t head = (_usb_tx_buffer_head + 1) & USB_TX_BUFFER_MASK;
	_usb_tx_buffer_head = head;
	_usb_tx_buffer[head] = data;
	ENTER_CRITICAL(W);
	_usb_tx_elements++;
	LEAVE_CRITICAL(W);
	USART1.CTRLA |= (1 << USART_DREIE_bp);
}
