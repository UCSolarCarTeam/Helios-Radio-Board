/*
 * CANTransmitTask.c
 *
 *  Created on: Feb 10, 2024
 *      Author: MacKante
 */

#include "CANTxGatekeeperTask.h"

void CANTxGatekeeperTask(void* arg) {

	// This block is to initialize a message queue of CAN Messages to send for testing
	#if 0
	// Define dummy messages here...
	CANMsg msg1 = {
		.DLC = 1,
		.ID = 0,
		.extendedID = 0xCCCCCCC,
		.data = {0xFF}
	};

	CANMsg msg2 = {
			.DLC = 2,
			.ID = 0,
			.extendedID = 0xA5A5A5A,
			.data = {0xFF, 0xFF}
	};

	CANMsg msg3 = {
			.DLC = 3,
			.ID = 0,
			.extendedID = 0xCFCFCFC,
			.data = {0xFF, 0xFF, 0xFF}
	};

	// Add test messages to queue here...
	osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
	osMessageQueuePut(CANTxMessageQueue, &msg2, 0, osWaitForever);
	osMessageQueuePut(CANTxMessageQueue, &msg3, 0, osWaitForever);
	#endif

    CANMsg newMsg;

    for (;;) {
//    	osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
        CANTxGatekeeper(&newMsg);
        osDelay(100);
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
		if ((msg->extendedID == 0) && (msg->ID != 0))
		{
			sendCANMessage(msg);
		}
		else
		{
			sendExtendedCANMessage(msg);
		}

		osMutexRelease(SPIMutexHandle);
	}

}
