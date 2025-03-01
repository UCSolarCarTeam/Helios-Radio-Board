#pragma once

#include "stm32wlxx.h"
#include "stm32wlxx_hal_uart.h"
#include "main.h"

#define LED_ON 1
#define LED_OFF 0
#define NUMBER_OF_ATTEMPTS 3

#define USART_TIMEOUT_PERIOD 5000

extern UART_HandleTypeDef hlpuart1;

void transmitMessage(uint8_t* payload, uint16_t bufferSize);
