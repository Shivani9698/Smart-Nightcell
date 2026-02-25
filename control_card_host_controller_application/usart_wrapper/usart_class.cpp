/*
 * usart.cpp
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#include "usart_class.h"


extern UART_HandleTypeDef huart1;



char string[MAX_SIZE_FOR_PRINT_BUFFER];

usart debug(&huart1, debug_uart_mutex);


//usart::usart(UART_HandleTypeDef * uart_handle, mutex_t& mutex_obj)
//{
//	huart = uart_handle;
//	mutex = mutex_obj;
//}

usart::usart(UART_HandleTypeDef* uart_handle, mutex_t& mutex_obj) : huart(uart_handle), mutex(mutex_obj)
{

}


usart::~usart()
{

}


uint8_t usart::set_baudrate(uint32_t baudrate)
{
	HAL_UART_DeInit(huart);
	huart->Init.BaudRate = baudrate;
	HAL_UART_Init(huart);

	return 1;
}


uint32_t usart:: get_baudrate(void)
{
	return  (huart->Init.BaudRate);
}


void usart::printf(const char *fmt, ...)
{
	if (1 == this->mutex.lock(2000))
	{
		va_list argp;
		va_start(argp, fmt);
		memset(string,'\0',sizeof(string));
		if(0 < vsprintf(string,fmt,argp))
		{
			HAL_UART_Transmit(huart, (uint8_t*)string, strlen(string), strlen(string) * 24);
		}
		va_end(argp);

		this->mutex.unlock();
	}
}


void usart::send_bytes(uint8_t * data, uint16_t length)
{
	if (1 == this->mutex.lock(2000))
	{
		HAL_UART_Transmit(huart, (uint8_t*)data, length, length * 20);

		this->mutex.unlock();
	}
}

void usart::send_bytes(char * data)
{
	if (1 == this->mutex.lock(2000))
	{
		HAL_UART_Transmit(huart, (uint8_t*)data, strlen(data),strlen(data) * 20);

		this->mutex.unlock();
	}
}


void usart::register_rx_callback(void (*callback)(UART_HandleTypeDef* ))
{
	HAL_UART_RegisterCallback(huart, HAL_UART_RX_COMPLETE_CB_ID, callback);
}


void usart::unregister_rx_callback(void)
{
	HAL_UART_UnRegisterCallback(huart, HAL_UART_RX_COMPLETE_CB_ID);
}


void usart::enable_interrupt()
{
	HAL_UART_Receive_IT(huart, &usart_rx_byte, 1);
}


void usart::init(void)
{
	if (HAL_UART_Init(huart) != HAL_OK)
	{

	}
}


void usart::deinit(void)
{
	if (HAL_UART_DeInit(huart) != HAL_OK)
	{

	}
}
