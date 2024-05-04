#include "queueMessageTask.h"

void queueMessageTask1(void* arg) {
	CANMsg msg1 = {
			.DLC = 1,
			.ID = 0,
			.extendedID = 0xCFCFCFC,
			.data = {0xCC}
	};

	for(;;) {
		osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
		osDelay(2000);
	}
}
