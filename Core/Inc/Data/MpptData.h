#pragma once

#include "stm32wlxx.h"

// Defined at https://docs.google.com/spreadsheets/d/1soVLjeD9Sl7z7Z6cYMyn1fmn-cG7tx_pfFDsvgkCqMU/edit?usp=sharing
#define MPPT_PKG_ID 9

#define NUM_OF_MPPTS 4
#define MPPT_ALIVE_BIT_MASK 0x80

struct MpptData
{
    uint32_t lastReceived;
    unsigned short int arrayVoltage;
    unsigned short int arrayCurrent;
    unsigned short int batteryVoltage;
    unsigned short int temperature;
};

extern struct MpptData mpptData[NUM_OF_MPPTS];
