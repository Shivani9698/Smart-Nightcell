/*
 * downlink_t.h
 *
 *  Created on: Oct 6, 2025
 *      Author: Amogh MP
 */

#ifndef DOWNLINK_T_H_
#define DOWNLINK_T_H_


#include "main.h"
#include "includes.h"
#include "wifi.h"



class downlink_t
{
	public:

	char processing_buffer[MAX_WIFI_BUFFER_SIZE];


	downlink_t();
	virtual ~downlink_t();

	uint8_t parse_set_config_downlink_command(void);
	uint8_t parse_get_config_downlink_command(void);
	uint8_t parse_configure_apn_downlink_command(void);
	uint8_t parse_reboot_downlink_command(void);
	uint8_t parse_reset_to_factory_defaults_downlink_command(void);
	uint8_t parse_turn_off_edge_device_downlink_command(void);
	uint8_t parse_set_lorawan_config_downlink_command(void);
	uint8_t parse_configure_mqtt_endpoint_downlink_command(void);

	void process_received_downlink(void);
};


extern downlink_t downlink;


#endif /* DOWNLINK_T_H_ */
