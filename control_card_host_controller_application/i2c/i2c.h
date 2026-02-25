/*
 * i2c.h
 *
 *  Created on: Mar 22, 2022
 *      Author: GND FIRMWARE
 */

#ifndef SRC_I2C_H_
#define SRC_I2C_H_


#include <stdarg.h>

#include "main.h"
#include "includes.h"


#define 	WRITE_COMPLETE_DELAY_IN_MS 				10

extern I2C_HandleTypeDef hi2c3;

class i2c
{
	public:
		I2C_HandleTypeDef * hi2c;
		i2c(I2C_HandleTypeDef * i2c_handle);
		virtual ~i2c();


		void memory_read(uint8_t *read_values, uint16_t size_of_data, uint16_t chip_address, uint16_t address);
		void memory_write(uint8_t *write_values, uint16_t size_of_data, uint16_t chip_address, uint16_t address);


		void write(uint16_t chip_address, uint8_t *data, uint16_t size);
		void read (uint16_t chip_address, uint8_t *data, uint16_t size);

};

#endif /* SRC_I2C_H_ */
