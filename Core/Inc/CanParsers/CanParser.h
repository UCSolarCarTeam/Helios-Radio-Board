#pragma once

#include "stm32wlxx.h"
#include "CAN.h"

#define AUX_BMS_CAN_MIN (0x720)
#define AUX_BMS_CAN_MAX (0x72F)

#define BMS_CAN_MIN (0x300)
#define BMS_CAN_MAX (0x30F)

#define DRIVER_CONTROL_CAN_MIN (0x700)
#define DRIVER_CONTROL_CAN_MAX (0x70F)

#define LIGHTS_CAN_MIN (0x710)
#define LIGHTS_CAN_MAX (0x71F)

#define MOTOR_CAN_MIN (0x400)
#define MOTOR_CAN_MAX (0x503)

#define MPPT_CAN_MIN (0x600)
#define MPPT_CAN_MAX (0x603)

#define LED_ON 1
#define LED_OFF 0


typedef struct
{
    uint32_t StdId;
    uint8_t DLC;
    uint8_t Data[8];
} CanMsg;

void parseCanTask(void const* arg);
void parseCanMessage(uint32_t stdId, uint8_t* data);
