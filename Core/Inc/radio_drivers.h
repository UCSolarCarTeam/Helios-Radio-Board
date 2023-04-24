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
#define XTAL_FREQ ( 32000000UL )

#define RADIO_FREQUENCY 915000000
#define TXADDRESS 0x00
#define RXADDRESS 0x08

#define TX 1
#define RF_HP 1

extern SUBGHZ_HandleTypeDef hsubghz;

void RadioInit();
void RadioSetupTX();
void RadioSetupRX();
int RadioTransmit(uint8_t* data, uint8_t size);
void RadioReceive(uint8_t* data, uint8_t* size);
void radioLoop();