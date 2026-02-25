/*
 * spi.cpp
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#include "spi.h"



spi::spi(SPI_HandleTypeDef *spi_handle)
{
	hspi = spi_handle;
}

spi::~spi()
{

}

void spi::spi_transmit(uint8_t *data, uint16_t data_length)
{
	HAL_SPI_Transmit(hspi, data, data_length, data_length *1);
}
void spi::spi_receive(uint8_t *data, uint16_t data_length)
{
	HAL_SPI_Receive(hspi, data, data_length, data_length *1);
}
uint8_t spi::spi_transmit_receive_byte(uint8_t send_data)
{
	uint8_t ret;

	HAL_SPI_TransmitReceive(hspi, &send_data, &ret, 1, 1);

	return ret;
}

uint8_t spi::spi_transmit_bytes(uint8_t * send_data, uint16_t length)
{
	HAL_SPI_Transmit(hspi, send_data,  length, length);
}

uint8_t spi::spi_transmit_receive_bytes(uint8_t * send_data, uint8_t  * receive_data, uint16_t length)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(hspi, send_data,receive_data,  length, length);

	return ret;
}
