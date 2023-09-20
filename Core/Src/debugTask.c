#include "debugTask.h"

/**
  * @brief  Function implementing the debug thread.
  * @param  argument: Not used
  * @retval None
*/
void debugTask(void *argument)
{
  uint8_t debugBuffer[UART_RX_BUFFER_SIZE];
  int parameterBuffer[PARAMTER_BUFFER_SIZE];
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(debugTaskQueue, debugBuffer, 0, 0);
    if(strncmp((char*)debugBuffer, "print: ", 7))
    {
        strToIntArray((char*)debugBuffer + 7, parameterBuffer);
        solarPrint("%d\n", parameterBuffer[0]);
    }
    if(strncmp((char*)debugBuffer, "print 3: ", 9))
    {
        strToIntArray((char*)debugBuffer + 9, parameterBuffer);
        for(int i = 0; i < 3; i++)
        solarPrint("%d\n", parameterBuffer[i]);
    }


  }
}
