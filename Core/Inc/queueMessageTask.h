#pragma once

#include "cmsis_os.h"
#include "CAN.h"

void queueMessageTask1(void* arg);
void queueMessageTask2(void* arg);

extern osMessageQueueId_t CANInterruptQueue;
