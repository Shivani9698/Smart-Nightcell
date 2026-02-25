/*
 * peripherals.h
 *
 *  Created on: Oct 1, 2025
 *      Author: Amogh MP
 */

#ifndef PERIPHERALS_T_H_
#define PERIPHERALS_T_H_



#include "includes.h"
#include "external_eeprom.h"


class peripherals_t
{
	public:

		uint8_t cellular_enable;
		uint8_t wifi_enable;
		uint8_t lorawan_enable;

		uint8_t gps_enable;
		uint8_t gnss_enable;
		uint8_t lbs_enable;

		uint8_t power_on_all_peripherals;

		uint8_t start_application;


		peripherals_t();
		virtual ~peripherals_t();

		void load_default_values(void);

		void store_to_eeprom(void);
		void read_from_eeprom(void);


		void print(void);
};


extern peripherals_t peripherals;


#endif /* PERIPHERALS_T_H_ */
