#include <string.h> // for memcpy

#include "cmsis_os.h"

#include "CanParser.h"

#include "AuxBmsCanParser.h"
#include "BmsCanParser.h"
#include "MotorCanParser.h"
#include "MpptCanParser.h"
#include "B3CanParser.h"

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

    if (stdId >= B3_CAN_MIN && stdId <= B3_CAN_MAX)
    {
        parseB3CanMessage(stdId, data);
    }
}
