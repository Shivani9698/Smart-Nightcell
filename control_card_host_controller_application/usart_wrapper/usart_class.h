/*
 * usart.h
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#ifndef SRC_USART_H_
#define SRC_USART_H_


#include "includes.h"
#include "mutex_t.h"



#define MAX_SIZE_FOR_PRINT_BUFFER  			 2048


class usart
{
	public:

		uint8_t usart_rx_byte;
		UART_HandleTypeDef * huart;

		mutex_t& mutex;

		usart(UART_HandleTypeDef * uart_handle, mutex_t& mutex_obj);
		uint8_t set_baudrate(uint32_t baudrate);
		uint32_t get_baudrate(void);
		void printf(const char *fmt, ...);
		void send_bytes(uint8_t * data, uint16_t length);
		void send_bytes(char * data);
		void unregister_rx_callback(void);
		void register_rx_callback(void (*callback)(UART_HandleTypeDef*));
		void enable_interrupt(void);
		void init(void);
		void deinit(void);
		virtual ~usart();


};

extern usart debug;


#endif /* SRC_USART_H_ */
