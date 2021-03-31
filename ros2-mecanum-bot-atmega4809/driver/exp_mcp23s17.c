/*
 * exp_mcp23s17.c
 *
 * Created: 24/03/2021 23:16:59
 *  Author: randy
 */ 

#include <driver/exp_mcp23s17.h>
#include <driver/spi_driver.h>

void exp_mcp23s17_init()
{
	
}

bool exp_mcp23s17_get_level(exp_mcp23s17_channel_t channel)
{
	// set mcp23s17 slave select low
	
	//spi_write(MCP23S17[0x40], EXP_DEV_ADR[0x??], EXP_READ[1]);
	//spi_write(addr);
	//value = spi_write(0);
	//spi_exchange_byte(OPCODE_READ);	// (0x20 << 1 | 0x01) => 0x20 = all 3 addr pin grounded
	//spi_exchange_byte(GPIOB);			// 0x13
	//bool level = exchange_byte(0);	// 0 is dummy to read
	
	// set mcp23s17 slave select high
	return false;
}