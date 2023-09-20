/*
 * uartRxTask.h
 *
 *  Created on: Sep. 08, 2023
 *      Author: Marcelo
 */
#ifndef UART_RX_TASK_H
#define UART_RX_TASK_H

#include "cmsis_os.h"
#include "stm32wlxx_hal.h"
#include "system_defines.h"
#include <string.h>

extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t uartRxQueue;
extern osMessageQueueId_t debugTaskQueue;

void uartRxTask(void *argument);

#endif /* UART_RX_TASK_H */
