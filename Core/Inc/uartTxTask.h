/*
 * uartTxTask.h
 *
 *  Created on: Sep. 08, 2023
 *      Author: Marcelo
 */
#ifndef UART_TX_TASK_H
#define UART_TX_TASK_H

#include "cmsis_os.h"
#include "stm32wlxx_hal.h"
#include "system_defines.h"

#define UART_HANDLER &huart2

extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t uartTxQueue;

void uartTxTask(void *argument);

#endif /* UART_TX_TASK_H */
