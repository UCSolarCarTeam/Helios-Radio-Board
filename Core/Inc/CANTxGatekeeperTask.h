/*
 * CANTransmitTask.h
 *
 *  Created on: Feb 10, 2024
 *      Author: MacKante
 */


#pragma once

#include "cmsis_os.h"
#include "CAN.h"

void CANTxGatekeeperTask(void* arg);
void CANTxGatekeeper(CANMsg *msg);

extern SPI_HandleTypeDef hspi1;
extern osMutexId_t SPIMutexHandle;
extern osMessageQueueId_t CANTxMessageQueue;
