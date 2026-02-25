/*
 * application_logic.cpp
 *
 *  Created on: Jan 2, 2026
 *      Author: Amogh MP
 */





#include "application_logic.h"
#include "lora_app.h"
#include "ArduinoJson.h"
#include "usart.h"
#include "LmHandler.h"

#include "secure-element.h"
#include "stm32_seq.h"
#include "utilities_def.h"



extern uint8_t uart_data_rx_buffer[RX_BUFF_SIZE];  // TODO: AMP 02/01/2026
extern uint8_t uart_data_processing_buffer[RX_BUFF_SIZE]; // TODO: AMP 02/01/2026
extern uint8_t debug_buffer[RX_BUFF_SIZE]; // TODO: AMP 02/01/2026
extern uint8_t lorawan_tx_buffer[RX_BUFF_SIZE];

extern uint16_t lorawan_tx_data_length;
extern uint16_t uart_data_rx_length;

extern uint16_t downlink_data_rx_length;

extern uint8_t downlink_data_rx_buffer[RX_BUFF_SIZE];

extern ActivationType_t ActivationType;

extern uint8_t device_join_status;

extern UTIL_TIMER_Object_t TxTimer;
extern UTIL_TIMER_Object_t run_state_machine_timer;


uint8_t dev_address[4] = {0};

uint8_t dev_eui[8] = {0};
uint8_t join_eui[8] = {0};

uint8_t dev_app_key[16] = {0};
uint8_t dev_nwk_key[16] = {0};
uint8_t dev_app_session_key[16] = {0};
uint8_t dev_nwk_session_key[16] = {0};


app_state_t current_state = STATE_IDLE;


extern UTIL_TIMER_Object_t JoinBackoffTimer;
void OnJoinBackoffTimerEvent(void *context);


extern "C"
{

#include "LmhpClockSync.h"


void send_data_over_uart(char* str)
{
//	MX_USART2_UART_Init();
	HAL_UART_Transmit(&huart2, (uint8_t*)str, strlen(str), 1000);
//	HAL_UART_DeInit(&huart2);
}

void string_to_hex(const char* str, uint8_t* holder, uint8_t size)
{
    if (str != NULL && holder != NULL)
    {
		size_t len = strlen(str);

		for (uint8_t i = 0 ; i < size ; i++)
		{
			if (i * 2 + 1 < len)
			{
				char byteString[3];

				byteString[0] = str[i * 2];
				byteString[1] = str[i * 2 + 1];
				byteString[2] = '\0';

				holder[i] = (uint8_t)strtol(byteString, NULL, 16);
			}
			else
			{
				holder[i] = 0;
			}
		}
    }
}



void hex_to_string(const uint8_t* holder, uint8_t size, char* str)
{
    if (holder != NULL && str != NULL)
    {
        for (uint8_t i = 0; i < size; i++)
        {
            sprintf(str + (i * 2), "%02X", holder[i]);
        }

        str[size * 2] = '\0';
    }
}



uint8_t parse_connect_to_lorawan_network_packet(void)
{
	uint8_t result = 0;

	DynamicJsonDocument doc(1024);

	DeserializationError error = deserializeJson(doc, uart_data_processing_buffer);

	if (error)
	{
		result = 0;
	}

	else
	{
		const char* command = doc["command"]; // "CONNECT_TO_LORAWAN_NETWORK"
		const char* lorawan_app_key = doc["lorawan_app_key"];
		const char* lorawan_nwk_key = doc["lorawan_nwk_key"];
		const char* lorawan_app_session_key = doc["lorawan_app_session_key"];
		const char* lorawan_nwk_session_key = doc["lorawan_nwk_session_key"];
		const char* lorawan_device_eui = doc["lorawan_device_eui"]; // "98,3A,DC,45,23,8E,89,A3"
		const char* lorawan_join_eui = doc["lorawan_join_eui"]; // "01,01,01,01,01,01,01,01"
		const char* lorawan_device_address = doc["lorawan_device_address"]; // "01,30,70,E5"

		if ((0 != strlen(lorawan_app_key)) && (0 != strlen(lorawan_nwk_key)) &&
				(0 != strlen(lorawan_app_session_key)) && (0 != strlen(lorawan_nwk_session_key)) &&
				(0 != strlen(lorawan_device_eui)) && (0 != strlen(lorawan_join_eui)) && (0 != strlen(lorawan_device_address)))
		{
			string_to_hex(lorawan_device_address, dev_address, 4);

			string_to_hex(lorawan_device_eui, dev_eui, 8);
			string_to_hex(lorawan_join_eui, join_eui, 8);

			string_to_hex(lorawan_app_key, dev_app_key, 16);
			string_to_hex(lorawan_nwk_key, dev_nwk_key, 16);
			string_to_hex(lorawan_app_session_key, dev_app_session_key, 16);
			string_to_hex(lorawan_nwk_session_key, dev_nwk_session_key, 16);



//			dev_eui[0] = 0x99;
//			dev_eui[1] = 0x88;
//			dev_eui[2] = 0x86;
//			dev_eui[3] = 0x66;
//			dev_eui[4] = 0x34;
//			dev_eui[5] = 0x56;
//			dev_eui[6] = 0x78;
//			dev_eui[7] = 0x90;


			result = 1;


			sprintf((char *)debug_buffer, "dev_address: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_address) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_address[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "lorawan_device_eui: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_eui) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_eui[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "join_eui: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(join_eui) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", join_eui[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "dev_app_key: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_app_key) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_app_key[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "dev_nwk_key: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_nwk_key) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_nwk_key[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "dev_app_session_key: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_app_session_key) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_app_session_key[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);




			sprintf((char *)debug_buffer, "dev_nwk_session_key: ");
			send_data_over_uart((char *)debug_buffer);

			for (uint8_t i = 0 ; i < sizeof(dev_nwk_session_key) ; i++)
			{
				sprintf((char *)debug_buffer, "%02X", dev_nwk_session_key[i]);
				send_data_over_uart((char *)debug_buffer);
			}

			sprintf((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);

		}
	}

	return result;
}


uint16_t serialize_lorawan_application_reset_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(64);

	doc["command"] = "LORAWAN_APPLICATION_RESET";

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}



uint16_t serialize_ready_ack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(64);

	doc["command"] = "READY_ACK";

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t serialize_connect_to_lorawan_network_nack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(64);

	doc["command"] = "CONNECT_TO_LORAWAN_NETWORK_NACK";

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t serialize_connect_to_lorawan_network_ack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

    DynamicJsonDocument doc(1024);

    char hexBuffer[33];

    doc["command"] = "CONNECT_TO_LORAWAN_NETWORK_ACK";

    hex_to_string(dev_app_key, 16, hexBuffer);
    doc["lorawan_app_key"] = (char*)hexBuffer;

    hex_to_string(dev_nwk_key, 16, hexBuffer);
    doc["lorawan_nwk_key"] = (char*)hexBuffer;

    hex_to_string(dev_app_session_key, 16, hexBuffer);
    doc["lorawan_app_session_key"] = (char*)hexBuffer;

    hex_to_string(dev_nwk_session_key, 16, hexBuffer);
    doc["lorawan_nwk_session_key"] = (char*)hexBuffer;

    hex_to_string(dev_eui, 8, hexBuffer);
    doc["lorawan_device_eui"] = (char*)hexBuffer;

    hex_to_string(join_eui, 8, hexBuffer);
    doc["lorawan_join_eui"] = (char*)hexBuffer;

    hex_to_string(dev_address, 4, hexBuffer);
    doc["lorawan_device_address"] = (char*)hexBuffer;

    serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t serialize_get_time_ack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(128);

	doc["command"] = "GET_TIME_ACK";
	doc["epoch_time"] = SysTimeGet().Seconds;

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t serialize_uplink_data_tx_ack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(128);

	doc["command"] = "UPLINK_PAYLOAD_TX_ACK";
	doc["epoch_time"] = SysTimeGet().Seconds;

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t serialize_uplink_data_tx_nack_packet(char * buffer, uint16_t size)
{
	uint16_t length = 0;

	DynamicJsonDocument doc(128);

	doc["command"] = "UPLINK_PAYLOAD_TX_NACK";
	doc["epoch_time"] = SysTimeGet().Seconds;

	serializeJson(doc, buffer, size);

	length = strlen(buffer);

	return length;
}


uint16_t parse_data_payload_packet(uint16_t received_data_length)
{
	uint8_t packet_type = uart_data_processing_buffer[1];
	uint16_t actual_data_payload_length = uart_data_processing_buffer[2];

//	sprintf((char *)debug_buffer, "actual data payload length: %d, received_data_length: %d\r\n",
//			actual_data_payload_length, received_data_length);
//	send_data_over_uart((char *)debug_buffer);

	if (received_data_length == (actual_data_payload_length + DATA_HEADER_LENGTH + 1)) // +1 because of '\n' at last
	{
		return actual_data_payload_length;
	}

	return 0;
}


void run_state_machine(void)
{
	uint16_t len = 0;

	if (STATE_IDLE == current_state)
	{
		len = serialize_lorawan_application_reset_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_READY == current_state)
	{
		UTIL_TIMER_Stop(&run_state_machine_timer);

		len = serialize_ready_ack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_JOINING == current_state)
	{
		UTIL_TIMER_Start(&JoinBackoffTimer);
	}

	else if (STATE_CONNECTED == current_state)
	{
		fetch_device_id_and_session_keys();

		len = serialize_connect_to_lorawan_network_ack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_JOIN_FAILED == current_state)
	{
		len = serialize_connect_to_lorawan_network_nack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_FETCH_TIME == current_state)
	{
		LmHandlerErrorStatus_t res = LmhpClockSyncAppTimeReq();
	}

	else if (STATE_FETCH_TIME_SUCCESS == current_state)
	{
		len = serialize_get_time_ack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_FLUSH_UPLINK_DATA == current_state)
	{
		UTIL_TIMER_Start(&TxTimer);
	}

	else if (STATE_UPLINK_DATA_TX_ACK == current_state)
	{
		len = serialize_uplink_data_tx_ack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_UPLINK_DATA_TX_NACK == current_state)
	{
		len = serialize_uplink_data_tx_nack_packet((char *)debug_buffer, sizeof(debug_buffer));

		if (len > 0)
		{
			strcat((char *)debug_buffer, "\r\n");
			send_data_over_uart((char *)debug_buffer);
		}
	}

	else if (STATE_DOWNLINK_DATA_RECEIVED == current_state)
	{
//		send_data_over_uart((char *)downlink_data_rx_buffer);
		HAL_UART_Transmit(&huart2, downlink_data_rx_buffer, downlink_data_rx_length, downlink_data_rx_length * 20);
	}
}



void process_uart_rx_data(void)
{
	uint8_t res = 0;

	if (strstr((char *)uart_data_processing_buffer, "READY") != NULL)
	{
		current_state = STATE_READY;

		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
	}

	else if (strstr((char *)uart_data_processing_buffer, "CONNECT_TO_LORAWAN_NETWORK") != NULL)
	{
		res = parse_connect_to_lorawan_network_packet();

		if (1 == res)
		{
			SecureElementSetDevEui(dev_eui);
			SecureElementSetJoinEui(join_eui);

			SecureElementSetKey(APP_KEY, dev_app_key);
			SecureElementSetKey(NWK_KEY, dev_nwk_key);

			current_state = STATE_JOINING;

//			UTIL_TIMER_Start(&JoinBackoffTimer);

			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
		}
	}

	else if (strstr((char *)uart_data_processing_buffer, "GET_TIME") != NULL)
	{
		current_state = STATE_FETCH_TIME;

		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
	}

	else if (GENERALIZED_DATA_PAYLOAD_START_BYTE == uart_data_processing_buffer[0])
	{
		if (0x01 == uart_data_processing_buffer[1])
		{
			lorawan_tx_data_length = 0;
			lorawan_tx_data_length = parse_data_payload_packet(uart_data_rx_length);

			if (lorawan_tx_data_length > 0)
			{
				memset(lorawan_tx_buffer, 0, sizeof(lorawan_tx_buffer));
				memcpy(lorawan_tx_buffer, &uart_data_processing_buffer[3], lorawan_tx_data_length);

				current_state = STATE_FLUSH_UPLINK_DATA;

				UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
			}
		}
	}

//	else if (GENERALIZED_DATA_PAYLOAD_START_BYTE == downlink_data_rx_buffer[0])
//	{
//		if (DOWNLINK_PACKET_TYPE == downlink_data_rx_buffer[1])
//		{
//			current_state = STATE_DOWNLINK_DATA_RECEIVED;
//
//			UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
//		}
//	}
}


}
