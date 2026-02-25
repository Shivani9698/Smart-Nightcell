/*
 * wifi.cpp
 *
 *  Created on: Jul 22, 2025
 *      Author: Amogh MP
 */




#include "wifi.h"
#include "mutex_t.h"
#include "ArduinoJson.h"
#include "semaphore_t.h"
#include "device_info.h"
#include "external_eeprom.h"
#include "realtimeclock.h"
#include "uplink_management.h"
#include "peripherals_t.h"



extern UART_HandleTypeDef huart5;



wifi_command_respone_t wifi_command_respone = wifi_invalid_response;

wificredential_t wificredentials[MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS];

wifi_t wifi(&huart5, wifi_print_mutex, WiFi_CHIP_ENABLE_GPIO_Port, WiFi_CHIP_ENABLE_Pin);



wifi_t::wifi_t(UART_HandleTypeDef * uart, mutex_t& m, GPIO_TypeDef *wifi_enable_port, uint16_t wifi_enable_pin_number) :
		usart(uart, m), mutex(m), gpio(wifi_enable_port, wifi_enable_pin_number)
{

}


wifi_t::~wifi_t()
{
	// TODO Auto-generated destructor stub
}


void wifi_t::initialise(void)
{
	deinit();
	off();

	on();
	init();
	register_rx_callback(wifi_t::my_callback);
	enable_interrupt();
}


void wifi_t::deinitialise(void)
{
	unregister_rx_callback();
	deinit();
	off();
}


void wifi_t::restart_module(void)
{
	ready_status = 0;
	connection_status = 0;
	wifi_manager_status = 0;
	time_status = 0;
	wifi_info_status = 0;
	thingsboard_broker_status = 0;
	wifi_reboot_required = 0;

	deinitialise();
	osDelay(3000); // TODO: check if 3 sec is required or less.?
	initialise();
}


void wifi_t::serialize_ready_packet(void)
{
	DynamicJsonDocument doc(64);

	doc["command"] = "READY";

	serializeJson(doc, uplink_message_buffer);
}



uint8_t wifi_t::parse_wifi_ready_ack_command(void)
{
	uint8_t res = 0;

	DynamicJsonDocument doc(128);

	DeserializationError error = deserializeJson(doc, downlink_processing_buffer);

	if (error)
	{
		debug.printf("Invalid JSON\r\n");

		res = 0;
	}

	const char* command = doc["command"];
	const char* mac = doc["mac_id"];

	if (mac && command)
	{
		sprintf(write_device_info.device_id, "%s", mac);
		sprintf(write_device_info.device_name, "%s", write_device_info.device_id);
		sprintf(write_device_info.ap_ssid, "%s", write_device_info.device_id);

		write_device_info.update();
		osDelay(250);
		read_device_info.get();

		res = 1;
	}

	return res;
}


void wifi_t::serialize_set_wifi_command(char* ssid, char* pwd)
{
	DynamicJsonDocument doc(512);

	doc["command"] = "SET_WIFI_WITHOUT_STATIC_IP";
//	doc["ssid"] = read_device_info.ssid; // TODO: replace with already connected ssid
//	doc["pwd"] = read_device_info.password; // TODO: replace with already connected password

	doc["ssid"] = ssid;
	doc["pwd"] = pwd;

	doc["ap_ssid"] = read_device_info.device_id;
	doc["ap_password"] = read_device_info.ap_password; // TODO: hardcode the ap_password in esp32
	doc["ap_static_ip"] = read_device_info.ap_static_ip; // TODO: fix the access point ip-address in esp32

	serializeJson(doc, uplink_message_buffer);
}



wifi_command_respone_t wifi_t::connect(void)
{
	uint8_t i = 0;
	wifi_command_respone_t status = wifi_invalid_response;

	read_device_info.get();
	osDelay(250);
	wifi.get_wifi_credentials();


	for (i = 0 ; i < MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS ; i++)
	{
		serialize_set_wifi_command(wificredentials[i].ssid, wificredentials[i].password);
		debug.printf("stm tx data: %s\r\n", uplink_message_buffer);

		status = send_command_and_wait_for_response(WIFI_CONNECTION_TIMEOUT);

		if (wifi_conneted == status)
		{
			break;
		}
	}

	if (i >= MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS)
	{
		status = turn_on_wifi_manager;
	}

	return status;
}


wifi_command_respone_t wifi_t::manage_wifi_connection(void)
{
	wifi_command_respone_t res = wifi.connect();

	return res;
}


void wifi_t::serialize_wifi_manager_command(void)
{
	DynamicJsonDocument doc(64);

	doc["command"] = "TURN_ON_WIFI_MANAGER";

	serializeJson(doc, uplink_message_buffer);
}


uint8_t wifi_t::parse_wifi_connected_command(void)
{
	uint8_t res = 0;

	DynamicJsonDocument doc(512);

	DeserializationError error = deserializeJson(doc, downlink_processing_buffer);

	if (error)
	{
		res = 0;
		debug.printf("Invalid JSON\r\n");
	}

	const char* command = doc["command"]; // "WIFI_CONNECTED"
	const char* mac = doc["mac_id"];
	const char* ssid_name = doc["ssid"]; // "abcd"
	const char* pwd = doc["pwd"]; // "123"
	const char* ap_name = doc["ap_name"]; // "mac_id"
	const char* ap_pwd = doc["ap_password"]; // "123"
	const char* ap_static_ip_addr = doc["ap_static_ip"]; // "192.162.4.1"

	if ((strstr(mac, read_device_info.device_id) != NULL) && command && ssid_name && pwd && ap_name && ap_pwd && ap_static_ip_addr)
	{
		sprintf(write_device_info.device_id, "%s", mac);
		sprintf(write_device_info.device_name, "%s", write_device_info.device_id);
		sprintf(write_device_info.ap_ssid, "%s", write_device_info.device_id);

		sprintf(write_device_info.ssid, "%s", ssid_name);
		sprintf(write_device_info.password, "%s", pwd);
		sprintf(write_device_info.ap_ssid, "%s", ap_name);
		sprintf(write_device_info.ap_password, "%s", ap_pwd);
		sprintf(write_device_info.ap_static_ip, "%s", ap_static_ip_addr);

		write_device_info.update();
		osDelay(500);
		read_device_info.get();

		for (uint8_t i = MAX_NUMBER_OF_STORED_WIFI_CREDENTIALS - 1 ; i >= 1 ; i--)
		{
			sprintf(wificredentials[i].ssid, "%s", wificredentials[i - 1].ssid);
			sprintf(wificredentials[i].password, "%s", wificredentials[i - 1].password);
		}

		sprintf(wificredentials[0].ssid, "%s", read_device_info.ssid);
		sprintf(wificredentials[0].password, "%s", read_device_info.password);


		update_wifi_credentials();
		osDelay(500);
		get_wifi_credentials();

		res = 1;

	}
	else
	{
		debug.printf("Error: Missing or NULL field in JSON\r\n");
	}

	return res;
}


void wifi_t::serialize_get_time_command(void)
{
	DynamicJsonDocument doc(64);

	doc["command"] = "GET_TIME_FROM_NTP_SERVER";

	serializeJson(doc, uplink_message_buffer);
}


void wifi_t::get_time(void)
{
	serialize_get_time_command();
	debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);
	send_command_and_wait_for_response(NTP_SERVER_TIMEOUT);
}


uint8_t wifi_t::parse_get_time_from_ntp_server_ack_command(void)
{
	uint8_t res = 0;

	DynamicJsonDocument doc(256);

	DeserializationError error = deserializeJson(doc, downlink_processing_buffer);

	if (error)
	{
		res = 0;
		debug.printf("Invalid JSON\r\n");
	}

	const char* command = doc["command"];
	uint32_t epoch = doc["TIME"];
	const char* mac = doc["mac_id"];

	if ((strstr(mac, read_device_info.device_id) != NULL) && command && (epoch > JANUARY_1_2025_EPOCH_TIME) && (epoch < JANUARY_1_2050_EPOCH_TIME))
	{
		rtc.set_time_in_utc(epoch);

		debug.printf("Received epoch time: %lu\r\n", epoch);

		res = 1;
	}
	else
	{
		debug.printf("Error: Missing or NULL field in JSON\r\n");

		res = 0;
	}

	return res;
}


void wifi_t::serialize_get_wifi_info_command(void)
{
	DynamicJsonDocument doc(32);

	doc["command"] = "GET_WIFI_INFO";

	serializeJson(doc, uplink_message_buffer);
}


void wifi_t::get_wifi_info(void)
{
	serialize_get_wifi_info_command();
	debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);

	send_command_and_wait_for_response(WIFI_CONNECTION_TIMEOUT);
}


uint8_t wifi_t::parse_get_wifi_info_ack_command(void)
{
	uint8_t res = 0;

	DynamicJsonDocument doc(512);

	DeserializationError error = deserializeJson(doc, downlink_processing_buffer);

	if (error)
	{
		debug.printf("Invalid JSON\r\n");
		res = 0;
	}

	const char* command = doc["command"]; // "GET_WIFI_INFO_ACK"
	uint32_t epoch = doc["time"]; // "12345678910"
	const char* mac = doc["mac_id"]; // "00:1A:2B:3C:4D:5E"
	const char* ssid_name = doc["ssid"]; // "abcd"
	const char* pwd = doc["pwd"]; // "123"
	const char* ap_name = doc["ap_name"]; // "mac_id"
	const char* ap_pwd = doc["ap_password"]; // "123"
	const char* ap_static_ip_addr = doc["ap_static_ip"]; // "192.162.4.1"

	if ((strstr(mac, read_device_info.device_id) != NULL) && command && ssid_name && pwd && ap_name && ap_pwd && ap_static_ip_addr &&
			(epoch > JANUARY_1_2025_EPOCH_TIME) && (epoch < JANUARY_1_2050_EPOCH_TIME))
	{
		sprintf(write_device_info.device_id, "%s", mac);
		sprintf(write_device_info.device_name, "%s", write_device_info.device_id);
		sprintf(write_device_info.ap_ssid, "%s", write_device_info.device_id);

		sprintf(write_device_info.ssid, "%s", ssid_name);
		sprintf(write_device_info.password, "%s", pwd);
		sprintf(write_device_info.ap_ssid, "%s", ap_name);
		sprintf(write_device_info.ap_password, "%s", ap_pwd);
		sprintf(write_device_info.ap_static_ip, "%s", ap_static_ip_addr);

		rtc.set_time_in_utc(epoch);

		write_device_info.update();
		osDelay(500);
		read_device_info.get();

		res = 1;
	}
	else
	{
		debug.printf("Error: Missing or NULL field in JSON\r\n");
	}

	return res;
}


//void wifi_t::serialize_configure_thingsboard_server_and_connect_command(void)
//{
//	DynamicJsonDocument doc(2048);
//
//	doc["command"] = "CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT";
//	doc["broker_end_point"] = read_device_info.mqtt_broker_endpoint;
//	doc["uplink_topic"] = read_device_info.device_uplink_telemetry_topic;
//	doc["downlink_topic"] = read_device_info.device_downlink_topic;
//	doc["device_name"] = read_device_info.device_name;
//	doc["device_profile"] = read_device_info.device_profile;
//	doc["acc_token"] = read_device_info.access_token;
//	doc["dev_id"] = read_device_info.device_id;
//	doc["port_no"] = read_device_info.port_number;
//
//	serializeJson(doc, uplink_message_buffer);
//}
//
//
//wifi_command_respone_t wifi_t::connect_to_mqtt_broker(void)
//{
//	read_device_info.get();
//
//	serialize_configure_thingsboard_server_and_connect_command();
//	debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);
//
//	wifi_command_respone_t status = send_command_and_wait_for_response(WIFI_CONNECTION_TIMEOUT);
//
//	return status;
//}
//
//
//uint8_t wifi_t::parse_configure_thingsboard_server_connect_ack_command(void)
//{
//	uint8_t res = 0;
//
//	DynamicJsonDocument doc(1024);
//
//	DeserializationError error = deserializeJson(doc, downlink_processing_buffer);
//
//	if (error)
//	{
//		res = 0;
//		debug.printf("Invalid JSON\r\n");
//	}
//
//	const char* command = doc["command"]; // "CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_ACK"
//	uint32_t epoch = doc["time"];
//	const char* mac = doc["mac_id"]; // nullptr
//	const char* broker_end_point = doc["broker_end_point"]; // "broker.emqx.io"
//	const char* uplink_topic = doc["uplink_topic"]; // "test_uplink"
//	const char* downlink_topic = doc["downlink_topic"]; // "test_downlink"
//	const char* device_name = doc["device_name"]; // "mra_test_device"
//	const char* device_profile = doc["device_profile"]; // "mra"
//	const char* acc_token = doc["acc_token"]; // "mra_test_device"
//	const char* dev_id = doc["dev_id"]; // nullptr
//	int port_no = doc["port_no"]; // 1883
//
//
//	debug.printf("mac: %s, broker_end_point: %s, uplink_topic: %s, downlink_topic: %s, device_name: %s, device_profile: %s, acc_token: %s, "
//			"dev_id: %s, epoch: %d, port_no: %d\r\n",
//			mac, broker_end_point, uplink_topic, downlink_topic, device_name, device_profile, acc_token, dev_id, epoch, port_no);
//
//
//	if ((strstr(mac, read_device_info.device_id) != NULL) && command && broker_end_point && uplink_topic && downlink_topic &&
//			device_name && device_profile && acc_token && (0 != port_no) &&
//			(epoch > JANUARY_1_2025_EPOCH_TIME) && (epoch < JANUARY_1_2050_EPOCH_TIME))
//	{
//		rtc.set_time_in_utc(epoch);
//
//		res = 1;
//	}
//	else
//	{
//		debug.printf("Error: Missing or NULL field in JSON\r\n");
//	}
//
//	return res;
//}


wifi_command_respone_t wifi_t::send_command_and_wait_for_response(uint32_t time_out)
{
	wifi_command_respone = wifi_invalid_result;
	uint32_t prev_time_in_millisec = HAL_GetTick();

	this->printf("%s\r\n", uplink_message_buffer);

	uint32_t current_time_in_millisec = HAL_GetTick();

	while((wifi_command_respone == wifi_invalid_result) && ((current_time_in_millisec - prev_time_in_millisec) < time_out))
	{
		current_time_in_millisec = HAL_GetTick();

		osDelay(100);
	}

	return wifi_command_respone;
}


uint8_t wifi_t::load_default_wifi_credentials(void)
{
	sprintf(wificredentials[0].ssid, "%s", DEFAULT_WIFI_SSID);
	sprintf(wificredentials[1].ssid, "%s", DEFAULT_WIFI_SSID);
	sprintf(wificredentials[2].ssid, "%s", DEFAULT_WIFI_SSID);
	sprintf(wificredentials[3].ssid, "%s", DEFAULT_WIFI_SSID);
	sprintf(wificredentials[4].ssid, "%s", DEFAULT_WIFI_SSID);

	sprintf(wificredentials[0].password, "%s", DEFAULT_WIFI_PASSWORD);
	sprintf(wificredentials[1].password, "%s", DEFAULT_WIFI_PASSWORD);
	sprintf(wificredentials[2].password, "%s", DEFAULT_WIFI_PASSWORD);
	sprintf(wificredentials[3].password, "%s", DEFAULT_WIFI_PASSWORD);
	sprintf(wificredentials[4].password, "%s", DEFAULT_WIFI_PASSWORD);

	update_wifi_credentials();
}


uint8_t wifi_t::update_wifi_credentials(void)
{
	uint8_t res = 0;

	res = eeprom.store_wifi_credentials_info((uint8_t *)&wificredentials, sizeof(wificredentials));

	return res;
}


void wifi_t::get_wifi_credentials(void)
{
	uint8_t res = eeprom.get_wifi_credentials_info((uint8_t *)&wificredentials, sizeof(wificredentials));
}


void wifi_t::process_wifi_data(void)
{
	if (strstr((char *)downlink_processing_buffer, "ESP32_RESET") != NULL)
	{
		debug.printf("received ESP32_RESET\r\n");

		ready_status = 0;
		connection_status = 0;
		wifi_manager_status = 0;
		time_status = 0;
		wifi_info_status = 0;
		thingsboard_broker_status = 0;
	}

	else if (strstr((char *)downlink_processing_buffer, "READY_ACK") != NULL)
	{
		if (1 == parse_wifi_ready_ack_command())
		{
			debug.printf("received READY_ACK\r\n");

			ready_status = 1;
			connection_status = 0;
			wifi_manager_status = 0;
			time_status = 0;
			wifi_info_status = 0;
			thingsboard_broker_status = 0;

			wifi_retry_counter = 0; // TODO: don't forget to reset the counter
		}
	}

	else if (strstr((char *)downlink_processing_buffer, "WIFI_CONNECTED") != NULL)
	{
		if (1 == parse_wifi_connected_command())
		{
			debug.printf("received WIFI_CONNECTED\r\n");

			ready_status = 1;
			connection_status = 1;
			wifi_manager_status = 0;
			time_status = 0;
			wifi_info_status = 0;
			thingsboard_broker_status = 0;

			wifi_retry_counter = 0; // TODO: don't forget to reset the counter

			wifi_command_respone = wifi_conneted;
		}
	}

	else if((strstr((char *)downlink_processing_buffer, "WIFI_DISCONNECTED") != NULL) ||
			(strstr((char *)downlink_processing_buffer, "WIFI_CONNECTION_FAIL") != NULL))
	{
		debug.printf("received WIFI_DISCONNECTED\r\n");

		ready_status = 1;
		connection_status = 0;
		time_status = 0;
		wifi_info_status = 0;
		thingsboard_broker_status = 0;

		if (1 == wifi_manager_status)
		{
			wifi_reboot_required = 1;
		}

		wifi_command_respone = wifi_disconnected;
	}

	else if (strstr((char *)downlink_processing_buffer, "GET_TIME_FROM_NTP_SERVER_ACK") != NULL)
	{
		if (1 == parse_get_time_from_ntp_server_ack_command())
		{
			debug.printf("received GET_TIME_FROM_NTP_SERVER_ACK\r\n");

			ready_status = 1;
			connection_status = 1;
			wifi_manager_status = 0;
			time_status = 1;
			wifi_info_status = 0;
			thingsboard_broker_status = 0;

			wifi_retry_counter = 0; // TODO: don't forget to reset the counter

			wifi_command_respone = wifi_get_time_ack;
		}
	}

	else if (strstr((char *)downlink_processing_buffer, "GET_WIFI_INFO_ACK") != NULL)
	{
		if (1 == parse_get_wifi_info_ack_command())
		{
			debug.printf("received GET_WIFI_INFO_ACK\r\n");

			ready_status = 1;
			connection_status = 1;
			wifi_manager_status = 0;
			time_status = 1;
			wifi_info_status = 1;
			thingsboard_broker_status = 0;

			wifi_retry_counter = 0; // TODO: don't forget to reset the counter

			wifi_command_respone = wifi_info_ack;
		}

		else
		{
			wifi_command_respone = wifi_info_nack;
		}
	}

//	else if (strstr((char *)downlink_processing_buffer, "CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_ACK") != NULL)
//	{
//		debug.printf("parsing CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_ACK\r\n");
//
//		if (1 == parse_configure_thingsboard_server_connect_ack_command())
//		{
//			debug.printf("received CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_ACK\r\n");
//
//			ready_status = 1;
//			connection_status = 1;
//			wifi_manager_status = 0;
//			time_status = 1;
//			wifi_info_status = 1;
//			wifi.thingsboard_broker_status = 1;
//
//			wifi_retry_counter = 0; // TODO: don't forget to reset the counter
//
//			wifi_command_respone = mqtt_broker_connection_success;
//		}
//	}
//
//	else if (strstr((char *)downlink_processing_buffer, "CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_NACK") != NULL)
//	{
//		debug.printf("received CONFIGURE_THINGSBOARD_SERVER_AND_CONNECT_NACK\r\n");
//
//		ready_status = 0;
//		connection_status = 0;
//		wifi_manager_status = 0;
//		time_status = 0;
//		wifi_info_status = 0;
//		thingsboard_broker_status = 0;
//
//		wifi_command_respone = mqtt_broker_connection_fail;
//	}

}



/* USER CODE BEGIN Header_wifi_thread_task */
/**
* @brief Function implementing the wifi_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_wifi_thread_task */
void wifi_thread_task(void const * argument)
{
	/* USER CODE BEGIN wifi_thread_task */

	osDelay(5000);

  /* Infinite loop */
  for(;;)
  {
	  HAL_IWDG_Refresh(&hiwdg1);

	  if ((wifi_enable == read_device_info.communication_mode_enabled) && (1 == peripherals.power_on_all_peripherals))
	  {
		  if (1 == wifi.wifi_reboot_required)
		  {
			  debug.printf("Max wifi retry count exceeded......Rebooting now\r\n");

			  wifi.wifi_retry_counter = 0;

			  wifi.restart_module();
		  }

		  if ((0 == wifi.ready_status) && (0 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (0 == wifi.time_status) && (0 == wifi.wifi_info_status)
				  && (0 == wifi.thingsboard_broker_status))
		  {
			  wifi.wifi_retry_counter++;

			  wifi.serialize_ready_packet();

			  debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);
			  wifi.printf("%s\r\n", wifi.uplink_message_buffer);

			  osDelay(3000);

			  if (wifi.wifi_retry_counter >= MAX_WIFI_RETRY_COUNTER)
			  {
				  wifi.wifi_reboot_required = 1;
			  }
		  }

		  if((1 == wifi.ready_status) && (0 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (0 == wifi.time_status) &&
				(0 == wifi.wifi_info_status) && (0 == wifi.thingsboard_broker_status))
		  {
			  wifi_command_respone_t wifi_status = wifi.manage_wifi_connection();

			  if (turn_on_wifi_manager == wifi_status)
			  {
				  wifi.wifi_manager_status = 1;
			  }
		  }

		  if((1 == wifi.ready_status) && (0 == wifi.connection_status) && (1 == wifi.wifi_manager_status) && (0 == wifi.time_status) &&
				(0 == wifi.wifi_info_status) && (0 == wifi.thingsboard_broker_status))
		  {
			  wifi.serialize_wifi_manager_command();
			  debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);

			  wifi.send_command_and_wait_for_response(WIFI_MANAGER_TIMEOUT); // TODO: check if the timeout should be same as webpage timeout.
		  }

		  if ((1 == wifi.ready_status) && (1 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (0 == wifi.time_status) &&
					(0 == wifi.wifi_info_status) && (0 == wifi.thingsboard_broker_status))
		  {
			  wifi.wifi_retry_counter++;

			  wifi.get_time();

			  if (wifi.wifi_retry_counter >= MAX_WIFI_RETRY_COUNTER)
			  {
				  wifi.wifi_reboot_required = 1;
			  }
		  }

		  if ((1 == wifi.ready_status) && (1 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (1 == wifi.time_status) &&
					(0 == wifi.wifi_info_status) && (0 == wifi.thingsboard_broker_status))
		  {
			  wifi.wifi_retry_counter++;

			  wifi.get_wifi_info();

			  if (wifi.wifi_retry_counter >= MAX_WIFI_RETRY_COUNTER)
			  {
				  wifi.wifi_reboot_required = 1;
			  }
		  }

//		  if ((1 == wifi.ready_status) && (1 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (1 == wifi.time_status) &&
//				  (1 == wifi.wifi_info_status) && (0 == wifi.thingsboard_broker_status))
//		  {
//			  wifi_command_respone_t res = wifi.connect_to_mqtt_broker();
//
//			  if (mqtt_broker_connection_fail == res)
//			  {
//				  wifi.wifi_reboot_required = 1;
//			  }
//		  }


		  if ((1 == wifi.ready_status) && (1 == wifi.connection_status) && (0 == wifi.wifi_manager_status) && (1 == wifi.time_status) &&
				  (1 == wifi.wifi_info_status) && (1 == wifi.thingsboard_broker_status))
		  {
			  uint16_t length = check_if_any_uplink_to_be_published(wifi.uplink_message_buffer, sizeof(wifi.uplink_message_buffer));

			  if (length > 0)
			  {
				  debug.printf("stm tx data: %s\r\n", wifi.uplink_message_buffer);
				  wifi.printf(wifi.uplink_message_buffer);
			  }
		  }
	  }

	  osDelay(1000);

  }
  /* USER CODE END wifi_thread_task */
}




/* USER CODE BEGIN Header_wifi_processing_task */
/**
* @brief Function implementing the wifi_processing thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_wifi_processing_task */
void wifi_processing_task(void const * argument)
{
  /* USER CODE BEGIN wifi_processing_task */

	osDelay(5000);

  /* Infinite loop */
  for(;;)
  {
	  if (1 == wifi_isr_semaphore.acquire(1000))
	  {
		  wifi.process_wifi_data();
	  }

	  osDelay(1000);
  }
  /* USER CODE END wifi_processing_task */
}



void wifi_t::my_callback(UART_HandleTypeDef * uart)
{
	wifi.rx_byte = wifi.huart->Instance->RDR;

	if(wifi.rx_counter >= (MAX_WIFI_BUFFER_SIZE - 4))
	{
		wifi.rx_counter = 0;
	}

	if(wifi.rx_byte == 0)
	{

	}

	else if(wifi.rx_byte == '\n')
	{
		wifi.downlink_data_buffer[wifi.rx_counter++] = wifi.rx_byte;
		wifi.downlink_data_buffer[wifi.rx_counter++] = '\0';

		if(wifi.rx_counter > 6)
		{
			memset(wifi.downlink_processing_buffer, 0, sizeof(wifi.downlink_processing_buffer));
			memcpy(wifi.downlink_processing_buffer, wifi.downlink_data_buffer, wifi.rx_counter);

			wifi.rx_counter = 0;

			wifi_isr_semaphore.acquire(0);
			uint8_t res = wifi_isr_semaphore.release();
		}
		else
		{
			wifi.rx_counter = 0;
		}

	}

	else
	{
		wifi.downlink_data_buffer[wifi.rx_counter++] = wifi.rx_byte;
	}

	HAL_UART_Receive_IT(wifi.huart, (uint8_t *)&wifi.usart_rx_byte , 1);
}
