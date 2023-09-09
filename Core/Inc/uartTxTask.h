#include "stm32wlxx_hal_uart.h"

extern UART_HandleTypeDef huart2;

void uartTxTask(void *argument);