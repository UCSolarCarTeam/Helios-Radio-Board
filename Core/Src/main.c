/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CAN.h"
#include "radio_drivers.h"
#include "system_defines.h"
#include "uartRxTask.h"
#include "uartTxTask.h"
#include "debugTask.h"

#include "CANRxInterruptTask.h"
#include "CANTxGatekeeperTask.h"
#include "queueMessageTask.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CAN_INTERRUPT_QUEUE_COUNT 5 // anticipate 2 sudden messages + 1 for extra room
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef hlpuart1;

SPI_HandleTypeDef hspi1;

SUBGHZ_HandleTypeDef hsubghz;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* USER CODE BEGIN PV */
/* Definitions for RadioTask */
osThreadId_t radioTaskHandle;
const osThreadAttr_t radioTask_attributes = {
  .name = "RadioTask",
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for ToggleTask */
osThreadId_t toggleTaskHandle;
const osThreadAttr_t toggleTask_attributes = {
  .name = "ToggleTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for uartRxTask */
osThreadId_t uartRxTaskHandle;
const osThreadAttr_t uartRxTask_attributes = {
  .name = "uartRxTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for uartTxTask */
osThreadId_t uartTxTaskHandle;
const osThreadAttr_t uartTxTask_attributes = {
  .name = "uartTxTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for debugTask */
osThreadId_t debugTaskHandle;
const osThreadAttr_t debugTask_attributes = {
  .name = "debugTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for SPIMutex */
osMutexId_t SPIMutexHandle;
const osMutexAttr_t SPIMutex_attributes = {
  .name = "SPIMutex"
};

/* Definitions for VA_LIST mutex */
osMutexId_t vaListMutexHandle;
const osMutexAttr_t vaListMutex_attributes = {
  .name = "vaListMutexHandle"
};

osMutexId_t CANTxGateKeeperTaskHandle;
const osThreadAttr_t CANTxGateKeeperTask_attributes = {
  .name = "CANTxGateKeeperTask",
  .priority = (osPriority) osPriorityNormal1,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

/* Definitions for CANRXInterruptTask */
osThreadId_t CANRXInterruptTaskHandle;
const osThreadAttr_t CANRXInterruptTask_attributes = {
  .name = "CANRXInterruptTask",
  .priority = (osPriority_t) osPriorityNormal1,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

osThreadId_t queueMessageTask1Handle;
const osThreadAttr_t queueMessageTask1_attributes = {
  .name = "queueMessageTask1",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

osThreadId_t queueMessageTask2Handle;
const osThreadAttr_t queueMessageTask2_attributes = {
  .name = "queueMessageTask2",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = DEFAULT_TASK_STACK_SIZE
};

osMessageQueueId_t CANInterruptQueue;
osMessageQueueId_t CANTxMessageQueue;

osMessageQueueId_t radioCommandQueue;
osMessageQueueId_t uartTxQueue;
osMessageQueueId_t uartRxQueue;
osMessageQueueId_t radioDataQueue;
osMessageQueueId_t debugTaskQueue;
osMessageQueueId_t toggleCommandQueue;
#if RX
osMessageQueueId_t RadioReceiveInterruptQueue;
#endif

#if 0
/* Semaphore for something */
osSemaphoreId_t radioSemaphoreHandle;
const osSemaphoreAttr_t radioSemaphore_attributes = {
  .name = "radioSemaphoreHandle"
};
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_SUBGHZ_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */
void ToggleTask(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_LPUART1_UART_Init();
  MX_SPI1_Init();
  MX_SUBGHZ_Init();
  /* USER CODE BEGIN 2 */
  RadioInit();
  ConfigureCANSPI();
  HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  SPIMutexHandle = osMutexNew(&SPIMutex_attributes); //unused
  vaListMutexHandle = osMutexNew(&vaListMutex_attributes);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */


  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  CANInterruptQueue = osMessageQueueNew(CAN_INTERRUPT_QUEUE_COUNT, sizeof(uint16_t), NULL);
  CANTxMessageQueue = osMessageQueueNew(8, sizeof(CANMsg), NULL);

  radioDataQueue = osMessageQueueNew(RADIO_DATA_QUEUE_COUNT, sizeof(RadioData), NULL);
  radioCommandQueue = osMessageQueueNew(UART_RX_DATA_QUEUE_COUNT, sizeof(RadioCommand), NULL);
  uartTxQueue = osMessageQueueNew(UART_TX_DATA_QUEUE_COUNT, sizeof(UartTxData), NULL);
  uartRxQueue = osMessageQueueNew(UART_RX_DATA_QUEUE_COUNT, sizeof(uint8_t), NULL);
  debugTaskQueue = osMessageQueueNew(DEBUG_QUEUE_COUNT, UART_RX_BUFFER_SIZE, NULL);
  toggleCommandQueue = osMessageQueueNew(TOGGLE_QUEUE_COUNT, sizeof(uint8_t), NULL);
#if RX
  RadioReceiveInterruptQueue = osMessageQueueNew(RADIO_RECEIVE_INTERRUPT_QUEUE_COUNT, sizeof(uint8_t), NULL);
#endif
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  // defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  /* creation of radioTask */
  // radioTaskHandle = osThreadNew(RadioTask, NULL, &radioTask_attributes);

  /* creation of toggleTask */
  // toggleTaskHandle = osThreadNew(ToggleTask, NULL, &toggleTask_attributes);

  /* creation of uartRxTask*/
  // uartRxTaskHandle = osThreadNew(UartRxTask, NULL, &uartRxTask_attributes);

  /* creation of uartTxTask*/
  // uartTxTaskHandle = osThreadNew(UartTxTask, NULL, &uartTxTask_attributes);

  /* creation of debugTask*/
  // debugTaskHandle = osThreadNew(DebugTask, NULL, &debugTask_attributes);

  
  CANRXInterruptTaskHandle = osThreadNew(CANRxInterruptTask, NULL, &CANRXInterruptTask_attributes);

  /* creation of CANTxGatekeeperTask*/
  CANTxGateKeeperTaskHandle = osThreadNew(CANTxGatekeeperTask, NULL, &CANTxGateKeeperTask_attributes);

  /* creation of queue CAN message tasks */
  queueMessageTask1Handle = osThreadNew(queueMessageTask1, NULL, &queueMessageTask1_attributes);
  queueMessageTask2Handle = osThreadNew(queueMessageTask2, NULL, &queueMessageTask2_attributes);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 18;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV6;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 115200;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_RTS;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV4;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SUBGHZ Initialization Function
  * @param None
  * @retval None
  */
static void MX_SUBGHZ_Init(void)
{

  /* USER CODE BEGIN SUBGHZ_Init 0 */

  /* USER CODE END SUBGHZ_Init 0 */

  /* USER CODE BEGIN SUBGHZ_Init 1 */
  //CLK = 32MHz, and acoridng to SUBGHZ module datasheet max SPI CLK = 16MHz, using 8MHz to be safe
  /* USER CODE END SUBGHZ_Init 1 */
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_4;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SUBGHZ_Init 2 */

  /* USER CODE END SUBGHZ_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CAN_CS_Pin|FLASH_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED_RED_Pin|LED_GREEN_Pin|LED_BLUE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RF_CTRL1_GPIO_Port, RF_CTRL1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : CAN_CS_Pin FLASH_CS_Pin RF_CTRL1_Pin */
  GPIO_InitStruct.Pin = CAN_CS_Pin|FLASH_CS_Pin|RF_CTRL1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : CAN_INT_Pin */
  GPIO_InitStruct.Pin = CAN_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CAN_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BUTTON1_Pin */
  GPIO_InitStruct.Pin = BUTTON1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUTTON1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CAN_RX0BF_Pin CAN_RX1BF_Pin */
  GPIO_InitStruct.Pin = CAN_RX0BF_Pin|CAN_RX1BF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_RED_Pin LED_GREEN_Pin LED_BLUE_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin|LED_GREEN_Pin|LED_BLUE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Function implementing the toggleTask thread.
  * @param  argument: Not used
  * @retval None
  */
void ToggleTask(void *argument)
{
  /* USER CODE BEGIN 5 */
#if TX
	RadioCommand radioCommand = {0};
  uint8_t sendBlink = SOLAR_FALSE;
	uint16_t ID = 1;
#elif RX
  RadioData radioData = {0};
#endif
  /* Infinite loop */
  for(;;)
  {
#if TX
    radioCommand.size = 3;
    radioCommand.command = TRANSMIT;
    radioCommand.data = solarMalloc(3);
    memcpy(radioCommand.data, &ID, 2);
    if(HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin)) {
        radioCommand.data[3] = 1;
    } else {
        radioCommand.data[3] = 0;
    }

    if(sendBlink){
      osStatus_t ret = osMessageQueuePut(radioCommandQueue, &radioCommand, 0, 1000);
      if(ret != osOK) {
        solarFree(radioCommand.data);
      } else {
        ID++;
      }
    }

    osMessageQueueGet(toggleCommandQueue, &sendBlink, 0, 0);
    osDelay(100);
#elif RX
    osMessageQueueGet(radioDataQueue, &radioData, NULL, osWaitForever);
    solarPrint("ID: %d", radioData.ID);
    if(radioData.data[0] == 1) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
    }
#endif
  }
  /* USER CODE END 5 */
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if ((GPIO_Pin == CAN_RX0BF_Pin) || (GPIO_Pin == CAN_RX1BF_Pin)) {
		osMessageQueuePut(CANInterruptQueue, &GPIO_Pin, 0, 0);
		//canReceive = 1;
	}
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
#if TX
	RadioCommand radioCommand = {0};
  uint8_t sendBlink = SOLAR_FALSE;
	uint16_t ID = 1;
#elif RX
  RadioData radioData = {0};
#endif
  /* Infinite loop */
  for(;;)
  {
#if TX
    radioCommand.size = 3;
    radioCommand.command = TRANSMIT;
    radioCommand.data = solarMalloc(3);
    memcpy(radioCommand.data, &ID, 2);
    if(HAL_GPIO_ReadPin(BUTTON1_GPIO_Port, BUTTON1_Pin)) {
        radioCommand.data[3] = 1;
    } else {
        radioCommand.data[3] = 0;
    }

    if(sendBlink){
      osStatus_t ret = osMessageQueuePut(radioCommandQueue, &radioCommand, 0, 1000);
      if(ret != osOK) {
        solarFree(radioCommand.data);
      } else {
        ID++;
      }
    }

    osMessageQueueGet(toggleCommandQueue, &sendBlink, 0, 0);
    osDelay(100);
#elif RX
    osMessageQueueGet(radioDataQueue, &radioData, NULL, osWaitForever);
    solarPrint("ID: %d", radioData.ID);
    if(radioData.data[0] == 1) {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
    }
#endif
  }
  /* USER CODE END 5 */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
