/*
 * gpio.cpp
 *
 *  Created on: June 16, 2025
 *      Author: Amogh MP
 */

#include "main.h"
#include "gpio.h"



gpio red_led(RED_LED_GPIO_Port, RED_LED_Pin);
gpio green_led(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
gpio blue_led(BLUE_LED_GPIO_Port, BLUE_LED_Pin);


gpio lora_chip_reset(LORA_CHIP_RESET_GPIO_Port, LORA_CHIP_RESET_Pin);
gpio user_switch(USER_SWITCH_GPIO_Port, USER_SWITCH_Pin);

gpio power_good(PGOOD_GPIO_Port, PGOOD_Pin);
gpio charger_staus(CHARGE_STATUS_GPIO_Port, CHARGE_STATUS_Pin);

gpio user_push_button(USER_SWITCH_GPIO_Port, USER_SWITCH_Pin);


gpio::gpio( void)
{

}

gpio::gpio(GPIO_TypeDef *gpio_port, uint16_t pin_number)
{
	gpiox = gpio_port;
	port_pin_number = pin_number;
}

gpio::~gpio() {
	// TODO Auto-generated destructor stub
}

void gpio::on(void)
{
	HAL_GPIO_WritePin(gpiox, port_pin_number, GPIO_PIN_SET);
}

void gpio::off(void)
{
	HAL_GPIO_WritePin(gpiox, port_pin_number, GPIO_PIN_RESET);
}

void gpio::toggle(void)
{
	HAL_GPIO_TogglePin(gpiox, port_pin_number);
}

uint8_t gpio:: read(void)
{
	return(HAL_GPIO_ReadPin(gpiox, port_pin_number));
}

void gpio::on(GPIO_TypeDef * gpio_port, uint16_t pin_number)
{
	HAL_GPIO_WritePin(gpio_port, pin_number, GPIO_PIN_SET);
}

void gpio::off(GPIO_TypeDef * gpio_port, uint16_t pin_number)
{
	HAL_GPIO_WritePin(gpio_port, pin_number, GPIO_PIN_RESET);
}

void gpio::toggle(GPIO_TypeDef * gpio_port, uint16_t pin_number)
{
	HAL_GPIO_TogglePin(gpio_port, pin_number);
}

uint8_t gpio::read(GPIO_TypeDef * gpio_port, uint16_t pin_number)
{
	return(HAL_GPIO_ReadPin(gpio_port, pin_number));
}
