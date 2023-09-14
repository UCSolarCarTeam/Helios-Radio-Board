/*
 * system_defines.h
 *
 * constants used for system wide operations
 * 
 *  Created on: Sep 08, 2023
 *      Author: Marcelo
 */
#ifndef SYSTEM_DEFINES_H
#define SYSTEM_DEFINES_H

#include "FreeRTOS.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <stdarg.h>

#define DEBUG_TAKE_MAX_TIME_MS 10
#define DEBUG_PRINT_MAX_SIZE 256

#define UART_RX_DATA_QUEUE_COUNT 16
#define UART_TX_DATA_QUEUE_COUNT 16

extern osMessageQueueId_t uartTxQueue;
extern osMutexId_t vaListMutexHandle;

typedef enum RadioCommandType {
    SET_COMMAND = 0,
    GET_COMMAND = 1,
    WRITE_BUFFER = 2,
    READ_BUFFER = 3,
    WRITE_REGISTER_COMMAND = 4,
    READ_REGISTER_COMMAND = 5
} RadioCommandType;

typedef struct RadioCommand {
    uint16_t size;
    RadioCommandType command;
    uint8_t address;
    uint8_t* data;
} RadioCommand;

typedef struct UartTxData {
    uint8_t size;
    uint8_t* data;
} UartTxData;

void *solarMalloc(size_t xWantedSize);

void solarPrint(const char* str, ...);
//how about have a command from the uart debug task to the radio drivers that provides 
//1)the address of the RadioConfig variable we want to change
//2) the value we would like to put it with 
//we should get away with a uint32_t + uint32_t size of queue data
//we would still need to implement a command variable and a malloc
//unless we plan to restart the radio module everytime we make a change (which fair)
//wait no, we still need a way to request debug data out (register values, command values, stats)
//yeah we need a command scheme, no way around it
#endif /* SYSTEM_DEFINES_H */
