#pragma once

#include "stm32wlxx.h"

void parseAuxBmsCanMessage(uint32_t stdId, uint8_t* data);
void parseAuxBmsHeartbeat();
void parseAuxBmsInfo(uint8_t* data);
void parseAuxTrip(uint8_t* data);