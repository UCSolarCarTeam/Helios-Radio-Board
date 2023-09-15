#include "uartTxTask.h"

/**
  * @brief  Function implementing the uartTx thread.
  * @param  argument: Not used
  * @retval None
*/
void uartTxTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    UartTxData uartTxData = {0};
    osMessageQueueGet(uartTxQueue, &uartTxData, 0, 0);
    HAL_StatusTypeDef ret = 0;
    if (HAL_UART_Transmit(UART_HANDLER, uartTxData.data, uartTxData.size, 0) != HAL_OK){
      //add error handler
    };
  }
}