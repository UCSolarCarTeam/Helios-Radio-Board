/*
 * CANTransmitTask.c
 *
 *  Created on: Feb 10, 2024
 *      Author: MacKante
 */

#include "CANTxGatekeeperTask.h"

void CANTxGatekeeperTask(void* arg) {

	// This block is to initialize a message queue of CAN Messages to send for testing
	# if CANTestSetup
	// Define dummy messages here...
	CANMsg msg1 = {
		.DLC = 1,
		.ID = NULL,
		.extendedID = 0xCCCCCCC,
		.data = {0xFF}
	};

	CANMsg msg2 = {
			.DLC = 1,
			.ID = NULL,
			.extendedID = 0xAAAAAAA,
			.data = {0xFF}
	};

	// Add test messages to queue here...
	osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
	osMessageQueuePut(CANTxMessageQueue, &msg2, 0, osWaitForever);
	osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
	osMessageQueuePut(CANTxMessageQueue, &msg2, 0, osWaitForever);
	# endif

    CANMsg newMsg;

    for (;;) {
        CANTxGatekeeper(&newMsg);
    }
}

void CANTxGatekeeper(CANMsg *msg) {
	// Acquire message to send from queue
	osMessageQueueGet(CANTxMessageQueue, msg, NULL, osWaitForever);

	// Wait for mutex
	if ( osMutexWait(SPIMutexHandle, 0) == osOK )
	{
		// check if CAN message is standard/extended
		// if extendedID == 0, then message is standard
		if (msg->extendedID == 0)
		{
			sendCANMessage(msg);
		}
		else
		{
			sendExtendedCANMessage(msg);
		}
		osDelay(1000);
		osMutexRelease(SPIMutexHandle);
	}

}
