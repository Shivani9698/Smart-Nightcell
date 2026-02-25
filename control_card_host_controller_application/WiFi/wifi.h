/*
 * wifi.h
 *
 *  Created on: Jul 22, 2025
 *      Author: Amogh MP
 */

#ifndef WIFI_H_
#define WIFI_H_


#include "main.h"
#include "includes.h"
#include "usart_class.h"
#include "gpio.h"


#define     WIFI_CONNECTION_TIMEOUT					30000
#define 	WIFI_MANAGER_TIMEOUT_IN_MINS			30

// TODO: Ensure that the WIFI_MANAGER_TIMEOUT is set to atleast 5 seconds longer than the one defined in the ESP firmware.
#define     WIFI_MANAGER_TIMEOUT					15000 // ((WIFI_MANAGER_TIMEOUT_IN_MINS * 60 * 1000) + 5000)

#define 	NTP_SERVER_TIMEOUT						30000
#define     COMMON_TIMEOUT							10000


#define 	MAX_WIFI_BUFFER_SIZE					4096
#define 	MAX_WIFI_RETRY_COUNTER					5 // 16


#define 	MAX_CREDENTIALS_TOKEN_LENGTH			64
#define 	MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS	5 // TODO: don't update this number, else eeprom memory will override


typedef enum
{
   wifi_invalid_response = 0,
   wifi_ready,
   wifi_wait,
   wifi_conneted,
   wifi_disconnected,
   turn_on_wifi_manager,
   wifi_get_time_ack,
   wifi_set_time_ack,
   wifi_info_ack,
   wifi_info_nack,
   wifi_get_fw_version_ack,
   wifi_get_fw_version_nack,
   get_time_response,
   wifi_manager_success,
   wifi_manager_fail,

   device_reset,
   stm32_fw_update_avaliable,

   get_signal_strength_ack,
   wifi_invalid_result,
   wifi_multi_enabled,

   mqtt_broker_connection_success,
   mqtt_broker_connection_fail
} wifi_command_respone_t;

extern wifi_command_respone_t wifi_command_respone;



typedef struct wificredential_s
{
    char ssid[MAX_CREDENTIALS_TOKEN_LENGTH];
    char password[MAX_CREDENTIALS_TOKEN_LENGTH];
}wificredential_t;

extern wificredential_t wificredentials[MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS]; // TODO: store this info in eeprom



class wifi_t : public usart, public gpio
{
	public:
		mutex_t& mutex;

		uint8_t wifi_retry_counter;
		uint8_t wifi_reboot_required;

		uint8_t ready_status;
		uint8_t time_status;
		uint8_t connection_status;
		uint8_t wifi_manager_status;
		uint8_t wifi_info_status;
		uint8_t thingsboard_broker_status;

		uint8_t rx_byte = 0xFF;

		uint16_t rx_counter = 0;


		uint8_t downlink_processing_buffer[MAX_WIFI_BUFFER_SIZE];// command processing
		uint8_t downlink_data_buffer[MAX_WIFI_BUFFER_SIZE];   // to store the receive data or command
		char uplink_message_buffer[MAX_WIFI_BUFFER_SIZE];

	    wifi_t(UART_HandleTypeDef * uart, mutex_t& m, GPIO_TypeDef *wifi_enable_port, uint16_t wifi_enable_pin_number);
	    virtual ~wifi_t();

		void initialise(void);
		void deinitialise(void);
		void restart_module();

		void serialize_ready_packet(void);
		uint8_t parse_wifi_ready_ack_command(void);

		void serialize_set_wifi_command(char* ssid, char* pwd);


		wifi_command_respone_t connect(void);
		wifi_command_respone_t manage_wifi_connection(void);
		void serialize_wifi_manager_command(void);
		uint8_t parse_wifi_connected_command(void);

		void serialize_get_time_command(void);
		void get_time(void);
		uint8_t parse_get_time_from_ntp_server_ack_command(void);


		void serialize_get_wifi_info_command(void);
		void get_wifi_info(void);
		uint8_t parse_get_wifi_info_ack_command(void);


		wifi_command_respone_t connect_to_mqtt_broker(void);
		void serialize_configure_thingsboard_server_and_connect_command(void);
		uint8_t parse_configure_thingsboard_server_connect_ack_command(void);



		wifi_command_respone_t send_command_and_wait_for_response(uint32_t time_out);


		uint8_t load_default_wifi_credentials();
		uint8_t update_wifi_credentials(void);
		void get_wifi_credentials(void);

		void process_wifi_data(void);

		static void my_callback(UART_HandleTypeDef * uart);
};


extern wifi_t wifi;


#endif /* WIFI_H_ */
