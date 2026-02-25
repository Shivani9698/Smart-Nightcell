/*
 * uplink_management.h
 *
 *  Created on: Jul 27, 2025
 *      Author: Amogh MP
 */

#ifndef INC_UPLINK_MANAGEMENT_H_
#define INC_UPLINK_MANAGEMENT_H_



#include "main.h"
#include "includes.h"



class next_message_to_send_t
{
	public:
		uint8_t start_up;
		uint8_t health;
		uint8_t set_config_ack;
		uint8_t get_config_ack;
		uint8_t configure_apn_ack;
		uint8_t reboot_ack;
		uint8_t reset_to_factory_defaults_ack;
		uint8_t turn_off_edge_device_ack;
		uint8_t https_error_message;
		uint8_t set_lorawan_config_ack;
		uint8_t set_lorawan_config_nack;
		uint8_t set_mqtt_endpoint_ack;
		uint8_t set_mqtt_endpoint_nack;

		next_message_to_send_t();
		virtual ~next_message_to_send_t();

		uint8_t load_default_values();
		uint8_t update(void);
		void get(void);
};

extern next_message_to_send_t next_message;



uint16_t serialize_https_error_message(char * uplink_buffer, uint16_t size);
uint16_t serialize_startup_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_health_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_set_config_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_get_config_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_configure_apn_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_reboot_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_reset_to_factory_defaults_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_turn_off_edge_device_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_smart_night_cell_data_payload_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_set_lorawan_config_ack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_set_lorawan_config_nack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_configure_mqtt_endpoint_nack_packet(char * uplink_buffer, uint16_t size);
uint16_t serialize_configure_mqtt_endpoint_ack_packet(char * uplink_buffer, uint16_t size);


uint8_t clear_next_message_flags(void);
uint16_t check_if_any_uplink_to_be_published(char * uplink_buffer, uint16_t size);



#endif /* INC_UPLINK_MANAGEMENT_H_ */
