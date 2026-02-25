/*
 * spi.h
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#ifndef SRC_SPI_H_
#define SRC_SPI_H_


#include "main.h"
#include "includes.h"



class spi
{
	public:
	SPI_HandleTypeDef *hspi;
	spi(SPI_HandleTypeDef *spi_handle);
	virtual ~spi();

	void spi_transmit(uint8_t *data, uint16_t data_length);
	void spi_receive(uint8_t *data, uint16_t data_length);
	uint8_t spi_transmit_receive_byte(uint8_t send_data);
	uint8_t spi_transmit_bytes(uint8_t * send_data, uint16_t length);
	uint8_t spi_transmit_receive_bytes(uint8_t * send_data,uint8_t * receive_data, uint16_t length);

};

#endif /* SRC_SPI_H_ */
