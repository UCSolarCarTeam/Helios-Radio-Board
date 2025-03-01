#include "queueMessageTask.h"

void queueMessageTask2(void* arg) {
	CANMsg msg2 = {
			.DLC = 1,
			.ID = 0,
			.extendedID = 0xAFAFAFA,
			.data = {0xAA}
	};

	for(;;) {
		osMessageQueuePut(CANTxMessageQueue, &msg2, 0, osWaitForever);
		osDelay(3500);
	}
}
