/*
 * semaphore_t.cpp
 *
 *  Created on: Jul 22, 2025
 *      Author: Amogh MP
 */


#include "semaphore_t.h"


semaphore_t wifi_isr_semaphore((char*) "wifi_isr_semaphore");
semaphore_t cellular_isr_semaphore((char*) "cellular_isr_semaphore");
semaphore_t downlink_semaphore((char*) "downlink_semaphore");
semaphore_t cellular_manual_data_process_semaphore((char*) "cellular_manual_data_process_semaphore");
semaphore_t radar_sensor_data_processing_sempahore((char*)"radar_sensor_data_processing_sempahore");
semaphore_t lorawan_isr_semaphore((char*) "lorawan_isr_semaphore");



semaphore_t::semaphore_t(char * semaphore_name)
{
	osSemaphoreDef(semaphore_name);
	semaphore = osSemaphoreCreate(osSemaphore(semaphore_name), 1);

	osSemaphoreWait(semaphore, 0);
}

semaphore_t::~semaphore_t()
{

}

uint8_t semaphore_t::acquire(uint32_t timeout)
{
	if(osSemaphoreWait(semaphore, timeout) == osOK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t semaphore_t::release(void)
{
	if (osSemaphoreRelease(semaphore) == osOK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

