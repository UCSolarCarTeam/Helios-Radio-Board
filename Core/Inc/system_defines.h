/*
 * system_defines.h
 *
 * constants used for system wide operations
 * 
 *  Created on: Feb. 28, 2023
 *      Author: Marcelo
 */

#define UART_RX_DATA_QUEUE_COUNT 16
#define UART_RX_DATA_SIZE 16
#define UART_TX_DATA_QUEUE_COUNT 16
#define UART_TX_DATA_SIZE 16

typedef struct uartData {
    uint8_t size;
    uint8_t command;
    uint8_t* data;
} uartData;

//how about have a command from the uart debug task to the radio drivers that provides 
//1)the address of the RadioConfig variable we want to change
//2) the value we would like to put it with 
//we should get away with a uint32_t + uint32_t size of queue data
//we would still need to implement a command variable and a malloc
//unless we plan to restart the radio module everytime we make a change (which fair)
//wait no, we still need a way to request debug data out (register values, command values, stats)
//yeah we need a command scheme, no way around it
