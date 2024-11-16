#include "queueMessageTask.h"

void queueMessageTask1(void* arg) {
	CANMsg msg1 = {
			.DLC = 1,
			.ID = 0,
			.extendedID = 0x701,
			.data = {0xA5}
	};

	for(;;) {
		osMessageQueuePut(CANTxMessageQueue, &msg1, 0, osWaitForever);
		osDelay(2000);
	}
}
