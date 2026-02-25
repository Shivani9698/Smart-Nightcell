/*
 * external_eeprom.cpp
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#include "external_eeprom.h"
#include "i2c.h"
#include "mutex_t.h"




uint32_t write_index = 0;
uint32_t write_counter = 0;

uint32_t read_index = 0;
uint32_t read_page_counter = 0;

uint32_t message_counter = 0;
uint32_t message_counter_index = 0;


uint32_t write_page_counter = 0;




external_eeprom eeprom(&hi2c3, EEPROM_DEVICE_ADDRESS);


external_eeprom::external_eeprom(I2C_HandleTypeDef * i2c_handler, uint16_t device_address) : i2c(i2c_handler)
{
	this->device_address = device_address;
}


external_eeprom::~external_eeprom()
{

}


void external_eeprom::erase_eeprom(void)
{
	uint8_t write_data[256] = {0};

	if (1 == eeprom_mutex.lock(1000))
	{
		for (int i = 0; i < EEPROM_MEMORY_SIZE; i = i + 256)
		{
			this->memory_write(write_data, 256, this->device_address, i);
		}

		eeprom_mutex.unlock();
	}
}


void external_eeprom::write_byte(uint8_t value, uint16_t address)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		this->memory_write(&value, 1, this->device_address, address);

		eeprom_mutex.unlock();
	}
}


uint8_t external_eeprom::read_byte(uint16_t address)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint8_t byte_read_value = 0;
		this->memory_read(&byte_read_value, 1, this->device_address, address);

		eeprom_mutex.unlock();

		return byte_read_value;
	}
}


void external_eeprom::write_int(int value, uint16_t address)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint8_t write_value[4] = {(value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)};
		this->memory_write(write_value, sizeof(int), this->device_address, address);

		eeprom_mutex.unlock();
	}
}


uint32_t external_eeprom::read_int(uint16_t address)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint32_t data = 0;
		uint8_t read_values[4] = {0};
		this->memory_read(read_values, sizeof(int), this->device_address, address);
		data = (read_values[0]) | (read_values[1] << 8) | (read_values[2] << 16) | (read_values[3] << 24);

		eeprom_mutex.unlock();

		return data;
	}

}


uint8_t external_eeprom::write_structure(uint8_t * write_data, uint16_t address, uint16_t length)
{
	uint8_t res = 0;

	if (1 == eeprom_mutex.lock(1000))
	{
		if(length > 256)
		{
			for(int i = 0 ; i < length ; i = i + 256)
			{
				this->memory_write(&write_data[i], 256, this->device_address, address + i);
			}

		}
		else if(length <= 256)
		{
			this->memory_write(write_data, length, this->device_address, address);
		}

		eeprom_mutex.unlock();
		res = 1;
	}

	return res;
}


uint8_t external_eeprom::read_structure(uint8_t * read_values, uint16_t address, uint16_t length)
{
	uint8_t res = 0;

	if (1 == eeprom_mutex.lock(1000))
	{
		this->memory_read(read_values, length, this->device_address, address);

		eeprom_mutex.unlock();
		res = 1;
	}

	return res;
}





void external_eeprom::storage_counter_write(uint32_t value, uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_WRITE_COUNTER + (index * 4);

		uint8_t write_value[4] = {(value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)};

		this->memory_write(write_value, sizeof(int), this->device_address, address);

		eeprom_mutex.unlock();
	}

}


uint32_t external_eeprom::storage_counter_read(uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_WRITE_COUNTER + (index * 4);

		uint32_t data = 0;
		uint8_t read_values[4] = {0};

		this->memory_read(read_values, sizeof(int), this->device_address, address);
		data = (read_values[0]) | (read_values[1] << 8) | (read_values[2] << 16) | (read_values[3] << 24);

		eeprom_mutex.unlock();

		return data;
	}

}

void external_eeprom::reader_counter_write(uint32_t value, uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_READ_COUNTER + (index * 4);

		uint8_t write_value[4] = {(value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)};

		this->memory_write(write_value, sizeof(int), this->device_address, address);

		eeprom_mutex.unlock();
	}
}

uint32_t external_eeprom::reader_counter_read( uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_READ_COUNTER + (index * 4);

		uint32_t data = 0;
		uint8_t read_values[4] = {0};

		this->memory_read(read_values, sizeof(int), this->device_address, address);
		data = (read_values[0]) | (read_values[1] << 8) | (read_values[2] << 16) | (read_values[3] << 24);

		eeprom_mutex.unlock();

		return data;
	}

}


void external_eeprom::message_counter_write(uint32_t value, uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_MESSAGE_COUNTER + (index * 4);

		uint8_t write_value[4] = {(value & 0xFF), ((value >> 8) & 0xFF), ((value >> 16) & 0xFF), ((value >> 24) & 0xFF)};

		this->memory_write(write_value, sizeof(int), this->device_address, address);

		eeprom_mutex.unlock();
	}
}


uint32_t external_eeprom::message_counter_read(uint16_t index)
{
	if (1 == eeprom_mutex.lock(1000))
	{
		uint16_t address = START_ADDRESS_OF_MESSAGE_COUNTER + (index * 4);

		uint32_t data = 0;
		uint8_t read_values[4] = {0};

		this->memory_read(read_values, sizeof(int), this->device_address, address);
		data = (read_values[0]) | (read_values[1] << 8) | (read_values[2] << 16) | (read_values[3] << 24);

		eeprom_mutex.unlock();

		return data;
	}

}



uint8_t external_eeprom::store_device_info(uint8_t* write_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_DEVICE_INFO;
	uint8_t res = write_structure(write_data, address, length);

	return res;
}

uint8_t external_eeprom::get_device_info(uint8_t* read_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_DEVICE_INFO;
	uint8_t res = read_structure(read_data, address, length);

	return res;
}


uint8_t external_eeprom::store_wifi_credentials_info(uint8_t* write_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_WIFI_CREDENTIALS;
	uint8_t res = write_structure(write_data, address, length);

	return res;
}


uint8_t external_eeprom::get_wifi_credentials_info(uint8_t* read_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_WIFI_CREDENTIALS;
	uint8_t res = read_structure(read_data, address, length);

	return res;
}


uint8_t external_eeprom::store_peripheral_configuration_info(uint8_t* write_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_PERIPHERAL_CONFIG;
	uint8_t res = write_structure(write_data, address, length);

	return res;
}


uint8_t external_eeprom::get_peripheral_configuration_info(uint8_t* read_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_PERIPHERAL_CONFIG;
	uint8_t res = read_structure(read_data, address, length);

	return res;
}


uint8_t external_eeprom::store_next_message_to_send_info(uint8_t* write_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_NEXT_MESSAGE_TO_SEND;
	uint8_t res = write_structure(write_data, address, length);

	return res;
}

uint8_t external_eeprom::get_next_message_to_send_info(uint8_t* read_data, uint16_t length)
{
	uint16_t address = START_ADDRESS_OF_NEXT_MESSAGE_TO_SEND;
	uint8_t res = read_structure(read_data, address, length);

	return res;
}

