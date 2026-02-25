/*
 * downlink_t.cpp
 *
 *  Created on: Oct 6, 2025
 *      Author: Amogh MP
 */



#include "downlink_t.h"
#include "semaphore_t.h"
#include "uplink_management.h"
#include "device_info.h"
#include "realtimeclock.h"
#include "peripherals_t.h"
#include "ArduinoJson.h"
#include "cellular.h"
#include "lorawan_stm32wl.h"
#include "utilities.h"



downlink_t downlink;



downlink_t::downlink_t()
{

}



downlink_t::~downlink_t()
{

}


uint8_t downlink_t::parse_set_config_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(512);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_set_command_downlink_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "SET_CONFIG"
			const char* device_id = doc["device_id"]; // "1234567890"
			long epoch_time = doc["epoch_time"]; // "234567890"
			int sampling_frequency = doc["sampling_frequency"]; // 30
//			long max_tank_capacity = doc["max_tank_capacity"]; // 300000
//			int tank_length = doc["tank_length"]; // 10
//			int tank_width = doc["tank_width"]; // 10
//			int tank_height = doc["tank_height"]; // 6
			int wifi_enabled = doc["wifi_enabled"]; // 1
			int cellular_enabled = doc["cellular_enabled"]; // 1
			int lorawan_enabled = doc["lorawan_enabled"]; // 1
			int gps_enabled = doc["gps_enabled"]; // 1
			int lbs_enabled = doc["lbs_enabled"];

   /*Shivani*/

			const char* heater_control_mode=doc["heater_control_mode"];
			const char *poweron_heater = doc["poweron_heater"];
			int maximum_temperature=doc["maximum_temperature"];
			int minimum_temperature=doc["minimum_temperature"];
			int maximum_humidity=doc["maximum_humidity"];
			int minimum_humidity=doc["minimum_humidity"];
			int light_intensity=doc["light_intensity"];
			int fan_speed=doc["fan_speed"];



			if ((strstr(device_id, read_device_info.device_id) != NULL) && (epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME) &&
					(sampling_frequency >= DEFAULT_DATA_SAMPLING_FREQUENCY_IN_SEC) && (sampling_frequency < MAX_SAMPLING_FREQUENCY_IN_SECONDS) &&
					(heater_control_mode != NULL) && (poweron_heater !=NULL) && (maximum_temperature <= MAX_TEMP_ALLOWED) && (minimum_temperature >= MIN_TEMP_ALLOWED ) && (maximum_humidity <= 100) &&
					(minimum_humidity >=0) && (maximum_temperature>minimum_temperature) && (maximum_humidity>minimum_humidity) && (light_intensity >=0) && (fan_speed >=0))
			{
				rtc.set_time_in_utc(epoch_time);

				write_device_info.data_sampling_frequency_in_sec = sampling_frequency;

//				write_device_info.max_tank_capacity_in_litres = max_tank_capacity;
//				write_device_info.tank_length = tank_length;
//				write_device_info.tank_width = tank_width;
//				write_device_info.tank_height = tank_height;





				if (wifi_enabled && cellular_enabled && lorawan_enabled)
				{
					wifi_enabled = 0;
					lorawan_enabled = 0;
					cellular_enabled = 1;
				}
				else if (wifi_enabled && cellular_enabled)
				{
					wifi_enabled = 0;
					cellular_enabled = 1;
				}
				else if (wifi_enabled && lorawan_enabled)
				{
					lorawan_enabled = 0;
					wifi_enabled = 1;
				}
				else if (cellular_enabled && lorawan_enabled)
				{
					lorawan_enabled = 0;
					cellular_enabled = 1;
				}


				if (cellular_enabled)
				{
					write_device_info.communication_mode_enabled = cellular_enable;
				}

				else if (wifi_enabled)
				{
					write_device_info.communication_mode_enabled = wifi_enable;
				}

				else if (lorawan_enabled)
				{
					write_device_info.communication_mode_enabled = lorawan_enable;
				}


				peripherals.wifi_enable = wifi_enabled;
				peripherals.cellular_enable = cellular_enabled;
				peripherals.lorawan_enable = lorawan_enabled;

				peripherals.gps_enable = gps_enabled;
				peripherals.lbs_enable = lbs_enabled;

				if(heater_manual)
				{
				write_device_info.heater_control_mode = heater_manual;
				}

				else if (heater_auto)
				{
				write_device_info.heater_control_mode = heater_auto;
				}


				if(heater_off)
				{
				write_device_info.poweron_heater=heater_off;

				}

				else if(heater_on)
				{
				write_device_info.poweron_heater= heater_on;
				}

//			    write_device_info.heater_control_mode = heater_control_mode;
//				write_device_info.poweron_heater = poweron_heater;
				write_device_info.maximum_temperature = maximum_temperature;
				write_device_info.minimum_temperature = minimum_temperature;
				write_device_info.maximum_humidity = maximum_humidity;
				write_device_info.minimum_humidity = minimum_humidity;
				write_device_info.light_intensity =light_intensity;
				write_device_info.fan_speed = fan_speed;

				peripherals.store_to_eeprom();
				osDelay(100);
				peripherals.read_from_eeprom();

				write_device_info.update();
//				osDelay(100);
//				read_device_info.get();

				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		uint32_t epoch_time = 0;
		memcpy(&epoch_time, &processing_buffer[9], 4);

		uint16_t sampling_freq = 0;
		memcpy(&sampling_freq, &processing_buffer[13], 2);

//		uint32_t tank_capacity = 0;
//		memcpy(&tank_capacity, &processing_buffer[15], 4);
//
//		uint8_t tank_length = processing_buffer[19];
//		uint8_t tank_width = processing_buffer[20];
//		uint8_t tank_height = processing_buffer[21];

		        uint8_t wifi_enabled = processing_buffer[15];
				uint8_t cellular_enabled = processing_buffer[16];
				uint8_t lorawan_enabled = processing_buffer[17];
				uint8_t gps_enabled = processing_buffer[18];
				uint8_t lbs_enabled = processing_buffer[19];

				/*shivani*/
				uint8_t heater_control_mode = processing_buffer[20];
				uint8_t poweron_heater = processing_buffer[21];
				int16_t maximum_temperature = 0;
				memcpy(&maximum_temperature, &processing_buffer[22],2);
				int16_t minimum_temperature = 0;
		        memcpy(&minimum_temperature, &processing_buffer[24],2);
				uint8_t maximum_humidity= processing_buffer[26];
				uint8_t minimum_humidity= processing_buffer[27];
		        uint8_t light_intensity= processing_buffer[28];
		        uint8_t fan_speed= processing_buffer[29];




		debug.printf("device_id: %s, epoch_time: %lu, sampling_freq: %X, "
				//"tank_capacity: %X, tank_length: %X, tank_width: %X, tank_height: %X, "
				"wifi_enabled: %X, cellular_enabled: %X, lorawan_enabled: %X, gps_enabled: %X, lbs_enabled: %X,"
				"heater_control_mode: %u, poweron_heater=%u,  maximum_temperature=%d, minimum_temperature=%d, maximum_humidity=%u,"
				"minimum_humidity=%u, light_intensity=%u, fan_speed=%u \r\n",


     			device_id, epoch_time, sampling_freq,
				//tank_capacity, tank_length, tank_width, tank_height,
				wifi_enabled, cellular_enabled, lorawan_enabled, gps_enabled, lbs_enabled,
				heater_control_mode,poweron_heater,maximum_temperature,minimum_temperature,maximum_humidity,minimum_humidity,
				light_intensity,fan_speed);

		if ((strstr(device_id, read_device_info.lorawan_device_eui) != NULL) && (epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME) &&
				(sampling_freq >= DEFAULT_DATA_SAMPLING_FREQUENCY_IN_SEC) && (sampling_freq < MAX_SAMPLING_FREQUENCY_IN_SECONDS) &&

				/*(tank_capacity != 0) && (tank_length != 0) && (tank_width != 0) && (tank_height != 0) &&*/ (wifi_enabled || cellular_enabled || lorawan_enabled)
				&&(heater_control_mode == 0 || heater_control_mode == 1) && (poweron_heater == 0 || poweron_heater == 1) && (maximum_temperature <= MAX_TEMP_ALLOWED) &&
				(minimum_temperature >= MIN_TEMP_ALLOWED ) && (maximum_humidity <= 100) && (minimum_humidity >=0) && (maximum_temperature>minimum_temperature) &&
				(maximum_humidity>minimum_humidity) && (light_intensity >=0) && (fan_speed >=0))

		{
			rtc.set_time_in_utc(epoch_time);

			write_device_info.data_sampling_frequency_in_sec = sampling_freq;

//			write_device_info.max_tank_capacity_in_litres = tank_capacity;
//			write_device_info.tank_length = tank_length;
//			write_device_info.tank_width = tank_width;
//			write_device_info.tank_height = tank_height;

			if (wifi_enabled && cellular_enabled && lorawan_enabled)
			{
				wifi_enabled = 0;
				lorawan_enabled = 0;
				cellular_enabled = 1;
			}
			else if (wifi_enabled && cellular_enabled)
			{
				wifi_enabled = 0;
				cellular_enabled = 1;
			}
			else if (wifi_enabled && lorawan_enabled)
			{
				lorawan_enabled = 0;
				wifi_enabled = 1;
			}
			else if (cellular_enabled && lorawan_enabled)
			{
				lorawan_enabled = 0;
				cellular_enabled = 1;
			}


			if (cellular_enabled)
			{
				write_device_info.communication_mode_enabled = cellular_enable;
			}

			else if (wifi_enabled)
			{
				write_device_info.communication_mode_enabled = wifi_enable;
			}

			else if (lorawan_enabled)
			{
				write_device_info.communication_mode_enabled = lorawan_enable;
			}


			peripherals.wifi_enable = wifi_enabled;
			peripherals.cellular_enable = cellular_enabled;
			peripherals.lorawan_enable = lorawan_enabled;

			peripherals.gps_enable = gps_enabled;
			peripherals.lbs_enable = lbs_enabled;

			if(heater_manual)
			{
			write_device_info.poweron_heater = heater_manual;

			}

			else if(heater_auto){
			write_device_info.poweron_heater = heater_auto;

			}
			if(heater_off)
	     	{
			write_device_info.heater_control_mode = heater_off;

		     }

			else if(heater_on){
			write_device_info.heater_control_mode = heater_on;

			}


			write_device_info.maximum_temperature = maximum_temperature;
			write_device_info.minimum_temperature = minimum_temperature;
			write_device_info.maximum_humidity = maximum_humidity;
			write_device_info.minimum_humidity = minimum_humidity;
			write_device_info.light_intensity =light_intensity;
			write_device_info.fan_speed = fan_speed;

			peripherals.store_to_eeprom();
			osDelay(100);
			peripherals.read_from_eeprom();

			write_device_info.update();
//			osDelay(100);
//			read_device_info.get();


			debug.printf("write_device_info.communication_mode_enabled: %d, read_device_info.communication_mode_enabled: %d, "
					"peripherals.wifi_enable: %d, peripherals.cellular_enable: %d, peripherals.lorawan_enable: %d\r\n",
					write_device_info.communication_mode_enabled, read_device_info.communication_mode_enabled,
					peripherals.wifi_enable, peripherals.cellular_enable, peripherals.lorawan_enable);


			result = 1;
		}
	}

	return result;
}


uint8_t downlink_t::parse_get_config_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(128);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_get_command_downlink_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "SET_CONFIG"
			const char* device_id = doc["device_id"]; // "1234567890"

			if (strstr(device_id, read_device_info.device_id) != NULL)
			{
				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		if (strstr(device_id, read_device_info.lorawan_device_eui) != NULL)
		{
			result = 1;
		}
	}

	return result;
}



uint8_t downlink_t::parse_configure_apn_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(256);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_configure_apn_downlink_command parse failed\r\n");
			result = 0;
		}

		const char* command = doc["command"]; // "CONFIGURE_APN"
		const char* device_id = doc["device_id"]; // "1234567890"
		long epoch_time = doc["epoch_time"]; // 234567890
		const char* apn = doc["apn"]; // "jionet"

		if ((strstr(device_id, read_device_info.device_id) != NULL) && (0 != strlen(apn)) &&
				(epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME))
		{
			rtc.set_time_in_utc(epoch_time);

			sprintf(write_device_info.apn, "%s", apn);

			result = 1;
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		uint32_t epoch_time = 0;
		memcpy(&epoch_time, &processing_buffer[9], 4);

		char apn[17] = {0};
		memcpy(apn, &processing_buffer[13], 16);

		debug.printf("device_id: %s, epoch: %lu, apn: %s\r\n", device_id, epoch_time, write_device_info.apn);

		if ((strstr(device_id, read_device_info.lorawan_device_eui) != NULL) && (0 != strlen(apn)) &&
				(epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME))
		{
			rtc.set_time_in_utc(epoch_time);

			sprintf(write_device_info.apn, "%s", apn);

			result = 1;
		}
	}

	return result;
}


uint8_t downlink_t::parse_reboot_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(128);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_reboot_downlink_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "REBOOT"
			const char* device_id = doc["device_id"]; // "1234567890"

			if (strstr(device_id, read_device_info.device_id) != NULL)
			{
				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		if (strstr(device_id, read_device_info.lorawan_device_eui) != NULL)
		{
			result = 1;
		}
	}

	return result;
}


uint8_t downlink_t::parse_reset_to_factory_defaults_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(128);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_reset_to_factory_defaults_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "RESET_TO_FACTORY_DEFUALTS"
			const char* device_id = doc["device_id"]; // "1234567890"

			if (strstr(device_id, read_device_info.device_id) != NULL)
			{
				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		if (strstr(device_id, read_device_info.lorawan_device_eui) != NULL)
		{
			result = 1;
		}
	}

	return result;
}


uint8_t downlink_t::parse_turn_off_edge_device_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(128);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_turn_off_edge_device_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "TURN_OFF_EDGE_DEVICE"
			const char* device_id = doc["device_id"]; // "1234567890"

			if (strstr(device_id, read_device_info.device_id) != NULL)
			{
				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		if (strstr(device_id, read_device_info.lorawan_device_eui) != NULL)
		{
			result = 1;
		}
	}

	return result;
}


uint8_t downlink_t::parse_set_lorawan_config_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(1024);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_set_lorawan_config_downlink_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "SET_LORAWAN_CONFIG"
			const char* device_id = doc["device_id"]; // "1234567890"
			long epoch_time = doc["epoch_time"]; // 234567890
			const char* lorawan_app_key = doc["lorawan_app_key"];
			const char* lorawan_nwk_key = doc["lorawan_nwk_key"];
			const char* lorawan_app_session_key = doc["lorawan_app_session_key"];
			const char* lorawan_nwk_session_key = doc["lorawan_nwk_session_key"];
			const char* lorawan_device_eui = doc["lorawan_device_eui"]; // "98,3A,DC,45,23,8E,89,A3"
			const char* lorawan_join_eui = doc["lorawan_join_eui"]; // "01,01,01,01,01,01,01,01"
			const char* lorawan_device_address = doc["lorawan_device_address"]; // "01,30,70,E5"


			debug.printf("read_device_info.device_id: %s, device_id: %s\r\n", read_device_info.device_id, device_id);

			if ((strstr(device_id, read_device_info.device_id) != NULL) && (0 != strlen(lorawan_app_key)) && (0 != strlen(lorawan_nwk_key)) &&
					(0 != strlen(lorawan_app_session_key)) && (0 != strlen(lorawan_nwk_session_key)) &&
					(0 != strlen(lorawan_device_eui)) && (0 != strlen(lorawan_join_eui)) && (0 != strlen(lorawan_device_address)) &&
					(epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME))
			{
				rtc.set_time_in_utc(epoch_time);


				snprintf(write_device_info.lorawan_app_key, 33, "%s", lorawan_app_key);
				snprintf(write_device_info.lorawan_nwk_key, 33, "%s", lorawan_nwk_key);
				snprintf(write_device_info.lorawan_app_session_key, 33, "%s", lorawan_app_session_key);
				snprintf(write_device_info.lorawan_nwk_session_key, 33, "%s", lorawan_nwk_session_key);

				snprintf(write_device_info.lorawan_device_eui, 17, "%s", lorawan_device_eui);
				snprintf(write_device_info.lorawan_join_eui, 17, "%s", lorawan_join_eui);

				snprintf(write_device_info.lorawan_device_address, 9, "%s", lorawan_device_address);


				debug.printf("write_device_info.lorawan_app_key: %s\r\n", write_device_info.lorawan_app_key);
				debug.printf("write_device_info.lorawan_nwk_key: %s\r\n", write_device_info.lorawan_nwk_key);
				debug.printf("write_device_info.lorawan_app_session_key: %s\r\n", write_device_info.lorawan_app_session_key);
				debug.printf("write_device_info.lorawan_nwk_session_key: %s\r\n", write_device_info.lorawan_nwk_session_key);
				debug.printf("write_device_info.lorawan_device_eui: %s\r\n", write_device_info.lorawan_device_eui);
				debug.printf("write_device_info.lorawan_join_eui: %s\r\n", write_device_info.lorawan_join_eui);
				debug.printf("write_device_info.lorawan_device_address: %s\r\n", write_device_info.lorawan_device_address);


				write_device_info.update();

				result = 1;
			}
		}
	}

	else if (lorawan_enable == read_device_info.communication_mode_enabled)
	{
		char device_id[16] = {'\0'};
		sprintf(device_id, "%02X%02X%02X%02X%02X%02X%02X%02X", processing_buffer[1], processing_buffer[2], processing_buffer[3],
				processing_buffer[4], processing_buffer[5], processing_buffer[6], processing_buffer[7], processing_buffer[8]);

		uint32_t epoch_time = 0;
		memcpy(&epoch_time, &processing_buffer[9], 4);


		uint8_t app_key[16] = {'\0'};
		memcpy(app_key, &processing_buffer[13], 16);


		uint8_t nwk_key[16] = {'\0'};
		memcpy(nwk_key, &processing_buffer[29], 16);


		uint8_t app_session_key[16] = {'\0'};
		memcpy(app_session_key, &processing_buffer[45], 16);


		uint8_t nwk_session_key[16] = {'\0'};
		memcpy(nwk_session_key, &processing_buffer[61], 16);


		uint8_t device_eui[8] = {'\0'};
		memcpy(device_eui, &processing_buffer[77], 8);


		uint8_t join_eui[8] = {'\0'};
		memcpy(join_eui, &processing_buffer[85], 8);


		uint8_t dev_address[4] = {'\0'};
		memcpy(dev_address, &processing_buffer[93], 4);


		debug.printf("app_key: ");
		for (uint8_t i = 0 ; i < 16 ; i++)
		{
			debug.printf("%02X", app_key[i]);
		}
		debug.printf("\r\n");


		debug.printf("nwk_key: ");
		for (uint8_t i = 0 ; i < 16 ; i++)
		{
			debug.printf("%02X", nwk_key[i]);
		}
		debug.printf("\r\n");


		debug.printf("app_session_key: ");
		for (uint8_t i = 0 ; i < 16 ; i++)
		{
			debug.printf("%02X", app_session_key[i]);
		}
		debug.printf("\r\n");


		debug.printf("nwk_session_key: ");
		for (uint8_t i = 0 ; i < 16 ; i++)
		{
			debug.printf("%02X", nwk_session_key[i]);
		}
		debug.printf("\r\n");


		debug.printf("device_eui: ");
		for (uint8_t i = 0 ; i < 8 ; i++)
		{
			debug.printf("%02X", device_eui[i]);
		}
		debug.printf("\r\n");


		debug.printf("join_eui: ");
		for (uint8_t i = 0 ; i < 8 ; i++)
		{
			debug.printf("%02X", join_eui[i]);
		}
		debug.printf("\r\n");


		debug.printf("dev_address: ");
		for (uint8_t i = 0 ; i < 4 ; i++)
		{
			debug.printf("%02X", dev_address[i]);
		}
		debug.printf("\r\n");


		debug.printf("epoch_time: %lu\r\n", epoch_time);


		if ((strstr(device_id, read_device_info.lorawan_device_eui) != NULL) && (0 != is_buffer_not_empty(app_key, 16)) &&
				(0 != is_buffer_not_empty(nwk_key, 16)) && (0 != is_buffer_not_empty(app_session_key, 16)) &&
				(0 != is_buffer_not_empty(nwk_session_key, 16)) && (0 != is_buffer_not_empty(device_eui, 8)) &&
				(0 != is_buffer_not_empty(join_eui, 8)) && (0 != is_buffer_not_empty(dev_address, 4)) &&
				(epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME))
		{
			rtc.set_time_in_utc(epoch_time);

			memset(write_device_info.lorawan_app_key, 0, sizeof(write_device_info.lorawan_app_key));
			memset(write_device_info.lorawan_nwk_key, 0, sizeof(write_device_info.lorawan_nwk_key));
			memset(write_device_info.lorawan_app_session_key, 0, sizeof(write_device_info.lorawan_app_session_key));
			memset(write_device_info.lorawan_nwk_session_key, 0, sizeof(write_device_info.lorawan_nwk_session_key));
			memset(write_device_info.lorawan_device_eui, 0, sizeof(write_device_info.lorawan_device_eui));
			memset(write_device_info.lorawan_join_eui, 0, sizeof(write_device_info.lorawan_join_eui));
			memset(write_device_info.lorawan_device_address, 0, sizeof(write_device_info.lorawan_device_address));

			for (int i = 0; i < 16; i++)
			{
				sprintf(&write_device_info.lorawan_app_key[i * 2], "%02X", processing_buffer[13 + i]);
			}

			for (int i = 0; i < 16; i++)
			{
				sprintf(&write_device_info.lorawan_nwk_key[i * 2], "%02X", processing_buffer[29 + i]);
			}

			for (int i = 0; i < 16; i++)
			{
				sprintf(&write_device_info.lorawan_app_session_key[i * 2], "%02X", processing_buffer[45 + i]);
			}

			for (int i = 0; i < 16; i++)
			{
				sprintf(&write_device_info.lorawan_nwk_session_key[i * 2], "%02X", processing_buffer[61 + i]);
			}

			for (int i = 0; i < 8; i++)
			{
				sprintf(&write_device_info.lorawan_device_eui[i * 2], "%02X", processing_buffer[77 + i]);
			}

			for (int i = 0; i < 8; i++)
			{
				sprintf(&write_device_info.lorawan_join_eui[i * 2], "%02X", processing_buffer[85 + i]);
			}

			for (int i = 0; i < 4; i++)
			{
				sprintf(&write_device_info.lorawan_device_address[i * 2], "%02X", processing_buffer[93 + i]);
			}


			write_device_info.update();

			result = 1;
		}
	}

	return result;
}



uint8_t downlink_t::parse_configure_mqtt_endpoint_downlink_command(void)
{
	uint8_t result = 0;

	if (lorawan_enable != read_device_info.communication_mode_enabled)
	{
		DynamicJsonDocument doc(1024);

		DeserializationError error = deserializeJson(doc, processing_buffer);

		if (error)
		{
			debug.printf("parse_configure_mqtt_endpoint_downlink_command parse failed\r\n");
			result = 0;
		}

		else
		{
			const char* command = doc["command"]; // "CONFIGURE_MQTT_ENDPOINT"
			const char* device_id = doc["device_id"]; // "1234567890"
			long epoch_time = doc["epoch_time"]; // 234567890
			const char* mqtt_endpoint = doc["mqtt_endpoint"]; // "devices.antzsystems.com"
			int mqtt_endpoint_port_number = doc["mqtt_endpoint_port_number"]; // 8883
			const char* username = doc["username"]; // ",djvkajdc"
			const char* password = doc["password"]; // "SKDjcnksC"
			const char* uplink_topic = doc["uplink_topic"]; // "v1/devices/me/telemetry/swjefcbwksc"
			const char* downlink_topic = doc["downlink_topic"]; // "v1/devices/me/attributes/swjefcbwksc"


			if ((strstr(device_id, read_device_info.device_id) != NULL) && (epoch_time > JANUARY_1_2025_EPOCH_TIME) && (epoch_time < JANUARY_1_2050_EPOCH_TIME) &&
				(strlen(mqtt_endpoint) != 0) && (strlen(username) != 0) && (strlen(password) != 0) &&
				(strlen(uplink_topic) != 0) && (strlen(downlink_topic) != 0) && (mqtt_endpoint_port_number != 0))
			{
				rtc.set_time_in_utc(epoch_time);

				sprintf(write_device_info.mqtt_broker_endpoint, "%s", mqtt_endpoint);
				sprintf(write_device_info.mqtt_username, "%s", username); // TODO: AMP 23-01-2026
				sprintf(write_device_info.mqtt_password, "%s", password); // TODO: AMP 23-01-2026
				sprintf(write_device_info.device_uplink_telemetry_topic, "%s", uplink_topic);
				sprintf(write_device_info.device_downlink_topic, "%s", downlink_topic);

				write_device_info.port_number = mqtt_endpoint_port_number;

				result = 1;
			}
		}
	}

	return result;
}



void downlink_t::process_received_downlink(void)
{
	uint8_t result = 0;

	if ((strstr(processing_buffer, "SET_CONFIG") != NULL) || (SET_CONFIG_DOWNLINK_COMMAND_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received set config downlink\r\n");

		result = parse_set_config_downlink_command();

		if (1 == result)
		{
			next_message.set_config_ack = 1;
		}
	}

	else if ((strstr(processing_buffer, "GET_CONFIG") != NULL) || (GET_CONFIG_DOWNLINK_COMMAND_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received get config downlink\r\n");

		result = parse_get_config_downlink_command();

		if (1 == result)
		{
			next_message.get_config_ack = 1;
		}
	}


	else if ((strstr(processing_buffer, "CONFIGURE_APN") != NULL) || (CONFIGURE_APN_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received configure apn downlink\r\n");

		result = parse_configure_apn_downlink_command();

		if (1 == result)
		{
			write_device_info.update();

			next_message.configure_apn_ack = 1;
		}
	}

	else if ((strstr(processing_buffer, "REBOOT") != NULL) || (REBOOT_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received reboot downlink\r\n");

		result = parse_reboot_downlink_command();

		if (1 == result)
		{
			next_message.reboot_ack = 1;
		}
	}

	else if ((strstr(processing_buffer, "RESET_TO_FACTORY_DEFAULTS") != NULL) || (RESET_TO_FACTORY_DEFAULTS_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received reset to factory defaults downlink\r\n");

		result = parse_reset_to_factory_defaults_downlink_command();

		if (1 == result)
		{
			next_message.reset_to_factory_defaults_ack = 1;
		}
	}

	else if ((strstr(processing_buffer, "TURN_OFF_EDGE_DEVICE") != NULL) || (TURN_OFF_EDGE_DEVICE_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received turn-off edge device downlink\r\n");

		result = parse_turn_off_edge_device_downlink_command();

		if (1 == result)
		{
			next_message.turn_off_edge_device_ack = 1;
		}
	}

	else if ((strstr(processing_buffer, "SET_LORAWAN_CONFIG") != NULL) || (SET_LORAWAN_CONFIG_PACKET_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received set lorawan config downlink\r\n");

		result = parse_set_lorawan_config_downlink_command();

		if (1 == result)
		{
			if (lorawan_enable != read_device_info.communication_mode_enabled)
			{
				next_message.set_lorawan_config_ack = 1;
			}

			else if (lorawan_enable == read_device_info.communication_mode_enabled)
			{
				lorawan.validate_new_connection = 1;
			}
		}
	}

	else if ((strstr(processing_buffer, "CONFIGURE_MQTT_ENDPOINT") != NULL) || (CONFIGURE_MQTT_ENDPOINT_START_BYTE == processing_buffer[0]))
	{
		debug.printf("received configure mqtt endpoint downlink\r\n");

		result = parse_configure_mqtt_endpoint_downlink_command();

		if (1 == result)
		{
			debug.printf("parse_configure_mqtt_endpoint_downlink_command parse success\r\n");

			if (1 == cellular_mutex.lock(60 * 1000)) // TODO: 60 sec because if the device is fetching data from radar sensor, this activity has to be performed once that activity is concluded.
			{
				result = lte.open_mqtt_connection(write_device_info.mqtt_broker_endpoint, write_device_info.port_number);

				if (1 == result)
				{
					debug.printf("Successfully opened MQTT Connection using new Credentials\r\n");

					result = lte.connect_to_broker(write_device_info.mqtt_username, write_device_info.mqtt_password); // TODO: AMP 23-01-2026

					if (1 == result)
					{
						debug.printf("Successfully connected to MQTT using new Credentials\r\n");

						write_device_info.update();
						osDelay(500);
						read_device_info.get();

						next_message.set_mqtt_endpoint_ack = 1;
					}

					else
					{
						debug.printf("Failed to connect to MQTT using new Credentials\r\n");

						memset(write_device_info.mqtt_broker_endpoint, 0, sizeof(write_device_info.mqtt_broker_endpoint));
						memset(write_device_info.mqtt_username, 0, sizeof(write_device_info.mqtt_username));
						memset(write_device_info.mqtt_password, 0, sizeof(write_device_info.mqtt_password));
						memset(write_device_info.device_uplink_telemetry_topic, 0, sizeof(write_device_info.device_uplink_telemetry_topic));
						memset(write_device_info.device_downlink_topic, 0, sizeof(write_device_info.device_downlink_topic));

						write_device_info.port_number = 0;

						sprintf(write_device_info.mqtt_broker_endpoint, "%s", read_device_info.mqtt_broker_endpoint);
						sprintf(write_device_info.mqtt_username, "%s", read_device_info.mqtt_username); // TODO: AMP 12-01-2026
						sprintf(write_device_info.mqtt_password, "%s", read_device_info.mqtt_password); // TODO: AMP 12-01-2026
						sprintf(write_device_info.device_uplink_telemetry_topic, "%s", read_device_info.device_uplink_telemetry_topic);
						sprintf(write_device_info.device_downlink_topic, "%s", read_device_info.device_downlink_topic);

						write_device_info.port_number = read_device_info.port_number;

						lte.close_connection();
						lte.clear_all_status_flags();

						next_message.set_mqtt_endpoint_nack = 1;
					}
				}

				else
				{
					debug.printf("Failed to open MQTT Connection using new Credentials\r\n");

					memset(write_device_info.mqtt_broker_endpoint, 0, sizeof(write_device_info.mqtt_broker_endpoint));
					memset(write_device_info.mqtt_username, 0, sizeof(write_device_info.mqtt_username));
					memset(write_device_info.mqtt_password, 0, sizeof(write_device_info.mqtt_password));
					memset(write_device_info.device_uplink_telemetry_topic, 0, sizeof(write_device_info.device_uplink_telemetry_topic));
					memset(write_device_info.device_downlink_topic, 0, sizeof(write_device_info.device_downlink_topic));

					write_device_info.port_number = 0;


					sprintf(write_device_info.mqtt_broker_endpoint, "%s", read_device_info.mqtt_broker_endpoint);
					sprintf(write_device_info.mqtt_username, "%s", read_device_info.mqtt_username); // TODO: AMP 12-01-2026
					sprintf(write_device_info.mqtt_password, "%s", read_device_info.mqtt_password); // TODO: AMP 12-01-2026
					sprintf(write_device_info.device_uplink_telemetry_topic, "%s", read_device_info.device_uplink_telemetry_topic);
					sprintf(write_device_info.device_downlink_topic, "%s", read_device_info.device_downlink_topic);

					write_device_info.port_number = read_device_info.port_number;

					lte.close_connection();
					lte.clear_all_status_flags();

					next_message.set_mqtt_endpoint_nack = 1;
				}

				cellular_mutex.unlock();
			}
		}
	}

	next_message.update();
}




/* USER CODE BEGIN Header_downlink_thread_task */
/**
* @brief Function implementing the downlink_thread thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_downlink_thread_task */
void downlink_thread_task(void const * argument)
{
  /* USER CODE BEGIN downlink_thread_task */
  /* Infinite loop */
  for(;;)
  {
	  if (1 == downlink_semaphore.acquire(1000))
	  {
		  downlink.process_received_downlink();
	  }

	  osDelay(1000);
  }
  /* USER CODE END downlink_thread_task */
}
