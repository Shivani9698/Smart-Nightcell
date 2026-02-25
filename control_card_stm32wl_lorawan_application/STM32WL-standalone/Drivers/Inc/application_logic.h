/*
 * application_logic.h
 *
 *  Created on: Jan 2, 2026
 *      Author: Amogh MP
 */

#ifndef INC_APPLICATION_LOGIC_H_
#define INC_APPLICATION_LOGIC_H_


#ifdef __cplusplus
extern "C" {
#endif


#define JANUARY_1_2025_EPOCH_TIME						1735689601
#define JANUARY_1_2050_EPOCH_TIME						2524608001

#define GENERALIZED_DATA_PAYLOAD_START_BYTE				0xF9 // TODO: cross-check this has to be changed in stm32wl application as well

#define DATA_HEADER_LENGTH								3 // DATA_HEADER_LENGTH = DATA_PAYLOAD_START_BYTE + PACKET_TYPE + Actual PAYLOAD_LENGTH

#define UPLINK_PACKET_TYPE								01
#define DOWNLINK_PACKET_TYPE							02

#define TANK_DATA_PAYLOAD_START_BYTE					0xA9
#define TANK_DATA_PAYLOAD_LENGTH						23



typedef enum {
    STATE_IDLE,
	STATE_READY,
    STATE_JOINING,
    STATE_CONNECTED,
    STATE_JOIN_FAILED,
	STATE_FETCH_TIME,
	STATE_FETCH_TIME_SUCCESS,
	STATE_FETCH_TIME_FAIL,
	STATE_FLUSH_UPLINK_DATA,
	STATE_UPLINK_DATA_TX_ACK,
	STATE_UPLINK_DATA_TX_NACK,
	STATE_DOWNLINK_DATA_RECEIVED
} app_state_t;

extern app_state_t current_state;



void run_state_machine(void);
void process_uart_rx_data(void);



#ifdef __cplusplus
}
#endif


#endif /* INC_APPLICATION_LOGIC_H_ */
