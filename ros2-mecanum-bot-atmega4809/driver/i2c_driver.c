/*
 * i2c_master.c
 *
 * Created: 24/03/2021 12:00:32
 *  Author: randy
 */ 

#include <mcu/mcu_port.h>
#include <driver/i2c_driver.h>
#include <util/delay.h>

#define _I2C_BAUD(F_SCL, T_RISE) ((((((float)F_CPU / (float)F_SCL)) - 10 - ((float)F_CPU * T_RISE / 1000000))) / 2)

typedef enum {
	I2C_STATE_IDLE = 0,
	I2C_STATE_SEND_ADR_READ,
	I2C_STATE_SEND_ADR_WRITE,
	I2C_STATE_TX,
	I2C_STATE_RX,
	I2C_STATE_TX_EMPTY,
	I2C_STATE_SEND_RESTART_READ,
	I2C_STATE_SEND_RESTART_WRITE,
	I2C_STATE_SEND_RESTART,
	I2C_STATE_SEND_STOP,
	I2C_STATE_RX_DO_ACK,
	I2C_STATE_TX_DO_ACK,
	I2C_STATE_RX_DO_NACK_STOP,
	I2C_STATE_RX_DO_NACK_RESTART,
	I2C_STATE_RESET,
	I2C_STATE_ADDRESS_NACK,
	I2C_STATE_BUS_COLLISION,
	I2C_STATE_BUS_ERROR
} i2c_fsm_states_t;

typedef enum {
	I2C_CB_DATACOMPLETE = 0,
	I2C_CB_WRITECOLLISION,
	I2C_CB_ADDRESSNACK,
	I2C_CB_DATANACK,
	I2C_CB_TIMEOUT,
	I2C_CB_NULL
} i2c_callback_index;

typedef struct {
	unsigned         open : 1;
	unsigned         busy : 1;
	unsigned         bufferFree : 1;
	unsigned         checkAddressNACK : 1;
	i2c_address_t    address;
	uint8_t*		 data_ptr;
	size_t           data_length;
	uint16_t         timeout;
	uint16_t         timeout_value;
	i2c_fsm_states_t state;
	i2c_error_t      error;
	i2c_callback	 callbackTable[6];
	void*			 callbackPayload[6];
} i2c_status_t;

void _i2c_polling();
void _i2c_set_callback(i2c_callback_index idx, i2c_callback cb, void* payload);
void _i2c_handle_state();
i2c_operations_t _i2c_return_stop(void* payload);
i2c_operations_t _i2c_return_reset(void* payload);
i2c_fsm_states_t _i2c_handle_I2C_STATE_IDLE();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_ADR_READ();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_ADR_WRITE();
i2c_fsm_states_t _i2c_handle_I2C_STATE_TX();
i2c_fsm_states_t _i2c_handle_I2C_STATE_RX();
i2c_fsm_states_t _i2c_handle_I2C_STATE_TX_EMPTY();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART_READ();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART();
i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_STOP();
i2c_fsm_states_t _i2c_handle_I2C_STATE_RX_DO_ACK();
i2c_fsm_states_t _i2c_handle_I2C_STATE_TX_DO_ACK();
i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_NACK_STOP();
i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_NACK_RESTART();
i2c_fsm_states_t _i2c_handle_I2C_STATE_RESET();
i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_ADDRESS_NACK();
i2c_fsm_states_t _i2c_handle_I2C_STATE_BUS_COLLISION();
i2c_fsm_states_t _i2c_handle_I2C_STATE_BUS_ERROR();

typedef i2c_fsm_states_t(i2c_stateHandlerFunction)(void);

i2c_stateHandlerFunction* _i2c_fsm_state_handlers[] = {
	_i2c_handle_I2C_STATE_IDLE,               // I2C_STATE_IDLE
	_i2c_handle_I2C_STATE_SEND_ADR_READ,      // I2C_STATE_SEND_ADR_READ
	_i2c_handle_I2C_STATE_SEND_ADR_WRITE,     // I2C_STATE_SEND_ADR_WRITE
	_i2c_handle_I2C_STATE_TX,                 // I2C_STATE_TX
	_i2c_handle_I2C_STATE_RX,                 // I2C_STATE_RX
	_i2c_handle_I2C_STATE_TX_EMPTY,           // I2C_STATE_TX_EMPTY
	_i2c_handle_I2C_STATE_SEND_RESTART_READ,  // I2C_STATE_SEND_RESTART_READ
	_i2c_handle_I2C_STATE_SEND_RESTART_WRITE, // I2C_STATE_SEND_RESTART_WRITE
	_i2c_handle_I2C_STATE_SEND_RESTART,       // I2C_STATE_SEND_RESTART
	_i2c_handle_I2C_STATE_SEND_STOP,          // I2C_STATE_SEND_STOP
	_i2c_handle_I2C_STATE_RX_DO_ACK,          // I2C_STATE_RX_DO_ACK
	_i2c_handle_I2C_STATE_TX_DO_ACK,          // I2C_STATE_TX_DO_ACK
	_i2c_handle_I2C_STATE_DO_NACK_STOP,       // I2C_STATE_RX_DO_NACK_STOP
	_i2c_handle_I2C_STATE_DO_NACK_RESTART,    // I2C_STATE_RX_DO_NACK_RESTART
	_i2c_handle_I2C_STATE_RESET,              // I2C_STATE_RESET
	_i2c_handle_I2C_STATE_DO_ADDRESS_NACK,    // I2C_STATE_ADDRESS_NACK
	_i2c_handle_I2C_STATE_BUS_COLLISION,      // I2C_STATE_BUS_COLLISION
	_i2c_handle_I2C_STATE_BUS_ERROR           // I2C_STATE_BUS_ERROR
};

i2c_status_t _i2c_status = {0};

void i2c_init()
{
	// Configure pins
	PORTC_init_pin(2, PORT_DIR_OUT, false);	// SDA
	PORTC_init_pin(3, PORT_DIR_OUT, false);	// SCL
	PORTMUX.TWISPIROUTEA |= PORTMUX_TWI01_bm;
	
	// Set baud rate
	TWI0.MBAUD = (uint8_t)_I2C_BAUD(100000, 0);
	
	// Set mode
	TWI0.MCTRLA = 1 << TWI_ENABLE_bp	// Enable TWI Master: enabled
		| 0 << TWI_RIEN_bp				// Read Interrupt Enable: disabled
		| 0 << TWI_WIEN_bp				// Write Interrupt Enable: disabled
		| 0 << TWI_QCEN_bp				// Quick Command Enable: disabled
		| 0 << TWI_SMEN_bp				// Smart Mode Enable: disabled
		| TWI_TIMEOUT_50US_gc;			// 50 Microseconds
}

i2c_error_t i2c_open(i2c_address_t address)
{
	if (_i2c_status.open) {
		return I2C_BUSY;
	}
	
	_i2c_status.address          = address;
	_i2c_status.open             = 1;
	_i2c_status.busy             = 0;
	_i2c_status.state            = I2C_STATE_RESET;
	_i2c_status.checkAddressNACK = 0;
	_i2c_status.timeout_value    = 500;
	_i2c_status.bufferFree       = 1;
	
	_i2c_status.callbackTable[I2C_CB_DATACOMPLETE]     = _i2c_return_stop;
	_i2c_status.callbackPayload[I2C_CB_DATACOMPLETE]   = NULL;
	_i2c_status.callbackTable[I2C_CB_WRITECOLLISION]   = _i2c_return_stop;
	_i2c_status.callbackPayload[I2C_CB_WRITECOLLISION] = NULL;
	_i2c_status.callbackTable[I2C_CB_ADDRESSNACK]      = _i2c_return_stop;
	_i2c_status.callbackPayload[I2C_CB_ADDRESSNACK]    = NULL;
	_i2c_status.callbackTable[I2C_CB_DATANACK]         = _i2c_return_stop;
	_i2c_status.callbackPayload[I2C_CB_DATANACK]       = NULL;
	_i2c_status.callbackTable[I2C_CB_TIMEOUT]          = _i2c_return_stop;
	_i2c_status.callbackPayload[I2C_CB_TIMEOUT]        = NULL;
	
	TWI0.MCTRLB |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	
	return I2C_OK;
}

i2c_error_t i2c_close()
{
	if (TWI0.MSTATUS & TWI_BUSERR_bm) {
		_i2c_status.busy = 0;
		_i2c_status.error = I2C_FAIL;
	}
	if (_i2c_status.busy == 0) {
		_i2c_status.open = 0;
		_i2c_status.address = 0xFF;
		TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
		TWI0.MCTRLA &= ~(TWI_RIEN_bm | TWI_WIEN_bm);
		return _i2c_status.error;
	}
	return I2C_BUSY;
}

i2c_error_t i2c_transmit(bool read)
{
	if (_i2c_status.busy) {
		return I2C_BUSY;
	}
	
	_i2c_status.busy = 1;
	if (read) {
		_i2c_status.state = I2C_STATE_SEND_ADR_READ;
	}
	else {
		_i2c_status.state = I2C_STATE_SEND_ADR_WRITE;
	}
	_i2c_handle_state();
	_i2c_polling();
	
	return I2C_OK;
}

i2c_error_t i2c_write()
{
	return i2c_transmit(false);
}

i2c_error_t i2c_read()
{
	return i2c_transmit(true);
}

i2c_error_t i2c_set_buffer(void* buffer, size_t size)
{
	if (_i2c_status.bufferFree == 1) {
		_i2c_status.data_ptr    = buffer;
		_i2c_status.data_length = size;
		_i2c_status.bufferFree  = 0;
		return I2C_OK;
	}
	return I2C_FAIL;
}

void i2c_set_address(i2c_address_t address)
{
	_i2c_status.address = address;
}

void i2c_set_timeout(uint8_t timeout)
{
	TWI0.MCTRLA &= ~(TWI_RIEN_bm | TWI_WIEN_bm);
	_i2c_status.timeout_value = timeout;
	TWI0.MCTRLA |= (TWI_RIEN_bm | TWI_WIEN_bm);
}

void i2c_set_baud_rate(uint32_t baud)
{
	TWI0.MBAUD = (uint8_t)_I2C_BAUD(baud, 0);
}

void i2c_set_data_complete_callback(i2c_callback cb, void* payload)
{
	_i2c_set_callback(I2C_CB_DATACOMPLETE, cb, payload);
}

void i2c_set_write_collision_callback(i2c_callback cb, void* payload)
{
	_i2c_set_callback(I2C_CB_WRITECOLLISION, cb, payload);
}

void i2c_set_address_nack_callback(i2c_callback cb, void* payload)
{
	_i2c_set_callback(I2C_CB_ADDRESSNACK, cb, payload);
}

void i2c_set_data_nack_callback(i2c_callback cb, void* payload)
{
	_i2c_set_callback(I2C_CB_DATANACK, cb, payload);
}

void i2c_set_timeout_callback(i2c_callback cb, void* payload)
{
	_i2c_set_callback(I2C_CB_TIMEOUT, cb, payload);
}

//---------------------------------------------------------
// Internal
//---------------------------------------------------------
void _i2c_polling()
{
	while (_i2c_status.busy) {
		_delay_ms(10);
		while (!(TWI0.MSTATUS & TWI_RIF_bm) && !(TWI0.MSTATUS & TWI_WIF_bm))
			;
		_delay_ms(10);
		_i2c_handle_state();
	}
}

void _i2c_set_callback(i2c_callback_index idx, i2c_callback cb, void* payload)
{
	if (cb == NULL) {
		_i2c_status.callbackTable[idx] = _i2c_return_stop;
		_i2c_status.callbackPayload[idx] = NULL;
	}
	else {
		_i2c_status.callbackTable[idx] = cb;
		_i2c_status.callbackPayload[idx] = payload;
	}
}

void _i2c_handle_state()
{
	TWI0.MSTATUS |= (TWI_RIF_bm | TWI_WIF_bm);
	if (_i2c_status.checkAddressNACK && (TWI0.MSTATUS & TWI_RXACK_bm)) {
		_i2c_status.state = I2C_STATE_ADDRESS_NACK;
	}
	if (TWI0.MSTATUS & TWI_ARBLOST_bm) {
		_i2c_status.state = I2C_STATE_BUS_COLLISION;
	}
	if (TWI0.MSTATUS & TWI_BUSERR_bm) {
		_i2c_status.state = I2C_STATE_BUS_ERROR;
	}
	_i2c_status.state = _i2c_fsm_state_handlers[_i2c_status.state]();
}

i2c_operations_t _i2c_return_stop(void* payload)
{
	return i2c_stop;
}

i2c_operations_t _i2c_return_reset(void* payload)
{
	return i2c_reset_link;
}

//---------------------------------------------------------
// State actions
//---------------------------------------------------------
i2c_fsm_states_t _i2c_handle_I2C_STATE_IDLE()
{
	_i2c_status.busy = 0;
	_i2c_status.error = I2C_OK;
	return I2C_STATE_IDLE;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_ADR_READ()
{
	_i2c_status.checkAddressNACK = 1;
	TWI0.MADDR = _i2c_status.address << 1 | 1;
	return I2C_STATE_RX;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_ADR_WRITE()
{
	_i2c_status.checkAddressNACK = 1;
	TWI0.MADDR = _i2c_status.address << 1;
	return I2C_STATE_TX;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_TX()
{
	if ((TWI0.MSTATUS & TWI_RXACK_bm)) {
		switch (_i2c_status.callbackTable[I2C_CB_DATANACK](_i2c_status.callbackPayload[I2C_CB_DATANACK])) {
			case i2c_restart_read:
				return _i2c_handle_I2C_STATE_SEND_RESTART_READ();
			case i2c_restart_write:
				return _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
			case i2c_continue:
			case i2c_stop:
			default:
				return _i2c_handle_I2C_STATE_SEND_STOP();
		}
	}
	else {
		_i2c_status.checkAddressNACK = 0;
		TWI0.MDATA = *_i2c_status.data_ptr++;
		if (--_i2c_status.data_length) {
			return I2C_STATE_TX;
		}
		else {
			return I2C_STATE_TX_EMPTY;
		}
	}
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_RX()
{
	_i2c_status.checkAddressNACK = 0;

	if (_i2c_status.data_length == 1) {
		TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	}
	else {
		TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
	}

	if (--_i2c_status.data_length) {
		*_i2c_status.data_ptr = TWI0.MDATA;
		_i2c_status.data_ptr++;
		TWI0.MCTRLB |= TWI_MCMD_RECVTRANS_gc;
		return I2C_STATE_RX;
	}
	else {
		*_i2c_status.data_ptr = TWI0.MDATA;
		_i2c_status.data_ptr++;
		_i2c_status.bufferFree = 1;
		switch (_i2c_status.callbackTable[I2C_CB_DATACOMPLETE](_i2c_status.callbackPayload[I2C_CB_DATACOMPLETE])) {
			case i2c_restart_write:
			case i2c_restart_read:
				return _i2c_handle_I2C_STATE_DO_NACK_RESTART();
			case i2c_continue:
			case i2c_stop:
			default:
				return _i2c_handle_I2C_STATE_DO_NACK_STOP();
		}
	}
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_TX_EMPTY()
{
	if ((TWI0.MSTATUS & TWI_RXACK_bm)) {
		switch (_i2c_status.callbackTable[I2C_CB_DATANACK](_i2c_status.callbackPayload[I2C_CB_DATANACK])) {
			case i2c_restart_read:
				return _i2c_handle_I2C_STATE_SEND_RESTART_READ();
			case i2c_restart_write:
				return _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
			case i2c_continue:
			case i2c_stop:
			default:
				return _i2c_handle_I2C_STATE_SEND_STOP();
		}
	}
	else {
		_i2c_status.bufferFree = 1;
		switch (_i2c_status.callbackTable[I2C_CB_DATACOMPLETE](_i2c_status.callbackPayload[I2C_CB_DATACOMPLETE])) {
			case i2c_restart_read:
				return _i2c_handle_I2C_STATE_SEND_RESTART_READ();
			case i2c_restart_write:
				return _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
			case i2c_continue:
				return _i2c_handle_I2C_STATE_TX();
			case i2c_stop:
			default:
				return _i2c_handle_I2C_STATE_SEND_STOP();
		}
	}
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART_READ()
{
	return _i2c_handle_I2C_STATE_SEND_ADR_READ();
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART_WRITE()
{
	return _i2c_handle_I2C_STATE_SEND_ADR_WRITE();
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_RESTART()
{
	return _i2c_handle_I2C_STATE_SEND_ADR_READ();
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_SEND_STOP()
{
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
	return _i2c_handle_I2C_STATE_IDLE();
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_RX_DO_ACK()
{
	TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
	return I2C_STATE_RX;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_TX_DO_ACK()
{
	TWI0.MCTRLB &= ~(1 << TWI_ACKACT_bp);
	return I2C_STATE_TX;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_NACK_STOP()
{
	TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	TWI0.MCTRLB |= TWI_MCMD_STOP_gc;
	return _i2c_handle_I2C_STATE_IDLE();
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_NACK_RESTART()
{
	TWI0.MCTRLB |= TWI_ACKACT_NACK_gc;
	return I2C_STATE_SEND_RESTART;
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_RESET()
{
	TWI0.MCTRLB  |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	
	_i2c_status.busy  = 0;
	_i2c_status.error = I2C_OK;
	
	return I2C_STATE_RESET;
}

// TODO: probably need 2 addressNACK's one from read and one from write.
//       the do NACK before RESTART or STOP is a special case that a new state simplifies.
i2c_fsm_states_t _i2c_handle_I2C_STATE_DO_ADDRESS_NACK()
{
	_i2c_status.error = I2C_FAIL;
	_i2c_status.checkAddressNACK = 0;
	switch (_i2c_status.callbackTable[I2C_CB_ADDRESSNACK](_i2c_status.callbackPayload[I2C_CB_ADDRESSNACK])) {
		case i2c_restart_read:
			return _i2c_handle_I2C_STATE_SEND_RESTART_READ();
		case i2c_restart_write:
			return _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
		default:
			return _i2c_handle_I2C_STATE_SEND_STOP();
	}
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_BUS_COLLISION()
{
	TWI0.MSTATUS |= TWI_ARBLOST_bm;
	
	_i2c_status.error = I2C_FAIL;
	switch (_i2c_status.callbackTable[I2C_CB_WRITECOLLISION](_i2c_status.callbackPayload[I2C_CB_WRITECOLLISION])) {
		case i2c_restart_read:
			return _i2c_handle_I2C_STATE_SEND_RESTART_READ();
		case i2c_restart_write:
			return _i2c_handle_I2C_STATE_SEND_RESTART_WRITE();
		default:
			return _i2c_handle_I2C_STATE_RESET();
	}
}

i2c_fsm_states_t _i2c_handle_I2C_STATE_BUS_ERROR()
{
	TWI0.MCTRLB  |= TWI_FLUSH_bm;
	TWI0.MSTATUS |= TWI_BUSSTATE_IDLE_gc;
	
	_i2c_status.busy  = 0;
	_i2c_status.error = I2C_FAIL;
	
	return I2C_STATE_RESET;
}