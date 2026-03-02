/*
 * device_info.h
 *
 *  Created on: Jul 22, 2025
 *      Author: Amogh MP
 */

#ifndef INC_DEVICE_INFO_H_
#define INC_DEVICE_INFO_H_


#include "wifi.h"


#define DEVICE_ID_MAX_LENGTH								64

#define MIN_TEMP_ALLOWED                                   (-40)
#define MAX_TEMP_ALLOWED                                    (125)

typedef enum communication_mode_s
{
	wifi_enable = 1,
	cellular_enable,
	lorawan_enable
};




typedef enum poweron_heater_s
{
	heater_manual = 0,
    heater_auto = 1
};



typedef enum heater_control_mode_s{
	heater_off = 0,
	heater_on = 1
};


//#define DEFAULT_COMMUNICATION_MODE	 					wifi_enable

#define DEFAULT_DEVICE_HARDWARE_VERSION						"1.0.0"

#define DEFAULT_DEVICE_FIRMWARE_VERSION						"1.0.0"
#define DEFAULT_DEVICE_NEW_FIRMWARE_VERSION					"1.0.0"

#define DEFAULT_MQTT_BROKER_ENDPOINT						"broker.emqx.io" // "devices.antzsystems.com"
#define	DEFAULT_MQTT_UPLINK_TELEMETRY_TOPIC					"v1/devices/me/telemetry/amp" // "v1/devices/me/telemetry"
#define DEFAULT_MQTT_DOWNLINK_TOPIC							"v1/devices/me/attributes/amp" // "v1/devices/me/attributes/"
#define DEFAULT_MQTT_PORT_NUMBER							1883


#define DEFAULT_DEVICE_ID 									"amp_device"
#define DEFAULT_DEVICE_PROFILE								"amp_device_profile"
//#define	DEFAULT_DEVICE_ACCESS_TOKEN							"amp"
#define	DEFAULT_DEVICE_MQTT_USERNAME						"amp" // TODO: AMP 23-01-2026
#define	DEFAULT_DEVICE_MQTT_PASSWORD						"amp" // TODO: AMP 23-01-2026
#define DEFAULT_DEVICE_NAME									DEFAULT_DEVICE_ID

#define DEFAULT_WIFI_SSID									"AMP_WiFi"
#define DEFAULT_WIFI_PASSWORD								"amoghmp98"
#define DEFAULT_WIFI_AP_SSID								DEFAULT_DEVICE_ID
#define DEFAULT_WIFI_AP_PASSWORD							"control_card@12345"
#define DEFAULT_WIFI_AP_STATIC_IP							"192.168.4.1"


//#define DEFAULT_LORAWAN_APP_KEY								"23,7E,7D,E7,1B,FC,53,6F,C3,0F,18,A0,2B,92,5D,E6"
//#define DEFAULT_LORAWAN_NETWORK_KEY							"23,7E,7D,E7,1B,FC,53,6F,C3,0F,18,A0,2B,92,5D,E6"
//#define DEFAULT_LORAWAN_APP_SESSION_KEY						"23,7E,7D,E7,1B,FC,53,6F,C3,0F,18,A0,2B,92,5D,E6"
//#define DEFAULT_LORAWAN_NETWORK_SESSION_KEY					"23,7E,7D,E7,1B,FC,53,6F,C3,0F,18,A0,2B,92,5D,E6"
//#define DEFAULT_LORAWAN_DEVICE_EUI							"98,3A,DC,45,23,8E,89,A3"
//#define DEFAULT_LORAWAN_JOIN_EUI							"01,01,01,01,01,01,01,01"
//#define DEFAULT_LORAWAN_DEVICE_ADDRESS						"01,30,70,E5"

#define DEFAULT_LORAWAN_APP_KEY								"34FBA5B1B48E84A8889CE9CDDBC8A56E"
#define DEFAULT_LORAWAN_NETWORK_KEY							"34FBA5B1B48E84A8889CE9CDDBC8A56E"
#define DEFAULT_LORAWAN_APP_SESSION_KEY						"34FBA5B1B48E84A8889CE9CDDBC8A56E"
#define DEFAULT_LORAWAN_NETWORK_SESSION_KEY					"34FBA5B1B48E84A8889CE9CDDBC8A56E"
#define DEFAULT_LORAWAN_DEVICE_EUI							"2001202600000001"
#define DEFAULT_LORAWAN_JOIN_EUI							"0101010101010101"
#define DEFAULT_LORAWAN_DEVICE_ADDRESS						"0006EB5E"


#define DEFAULT_CELLULAR_APN						 		"jiociot2" // "jionet"





#define DEFAULT_DATA_SAMPLING_FREQUENCY_IN_SEC				30 // 300 // TODO: change this
#define MAX_SAMPLING_FREQUENCY_IN_SECONDS					64800 // 64800 sec = 18 hours
#define DEFAULT_HEALTH_PACKET_FREQUENCY_IN_SEC				120


#define DEFAULT_TIME_SYNC_FREQUENCY_IN_SEC					28800 // 28800 sec = 8 hours
#define DEFAULT_LORAWAN_REJOIN_FREQUENCY_IN_SEC				86400 // 86400 sec = 24 hours


//
//#define RADAR_SENSOR_CONGIGURED_HEIGHT_IN_METERS		10
//#define MAX_TANK_CAPACITY_IN_LITRES						300000
//#define TANK_LENGTH_IN_METERS							8
//#define TANK_WIDTH_IN_METERS							8
//#define TANK_HEIGHT_IN_METERS							8




extern char ca_certificate[2048];
extern char client_certificate[2048];
extern char clientkey[2048];


extern UART_HandleTypeDef huart6;


class device_info_t
{
	public:

	char mqtt_broker_endpoint[128];
	char device_uplink_telemetry_topic[128];
	char device_downlink_topic[128];

	char ssid[MAX_CREDENTIALS_TOKEN_LENGTH];
	char password[MAX_CREDENTIALS_TOKEN_LENGTH];
	char ap_ssid[DEVICE_ID_MAX_LENGTH];
	char ap_password[MAX_CREDENTIALS_TOKEN_LENGTH];
	char ap_static_ip[DEVICE_ID_MAX_LENGTH];

	char device_name[DEVICE_ID_MAX_LENGTH];
	char device_profile[MAX_CREDENTIALS_TOKEN_LENGTH];
//	char access_token[MAX_CREDENTIALS_TOKEN_LENGTH]; // TODO: AMP 23-01-2026
	char mqtt_username[MAX_CREDENTIALS_TOKEN_LENGTH]; // TODO: AMP 23-01-2026
	char mqtt_password[MAX_CREDENTIALS_TOKEN_LENGTH]; // TODO: AMP 23-01-2026
	char device_id[DEVICE_ID_MAX_LENGTH];


	char lorawan_app_key[MAX_CREDENTIALS_TOKEN_LENGTH];
	char lorawan_nwk_key[MAX_CREDENTIALS_TOKEN_LENGTH];
	char lorawan_app_session_key[MAX_CREDENTIALS_TOKEN_LENGTH];
	char lorawan_nwk_session_key[MAX_CREDENTIALS_TOKEN_LENGTH];
	char lorawan_device_eui[DEVICE_ID_MAX_LENGTH];
	char lorawan_join_eui[DEVICE_ID_MAX_LENGTH];
	char lorawan_device_address[16];


	char sim_imei_number[32];
	char sim_ccid_number[32];
	char apn[32];


//	uint32_t max_tank_capacity_in_litres;
//	uint8_t tank_length;
//	uint8_t tank_width;
//	uint8_t tank_height;


	int16_t maximum_temperature;
	int16_t minimum_temperature;
	uint8_t maximum_humidity;
	uint8_t minimum_humidity;
	//char poweron_heater[8];
	uint8_t poweron_heater;
	uint8_t light_intensity;
	uint8_t fan_speed;
	uint8_t heater_control_mode;
	//char heater_control_mode[16];




	char hw_ver[16];
	char fw_ver[16];
	char new_fw_ver[16];

	int rssi;

	uint32_t device_start_time;

	uint16_t data_sampling_frequency_in_sec;
	uint16_t port_number;

	uint8_t communication_mode_enabled;


	device_info_t(void);
	virtual ~device_info_t(void);

	uint8_t load_default_values();
	uint8_t update(void);
	void get(void);

	void device_reboot(void);
};

extern device_info_t write_device_info, read_device_info;



//typedef struct water_tank_data_s
//{
//	char device_id[DEVICE_ID_MAX_LENGTH];
//
//	uint32_t packet_number;
//	uint32_t sample_epoch_time;
//
//	char device_power_source[16];
//
//	//uint16_t water_level_in_percentage;
//
//	int16_t temperature; //TODO:Shivani
//	uint8_t humidity;  //TODO:Shivani
//
//
//	uint8_t device_battery_percentage;
//	uint8_t dummy[36];
//
//}water_tank_data_t;
//
//extern water_tank_data_t water_tank_write_data, water_tank_read_data;



typedef struct smart_night_cell_data_s
{
	char device_id[DEVICE_ID_MAX_LENGTH];

	uint32_t packet_number;
	uint32_t sample_epoch_time;


	char device_power_source[16];

	uint8_t device_battery_percentage;

	int16_t temperature; //TODO:Shivani
	uint8_t humidity;  //TODO:Shivani

	uint8_t dummy[36];

}smart_night_cell_data_t;

extern smart_night_cell_data_t night_cell_write_data, night_cell_read_data;



void doubleToString(double value, char* buffer, int precision = 6);
uint8_t process_battery_voltage(void);


#endif /* INC_DEVICE_INFO_H_ */
