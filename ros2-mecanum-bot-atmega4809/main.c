/*
 * ros2-mecanum-bot-atmega4809.c
 *
 * Created: 22/03/2021 20:44:51
 * Author : randy
 */ 

#include <util/compiler.h>
#include <util/atomic.h>
#include <mcu/mcu_clock.h>
#include <mcu/mcu_port.h>
#include <mcu/mcu.h>
#include <driver/i2c_driver.h>
#include <driver/spi_driver.h>
#include <driver/motor_driver.h>
#include <driver/lsm9ds1.h>
#include <driver/usb_serial.h>
#include <driver/debug_serial.h>
#include <protocol/usb_hdlc.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <string.h>

#define DEBUG_BAUD_RATE(BAUD_RATE) ((float)(F_CPU * 64 / (16 * (float)BAUD_RATE)) + 0.5)

#define USB_CMD_SET_MOTOR		0x01
#define USB_CMD_BRAKE			0x02
#define USB_CMD_STOP			0x03
#define USB_CMD_IMU_DATA		0x04
#define USB_CMD_IMU_CALIB		0x0F

lsm9ds1_raw_vector_t raw_imu_a;
lsm9ds1_raw_vector_t raw_imu_g;
lsm9ds1_raw_vector_t raw_imu_m;

uint16_t m1_duty = 0;

void set_motor(uint8_t* data, size_t length)
{
	if (length < 4) {
		return;
	}
	motor_channel_t channels = (motor_channel_t)data[0];
	motor_direction_t dir    = (motor_direction_t)data[1];
	uint16_t duty            = (((uint16_t)data[3] << 8) | (uint16_t)data[2]);
	m1_duty = duty;
	motor_set_direction(channels, dir);
	motor_set_duty(channels, duty);
}

void usb_frame_handler(uint8_t* data, size_t length)
{
	if (length >= 2) {
		uint8_t cmd = data[0];
		uint8_t len = data[1];
		switch (cmd) {
			case USB_CMD_SET_MOTOR:
				set_motor(&data[2], (size_t)len);
				break;
			case USB_CMD_BRAKE:
				motor_brake(MOTOR_FL | MOTOR_FR | MOTOR_RL | MOTOR_RR);
				break;
			case USB_CMD_STOP:
				motor_stop(MOTOR_FL | MOTOR_FR | MOTOR_RL | MOTOR_RR);
				break;
			case USB_CMD_IMU_CALIB:
				lsm9ds1_calibrate();
				break;
		}
	}
}

ISR(PORTE_PORT_vect)
{
	cli();
	
	if (PORTE.INTFLAGS & PIN2_bm) {
		PORTE.INTFLAGS &= PIN2_bm;
	}
	if (PORTE.INTFLAGS & PIN3_bm) {
		PORTE.INTFLAGS &= PIN3_bm;
	}
	
	sei();
}

int main(void)
{
	// Initialize all drivers
	mcu_init();
	i2c_init();
	i2c_set_timeout(100);
	spi_init();
	usb_init();
	usb_hdlc_set_frame_callback(usb_frame_handler);
	motor_driver_init();
	
	// Set the debug UART
	debug_serial_init();
	
	/*
	cli();
	PORTE_set_pin_dir(2, PORT_DIR_IN);
	PORTE_set_pin_pull_mode(2, PORT_PULLUPEN_bm);
	PORTE_set_pin_isc(2, PORT_ISC_FALLING_gc);
	
	PORTE_set_pin_dir(3, PORT_DIR_IN);
	PORTE_set_pin_pull_mode(3, PORT_PULLUPEN_bm);
	PORTE_set_pin_isc(3, PORT_ISC_FALLING_gc);
	sei();
	*/
	
	// Wait for the lsm9ds1 to become ready
	lsm9ds1_reset();
	while (!lsm9ds1_is_ready()) {
		_delay_ms(100);
	}
	lsm9ds1_init();
	//lsm9ds1_calibrate();
	
	//motor_set_direction(MOTOR_FL | MOTOR_FR | MOTOR_RL | MOTOR_RR, MOTOR_DIR_FWD);
	//motor_set_duty(MOTOR_FL | MOTOR_FR | MOTOR_RL | MOTOR_RR, 200);
	
	ENABLE_INTERRUPTS();

    while (1) 
    {
		_delay_ms(1);
		
		// Read the command frames from usb
		usb_hdlc_read();
		
		/*
		float res = 0.00875f;
		char msg[20];
		if (lsm9ds1_accel_available()) {
			lsm9ds1_read_raw_accel(&raw_imu_a);
			sprintf(msg, "A: [ %d %d %d ]", (int)((float)raw_imu_a.axis[0] * res), (int)((float)raw_imu_a.axis[1] * res), (int)((float)raw_imu_a.axis[2] * res));
			//debug_serial_write_line(msg);
		}
		if (lsm9ds1_gyro_available()) {
			lsm9ds1_read_raw_gyro(&raw_imu_g);
			sprintf(msg, "G: [ %d %d %d ]", (int)((float)raw_imu_g.axis[0] * res), (int)((float)raw_imu_g.axis[1] * res), (int)((float)raw_imu_g.axis[2] * res));
			debug_serial_write_line(msg);
		}
		if (lsm9ds1_mag_available()) {
			lsm9ds1_read_raw_mag(&raw_imu_m);
			sprintf(msg, "M: [ %d %d %d ]", (int)((float)raw_imu_m.axis[0] * res), (int)((float)raw_imu_m.axis[1] * res), (int)((float)raw_imu_m.axis[2] * res));
			//debug_serial_write_line(msg);
		}
		*/
    }
}

