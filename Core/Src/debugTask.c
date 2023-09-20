#include "debugTask.h"

/**
  * @brief  Function implementing the debug thread.
  * @param  argument: Not used
  * @retval None
*/
void debugTask(void *argument)
{
  uint8_t debugBuffer[UART_RX_BUFFER_SIZE];
  int parameterBuffer[PARAMATER_BUFFER_SIZE];
  uint8_t parameterBufferSize = 0;
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(debugTaskQueue, debugBuffer, 0, 0);
    if(strncmp((char*)debugBuffer, "print: ", 7))
    {
        strToIntArray((char*)debugBuffer + 7, parameterBuffer, &parameterBufferSize);
        solarPrint("%d\n", parameterBuffer[0]);
    }
    else if(strncmp((char*)debugBuffer, "print3: ", 8))
    {
        strToIntArray((char*)debugBuffer + 8, parameterBuffer, &parameterBufferSize);
        for(int i = 0; i < 3; i++)
            solarPrint("%d\n", parameterBuffer[i]);
    }
    else if(strncmp((char*)debugBuffer, "radioSetCommand: ", 17)) //readio set command parameters are [command enum, byte1, byte 2, etc]
    {
        strToIntArray((char*)debugBuffer + 17, parameterBuffer, &parameterBufferSize);
        RadioCommand radioCommand = {0};
        radioCommand.address = parameterBuffer[0];
        radioCommand.size = parameterBufferSize - 1;
        radioCommand.command = SET_COMMAND;
        radioCommand.data = solarMalloc(parameterBufferSize - 1);
        memcpy(radioCommand.data, parameterBuffer + 1, parameterBufferSize - 1);
        osMessageQueuePut(radioCommandQueue, &radioCommand, 0, 0);
    }
    else if(strncmp((char*)debugBuffer, "radioGetCommand: ", 17)) //readio set command parameters are [command enum,size]
    {
        strToIntArray((char*)debugBuffer + 17, parameterBuffer, &parameterBufferSize);
        RadioCommand radioCommand = {0};
        radioCommand.address = parameterBuffer[0];
        radioCommand.size = parameterBuffer[1];
        radioCommand.command = GET_COMMAND;
        osMessageQueuePut(radioCommandQueue, &radioCommand, 0, 0);
    }
  }
}
