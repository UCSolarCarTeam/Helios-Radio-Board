#pragma once

#include "main.h"
#include <stdint.h>
#include "cmsis_os.h"
#include "system_defines.h"
#include "radio_drivers.h"

void CAN_IC_READ_REGISTER(uint8_t address, uint8_t* buffer);
void CAN_IC_WRITE_REGISTER_BITWISE(uint8_t address, uint8_t mask, uint8_t value);
void CAN_IC_WRITE_REGISTER(uint8_t address, uint8_t value);
void ConfigureCANSPI(void);
void sendCANMessage(uint8_t channel, uint16_t ID, uint8_t DLC, uint8_t* data);
void sendExtendedCANMessage(uint8_t channel, uint64_t ID, uint8_t DLC, uint8_t* data);
void receiveCANMessage(uint8_t channel, uint32_t* ID, uint8_t* DLC, uint8_t* data);
void CANRXInterruptTask(void* arg);

extern uint8_t canReceive;

extern SPI_HandleTypeDef hspi1;
extern osMutexId_t SPIMutexHandle;
extern osMessageQueueId_t CANInterruptQueue;

extern uint8_t blueStatus;
extern uint8_t greenStatus;
