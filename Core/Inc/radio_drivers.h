/*
 * radio_drivers.h
 *
 *  Created on: Feb. 28, 2023
 *      Author: marce
 */


#include <stdint.h>
#include "main.h"
#include "cmsis_os.h"
#include "stm32wlxx_hal.h"
#include "stm32wlxx_hal_subghz.h"

#define SMPS_CLK_DET_ENABLE ((uint8_t) (1<<6))
#define SUBGHZ_SMPSC0R      0x0916

extern SUBGHZ_HandleTypeDef hsubghz;

void RadioInit();
void RadioSetupTX();
void RadioSetupRX();
void RadioTransmit();