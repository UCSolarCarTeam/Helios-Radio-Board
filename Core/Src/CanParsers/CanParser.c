#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "CanParser.h"

#include "AuxBmsCanParser.h"
#include "BmsCanParser.h"
#include "DriverControlsCanParser.h"
#include "LightsCanParser.h"
#include "MotorCanParser.h"
#include "MpptCanParser.h"

extern osMessageQueueId_t canRxQueue;

void parseCanTask(void const* arg)
{
    //uint32_t prevWakeTime = osKernelSysTick();
    CanMsg msg;

    for (;;)
    {
        osMessageQueueGet(canRxQueue, &msg, 0, osWaitForever); // Blocks

        parseCanMessage(msg.StdId, msg.Data);
    }
}

void parseCanMessage(uint32_t stdId, uint8_t* data)
{

    if (stdId >= AUX_MBMS_CAN_MIN && stdId <= AUX_MBMS_CAN_MAX)
    {
        parseAuxBmsCanMessage(stdId, data);
    }
    else if (stdId >= MBMS_CAN_MIN && stdId <= MBMS_CAN_MAX)
    {
        parseBmsCanMessage(stdId, data);
    }
    else if (stdId >= DRIVER_CONTROL_CAN_MIN && stdId <= DRIVER_CONTROL_CAN_MAX)
    {
        parseDriverControlsCanMessage(stdId, data);
    }
    else if (stdId >= POWER_BOARD_CAN_MIN && stdId <= POWER_BOARD_CAN_MAX)
    {
        parseLightsCanMessage(stdId, data);
    }
    else if (stdId >= MPPT_CAN_MIN && stdId <= MPPT_CAN_MAX)
    {
        parseMpptCanMessage(stdId, data);
    }
    else if (stdId >= MOTOR_CONTROLLER_CAN_MIN && stdId <= MOTOR_CONTROLLER_CAN_MAX)
    {
        parseMotorCanMessage(stdId, data);
    }
}
