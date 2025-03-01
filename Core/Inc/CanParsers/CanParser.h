#pragma once

#include "stm32wlxx.h"
#include "CAN.h"


#define MBMS_CAN_MIN (0x100)
#define MBMS_CAN_MAX (0x10F)

#define CONTACTOR_CAN_MIN (0x200)
#define CONTACTOR_CAN_MAX (0x20F)

#define AUX_MBMS_CAN_MIN (0x300) //Orion
#define AUX_MBMS_CAN_MAX (0x30F)

#define MPPT_CAN_MIN (0x400)
#define MPPT_CAN_MAX (0x403)

#define MOTOR_CONTROLLER_CAN_MIN (0x500)
#define MOTOR_CONTROLLER_CAN_MAX (0x507)

#define MOTOR_COMMAND_CAN_MIN (0x550)
#define MOTOR_COMMAND_CAN_MAX (0x551)

#define B3_CAN_MIN (0x600)
#define B3_CAN_MAX (0x6FF)

#define DRIVER_CONTROL_CAN_MIN (0x610)
#define DRIVER_CONTROL_CAN_MAX (0x61F)

#define POWER_BOARD_CAN_MIN (0x620)
#define POWER_BOARD_CAN_MAX (0x62F)

#define TELEMETRY_CAN_MIN (0x630)
#define TELEMETRY_CAN_MAX (0x63F)

#define PROXIMITY_CAN_MIN (0x700)
#define PROXIMITY_CAN_MAX (0x704)


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
