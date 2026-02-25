/*
 * i2c.cpp
 *
 *  Created on: Mar 22, 2022
 *      Author: GND FIRMWARE
 */

#include "i2c.h"




i2c::i2c(I2C_HandleTypeDef * i2c_handle)
{
	// TODO Auto-generated constructor stub
	hi2c = i2c_handle;
}

i2c::~i2c()
{
	// TODO Auto-generated destructor stub
}


void i2c :: memory_read(uint8_t *read_values, uint16_t size_of_data, uint16_t chip_address, uint16_t address)
{
    HAL_I2C_Mem_Read(hi2c, chip_address, address, 2, read_values, size_of_data, 1000);
}


void i2c :: memory_write(uint8_t *write_values, uint16_t size_of_data, uint16_t chip_address, uint16_t address)
{
    HAL_I2C_Mem_Write(hi2c, chip_address, address, 2, write_values, size_of_data, 1000);
    osDelay(WRITE_COMPLETE_DELAY_IN_MS);
}



void i2c::write(uint16_t chip_address, uint8_t *data, uint16_t size)
{
    HAL_I2C_Master_Transmit(hi2c, chip_address, data, size, 1000);
}

void i2c::read(uint16_t chip_address, uint8_t *data, uint16_t size)
{
    HAL_I2C_Master_Receive(hi2c, chip_address, data, size, 1000);
}






