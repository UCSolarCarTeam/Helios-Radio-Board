/*
 * system_defines.c
 *
 *  Created on: Sep 14, 2023
 *      Author: Marcelo Li
 */

#include "system_defines.h"

void *solarMalloc(size_t xWantedSize)
{
    void *ret = pvPortMalloc(xWantedSize);

    return ret;
}

void solarPrint(const char* str, ...) {
    //Try to take the VA list mutex
    if(osMutexWait(vaListMutexHandle, 0) == osOK)
    {
        // If we have a message, and can use VA list, extract the string into a new buffer, and null terminate it
        uint8_t str_buffer[DEBUG_PRINT_MAX_SIZE] = {};
        va_list argument_list;
        va_start(argument_list, str);
        int16_t buflen = vsnprintf((char *)(str_buffer), sizeof(str_buffer) - 1, str, argument_list);
        va_end(argument_list);
        if (buflen > 0) {
            str_buffer[buflen] = '\0';
        }

        // Release the VA List Mutex
        osMutexRelease(vaListMutexHandle);

        //Generate a command
        UartTxData uartTxData = {.size = buflen};

        //Copy data into the command
        uartTxData.data = solarMalloc(buflen);
        memcpy(uartTxData.data, str_buffer, buflen); // null character is ignored, since terminal adds it anyways

        //Send this packet off to the UART Task
        osMessageQueuePut(uartTxQueue, &uartTxData, 0, 0);
    }
    else
    {
        //TODO: add error functionality reporting functionality
    }
}
