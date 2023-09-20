/*
 * debugTask.h
 *
 *  Created on: Sep. 20, 2023
 *      Author: Marcelo
 */
#ifndef DEBUG_TASK_H
#define DEBUG_TASK_H

#define PARAMTER_BUFFER_SIZE 4

#include "cmsis_os.h"
#include "stm32wlxx_hal.h"
#include "system_defines.h"
#include <string.h>

extern osMessageQueueId_t debugTaskQueue;

void debugTask(void *argument);

#endif /* DEBUG_TASK_H */
