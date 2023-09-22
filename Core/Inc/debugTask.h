/*
 * debugTask.h
 *
 *  Created on: Sep. 20, 2023
 *      Author: Marcelo
 */
#ifndef DEBUG_TASK_H
#define DEBUG_TASK_H

#define PARAMATER_BUFFER_SIZE 6

#include "cmsis_os.h"
#include "stm32wlxx_hal.h"
#include "system_defines.h"
#include <string.h>

extern osMessageQueueId_t debugTaskQueue;
extern osMessageQueueId_t radioCommandQueue;
extern osMessageQueueId_t toggleCommandQueue;

void DebugTask(void *argument);

#endif /* DEBUG_TASK_H */
