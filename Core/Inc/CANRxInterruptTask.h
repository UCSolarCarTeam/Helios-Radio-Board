/*
 * CANTransmitTask.h
 *
 *  Created on: Feb 10, 2024
 *      Author: MacKante
 */

#pragma once

#include "cmsis_os.h"
#include "CAN.h"

void CanRxInterruptTask(void* arg);
void CANRxInterrupt(void const* arg);

extern SPI_HandleTypeDef hspi1;
extern osMutexId_t SPIMutexHandle;
extern osMessageQueueId_t CANInterruptQueue;
