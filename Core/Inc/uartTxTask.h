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

extern UART_HandleTypeDef hlpuart1;
extern osMessageQueueId_t uartTxQueue;

void UartTxTask(void *argument);

#endif /* UART_TX_TASK_H */
