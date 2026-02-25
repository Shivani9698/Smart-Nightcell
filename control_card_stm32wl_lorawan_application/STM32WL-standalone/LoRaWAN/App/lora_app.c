
#include "platform.h"
#include "Region.h" /* Needed for LORAWAN_DEFAULT_DATA_RATE */
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "utilities_def.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "stm32_lpm.h"
#include "adc_if.h"
#include "sys_conf.h"
#include "radio.h"
#include "send_raw_lora.h"
#include "stdlib.h"

#define LINE_SIZE 34
#include  "General_Setup.h"



#include "gpio.h"
#include "application_logic.h"
#include "flash_if.h"



extern uint8_t power_up;



extern uint8_t dev_address[4];

extern uint8_t dev_eui[8];
extern uint8_t join_eui[8];

extern uint8_t dev_app_key[16];
extern uint8_t dev_nwk_key[16];
extern uint8_t dev_app_session_key[16];
extern uint8_t dev_nwk_session_key[16];



static void byteReception(uint8_t *PData, uint16_t Size, uint8_t Error);


static uint8_t rxBuff[RX_BUFF_SIZE];
uint8_t isRxConfirmed;
uint32_t LoRaMode = 0;
uint8_t size_txBUFFER = 0;
uint8_t txBUFFER[100];
uint8_t isTriggered = 0;



static uint8_t rx_byte;
static uint32_t rx_counter = 0;
uint16_t uart_data_rx_length = 0;
uint16_t downlink_data_rx_length = 0;


uint8_t link_check_test_buffer[1] = {0};
LmHandlerAppData_t link_check_payload = {.Buffer = link_check_test_buffer, .BufferSize = 0, .Port = 1};
UTIL_TIMER_Time_t nextTxIn = 0;


uint8_t uart_data_rx_buffer[RX_BUFF_SIZE] = {0};
uint8_t uart_data_processing_buffer[RX_BUFF_SIZE] = {0};
uint8_t debug_buffer[RX_BUFF_SIZE] = {0};

uint8_t lorawan_tx_buffer[RX_BUFF_SIZE] = {0};
uint16_t lorawan_tx_data_length = 0;


uint8_t downlink_data_rx_buffer[RX_BUFF_SIZE] = {0};


typedef enum TxEventType_e
{
	TX_ON_TIMER,
	TX_ON_EVENT
} TxEventType_t;

static void SendTxData(void);
static void OnTxTimerEvent(void *context);
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);
static void OnTxData(LmHandlerTxParams_t *params);
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);
static void OnMacProcessNotify(void);

static void OnTxTimerLedEvent(void *context);
static void OnRxTimerLedEvent(void *context);
static void OnJoinTimerLedEvent(void *context);



void OnJoinBackoffTimerEvent(void *context);

static void OnIWDGRefreshTimerEvent(void *context);

static void MX_BP_IT_Init(void);

static void while_loop(void);


static void OnSysTimeUpdate(void *context);


static void run_state_machine_timer_event(void *context);



ActivationType_t ActivationType = LORAWAN_DEFAULT_ACTIVATION_TYPE;

static LmHandlerCallbacks_t LmHandlerCallbacks =
{
		.GetBatteryLevel =           GetBatteryLevel,
		.GetTemperature =            GetTemperatureLevel,
		.GetUniqueId =               GetUniqueId,
		.GetDevAddr =                GetDevAddr,
		.OnMacProcess =              OnMacProcessNotify,
		.OnJoinRequest =             OnJoinRequest,
		.OnTxData =                  OnTxData,
		.OnRxData =                  OnRxData,

		.OnSysTimeUpdate = 			OnSysTimeUpdate, // TODO: AMP 04-01-2026
};

static LmHandlerParams_t LmHandlerParams =
{
		.ActiveRegion =             ACTIVE_REGION,
		.DefaultClass =             LORAWAN_DEFAULT_CLASS,
		.AdrEnable =                LORAWAN_ADR_STATE,
		.TxDatarate =               LORAWAN_DEFAULT_DATA_RATE,
		.PingPeriodicity =          LORAWAN_DEFAULT_PING_SLOT_PERIODICITY
};


UTIL_TIMER_Object_t TxTimer;

static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };
static uint8_t AppLedStateOn = RESET;
static UTIL_TIMER_Object_t TxLedTimer;
static UTIL_TIMER_Object_t RxLedTimer;
static UTIL_TIMER_Object_t JoinLedTimer;


UTIL_TIMER_Object_t JoinBackoffTimer;
static UTIL_TIMER_Object_t IWDGRefreshTimer;


UTIL_TIMER_Object_t run_state_machine_timer;



void fetch_device_id_and_session_keys(void)
{
	Key_t *keyItem;

	SecureElementGetKeyByID(NWK_KEY, &keyItem);
	memcpy(dev_nwk_key, keyItem->KeyValue, 16);


	SecureElementGetKeyByID(APP_KEY, &keyItem);
	memcpy(dev_app_key, keyItem->KeyValue, 16);


	SecureElementGetKeyByID(NWK_S_KEY, &keyItem);
	memcpy(dev_nwk_session_key, keyItem->KeyValue, 16);


	SecureElementGetKeyByID(APP_S_KEY, &keyItem);
	memcpy(dev_app_session_key, keyItem->KeyValue, 16);
}



void LoRaWAN_Init(void)
{
	UTIL_TIMER_Create(&TxLedTimer, 500, UTIL_TIMER_ONESHOT, OnTxTimerLedEvent, NULL);
	UTIL_TIMER_Create(&RxLedTimer, 500, UTIL_TIMER_ONESHOT, OnRxTimerLedEvent, NULL);
	UTIL_TIMER_Create(&JoinLedTimer, 500, UTIL_TIMER_PERIODIC, OnJoinTimerLedEvent, NULL);


	if (FLASH_IF_Init(NULL) != FLASH_IF_OK)
	{
		Error_Handler();
	}

	UTIL_TIMER_Create(&JoinBackoffTimer, 1000, UTIL_TIMER_ONESHOT, OnJoinBackoffTimerEvent, NULL); // TODO: AMP 04/01/2026
	UTIL_TIMER_Create(&IWDGRefreshTimer, 25 * 1000, UTIL_TIMER_PERIODIC, OnIWDGRefreshTimerEvent, NULL);


	UTIL_TIMER_Create(&run_state_machine_timer, 10 * 1000, UTIL_TIMER_PERIODIC, run_state_machine_timer_event, NULL);
	UTIL_TIMER_Create(&TxTimer, 1000, UTIL_TIMER_ONESHOT, OnTxTimerEvent, NULL);


	UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LmHandlerProcess), UTIL_SEQ_RFU, LmHandlerProcess);
	UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), UTIL_SEQ_RFU, SendTxData);


	UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Parse_UART_RX_Data), UTIL_SEQ_RFU, process_uart_rx_data);
	UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Run_State_Machine), UTIL_SEQ_RFU, run_state_machine);

	LoraInfo_Init();

	/* Initialize all callbacks */
	LmHandlerInit(&LmHandlerCallbacks);

	/* Print LoRaWAN information : DevEUI & Devaddr when ABP - DevEUI & AppEUI-JoinEUI for OTAA */
	/* Print Session Keys for ABP - Print Root Key for OTAA :LmHandlerConfigure() > LoRaMacInitialization() > SecureElementInit() > PrintKey() */
	LmHandlerConfigure(&LmHandlerParams);

	/* Let all print out terminated. Otherwise logs are affected.*/
	HAL_Delay(500);

	/* Join Red LED starts blinking */
	UTIL_TIMER_Start(&JoinLedTimer);

	UTIL_TIMER_Start(&run_state_machine_timer);

	UTIL_ADV_TRACE_StartRxProcess(byteReception); // TODO: AMP 03/01/2026
}


static void byteReception(uint8_t *PData, uint16_t Size, uint8_t Error)
{
	rx_byte = *PData;

	if (rx_counter >= (RX_BUFF_SIZE - 4))
	{
		rx_counter = 0;
	}

	else if (rx_byte == '\n')
	{
		uart_data_rx_buffer[rx_counter++] = '\n';

		if (rx_counter >= 12)
		{
			if (GENERALIZED_DATA_PAYLOAD_START_BYTE == uart_data_rx_buffer[0])
			{
				if (rx_counter >= (uart_data_rx_buffer[2] + DATA_HEADER_LENGTH))
				{
					memset(uart_data_processing_buffer, 0, sizeof(uart_data_processing_buffer));
					memcpy(uart_data_processing_buffer, uart_data_rx_buffer, rx_counter);

					uart_data_rx_length = rx_counter;
					rx_counter = 0;

					UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Parse_UART_RX_Data), CFG_SEQ_Prio_0);
				}
			}
			else
			{
				memset(uart_data_processing_buffer, 0, sizeof(uart_data_processing_buffer));
				memcpy(uart_data_processing_buffer, uart_data_rx_buffer, rx_counter);

				uart_data_rx_length = rx_counter;
				rx_counter = 0;

				UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Parse_UART_RX_Data), CFG_SEQ_Prio_0);
			}
		}

		else
		{
			rx_counter = 0;
		}
	}
	else
	{
		uart_data_rx_buffer[rx_counter++] = rx_byte;
	}
}



uint8_t flush_data_onto_cloud(void)
{
	uint8_t res = 0;
	UTIL_TIMER_Time_t nextTxIn = 0;

	LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;

//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//	HAL_Delay(250);
//	turn_radio_switch(RF_SWITCH_ON);



	if ((JoinLedTimer.IsRunning) && (LmHandlerJoinStatus() == LORAMAC_HANDLER_SET))
	{
		res = 0;
	}

//	status = LmHandlerSend(&AppData, LORAWAN_DEFAULT_CONFIRMED_MSG_STATE, &nextTxIn, false);
	status = LmHandlerSend(&AppData, LORAMAC_HANDLER_CONFIRMED_MSG, &nextTxIn, false);

	if (LORAMAC_HANDLER_SUCCESS == status)
	{
		res = 1;
	}

//	turn_radio_switch(RF_SWITCH_OFF);
//	HAL_Delay(250);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//	HAL_Delay(250);

	return res;
}


static void SendTxData(void)
{
	/* USER CODE BEGIN SendTxData_1 */
	UTIL_TIMER_Time_t nextTxIn = 0;
	LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;

	AppData.Port = LORAWAN_USER_APP_PORT;
	AppData.BufferSize = lorawan_tx_data_length;

	memset(AppData.Buffer, 0, sizeof(AppData.Buffer));
	memcpy(AppData.Buffer, lorawan_tx_buffer, lorawan_tx_data_length);


//	sprintf((char *)debug_buffer, "lorawan tx data: ");
//	send_data_over_uart((char *)debug_buffer);
//
//	for (uint8_t i = 0 ; i < lorawan_tx_data_length ; i++)
//	{
//		sprintf((char *)debug_buffer, "%02X", AppData.Buffer[i]);
//		send_data_over_uart((char *)debug_buffer);
//	}
//	sprintf((char *)debug_buffer, "\r\n");
//	send_data_over_uart((char *)debug_buffer);



	if (LmHandlerIsBusy() == false)
	{
		flush_data_onto_cloud();
	}

	else
	{
//		sprintf(debug_buffer, "Message Send Fail\r\n");
//		send_data_over_uart(debug_buffer);
	}

	/* USER CODE END SendTxData_1 */
}




static void OnTxData(LmHandlerTxParams_t *params)
{
	uint8_t res = 0;

	if ((params != NULL))
	{
		if (params->IsMcpsConfirm != 0)
		{
			if(params->AppData.Port != 0)
			{
				if (1 == params->AckReceived)
				{
					current_state = STATE_UPLINK_DATA_TX_ACK;

//					sprintf(debug_buffer, "ACK Received\r\n");
//					send_data_over_uart(debug_buffer);

				    res = 1;
				}
			}
		}
	}

	if (1 != res)
	{
		current_state = STATE_UPLINK_DATA_TX_NACK;

//		sprintf(debug_buffer, "No ACK Received\r\n");
//		send_data_over_uart(debug_buffer);
	}

	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
}


static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
	if ((appData != NULL) && (params != NULL))
	{
		if(appData->Port == 0)
		{

		}

		else
		{

		}

		switch (appData->Port)
		{
			case LORAWAN_SWITCH_CLASS_PORT:

				if (appData->BufferSize == 1)
				{
					switch (appData->Buffer[0])
					{
						case 0:
							{
								LmHandlerRequestClass(CLASS_A); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class A\r\n");
								break;
							}
						case 1:
							{
								LmHandlerRequestClass(CLASS_B); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class B\r\n");
								break;
							}
						case 2:
							{
								LmHandlerRequestClass(CLASS_C); APP_LOG(TS_OFF, VLEVEL_L, "Switch to class C\r\n");
								break;
							}
						default:
							break;
					}
				}
				break;


			default:
//				if ((appData->BufferSize >= 1) && (appData->BufferSize < 50))
				if ((appData->BufferSize >= 1) && (appData->BufferSize < 1024))
				{
					memset(downlink_data_rx_buffer, 0 , sizeof(downlink_data_rx_buffer));
					memcpy(downlink_data_rx_buffer, appData->Buffer, appData->BufferSize);

					downlink_data_rx_length = appData->BufferSize;

					current_state = STATE_DOWNLINK_DATA_RECEIVED;

//					UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Parse_UART_RX_Data), CFG_SEQ_Prio_0);
					UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
				}

				break;
		}
	}
}


//uint8_t device_join_status = 0;


static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
	HAL_IWDG_Refresh(&hiwdg);

	if (joinParams != NULL)
	{
		if (joinParams->Status == LORAMAC_HANDLER_SUCCESS)
		{
			UTIL_TIMER_Stop(&JoinLedTimer);
			UTIL_TIMER_Stop(&JoinBackoffTimer);

			if (joinParams->Mode == ACTIVATION_TYPE_ABP)
			{
//				sprintf(debug_buffer, "ABP ======================\r\n");
//				send_data_over_uart(debug_buffer);

				current_state = STATE_CONNECTED;

			}
			else
			{
//				sprintf(debug_buffer, "OTAA ======================\r\n");
//				send_data_over_uart(debug_buffer);

				current_state = STATE_CONNECTED;

				UTIL_TIMER_Start(&IWDGRefreshTimer); // TODO: check this if the device joins late to the network the iwdg kicks and reboots the device.
			}
		}
		else
		{
			current_state = STATE_JOIN_FAILED;
		}

		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
	}
}


void OnJoinBackoffTimerEvent(void *context)
{
	HAL_IWDG_Refresh(&hiwdg);

	LmHandlerJoin(ActivationType);
}


static void OnMacProcessNotify(void)
{
	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LmHandlerProcess), CFG_SEQ_Prio_0);
}


static void OnTxTimerEvent(void *context)
{
	HAL_IWDG_Refresh(&hiwdg);

	UTIL_TIMER_Time_t nextTxIn = 0;
	LmHandlerErrorStatus_t status = LORAMAC_HANDLER_ERROR;

	if (LmHandlerIsBusy() == false)
	{
		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaSendOnTxTimerOrButtonEvent), CFG_SEQ_Prio_0);

	}
	else
	{
		sprintf(debug_buffer, "TxTimer uplink BUSY\r\n"); // TODO: don't remove this print as I'm using this print statement in host application
		send_data_over_uart(debug_buffer);
	}
}

static void OnTxTimerLedEvent(void *context)
{
	BSP_LED_Off(LED_GREEN) ;
}

static void OnRxTimerLedEvent(void *context)
{
	BSP_LED_Off(LED_BLUE) ;
}

static void OnJoinTimerLedEvent(void *context)
{
	BSP_LED_Toggle(LED_RED) ;
}


static void OnIWDGRefreshTimerEvent(void *context)
{
	HAL_IWDG_Refresh(&hiwdg);
}


void OnSysTimeUpdate(void *context) // TODO: AMP 29-08-2025
{
	HAL_IWDG_Refresh(&hiwdg);

	SysTime_t sysTime = SysTimeGet();

	if ((sysTime.Seconds > JANUARY_1_2025_EPOCH_TIME) && (sysTime.Seconds < JANUARY_1_2050_EPOCH_TIME))
	{
		sprintf(debug_buffer, "epoch_time: %d\r\n", sysTime.Seconds);
		send_data_over_uart(debug_buffer);

		current_state = STATE_FETCH_TIME_SUCCESS;

		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
	}
}


void run_state_machine_timer_event(void *context)
{
	UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_Run_State_Machine), CFG_SEQ_Prio_0);
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
//	switch (GPIO_Pin)
//	{
//	case USER_SW_Pin:
//		sw_pressed = 1;
//		__HAL_GPIO_EXTI_CLEAR_IT(USER_SW_Pin);
//		UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_CheckUserSwitchStatus), CFG_SEQ_Prio_0);
//		break;
//	default:
//		break;
//	}
}
