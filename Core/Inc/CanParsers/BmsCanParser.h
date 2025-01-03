#pragma once

#include "stm32wlxx.h"

void parseBmsCanMessage(uint32_t stdId, uint8_t* data);
void parseBmsHeartbeat();
void parseStartupInfo(uint8_t* data);
void parsePackInfo(uint8_t* data);
void parseErrors(uint8_t* data);
void parseTempInfo(uint8_t* data);
void parseCellVoltages(uint8_t* data);
