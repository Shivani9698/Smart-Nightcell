/*
 * adc.h
 *
 *  Created on: Oct 25, 2025
 *      Author: Amogh MP
 */

#ifndef ADC_H_
#define ADC_H_



#include "main.h"
#include "includes.h"


extern ADC_HandleTypeDef hadc2;


class adc
{
	public:
	ADC_HandleTypeDef * hadc;
	ADC_MultiModeTypeDef * multimode;
	ADC_ChannelConfTypeDef * sConfig;
	uint16_t pin_numbner;
	uint8_t resolution;

	adc(ADC_HandleTypeDef * adc_handle);
	virtual ~adc();

	uint32_t read_adc(void);
	uint8_t read_battery_percentage_average(uint8_t number_of_samples);
};

extern adc battery;



#endif /* ADC_H_ */
