/*
 * CANRxInterruptTask.c
 *
 *  Created on: Feb 10, 2024
 *      Author: MacKante
 */

#include "CANRxInterruptTask.h"
#include "CAN.h"
#include "CanParser.h"

extern osMessageQueueId_t canRxQueue;

void CANRxInterruptTask(void* arg)
{
	for(;;) {
		CANRxInterrupt();
	}
}

void CANRxInterrupt()
{
	uint16_t GPIO_Pin = 0;
	osMessageQueueGet(CANInterruptQueue, &GPIO_Pin, 0, osWaitForever);

	CanMsg msg = {0};


	if (osMutexWait(SPIMutexHandle, 0) == osOK)
	{
		if(GPIO_Pin == CAN_RX0BF_Pin)
		{
			receiveCANMessage(0, &(msg.StdId), &(msg.DLC), msg.Data);
		}
		else if (GPIO_Pin == CAN_RX1BF_Pin)
		{
			receiveCANMessage(1, &(msg.StdId), &(msg.DLC), msg.Data);
		}

		osMutexRelease(SPIMutexHandle);
	}

	osStatus_t x = osMessageQueuePut(canRxQueue, &msg, 0, osWaitForever);

}
