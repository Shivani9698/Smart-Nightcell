/*
 * gpio.h
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#include "includes.h"

class gpio {
public:

	GPIO_TypeDef * gpiox;
	uint16_t port_pin_number;
	gpio( void);
	gpio(GPIO_TypeDef * gpio_port, uint16_t pin_number);
	virtual ~gpio();

	void on(void);
	void off(void);
	void toggle(void);
	uint8_t read(void);

	void on(GPIO_TypeDef * gpio_port, uint16_t pin_number);
	void off(GPIO_TypeDef * gpio_port, uint16_t pin_number);
	void toggle(GPIO_TypeDef * gpio_port, uint16_t pin_number);
	uint8_t read(GPIO_TypeDef * gpio_port, uint16_t pin_number);
};


extern gpio red_led;
extern gpio green_led;
extern gpio blue_led;

extern gpio lora_chip_reset;
extern gpio user_switch;

extern gpio power_good;
extern gpio charger_staus;

extern gpio user_push_button;


#endif /* SRC_GPIO_H_ */

