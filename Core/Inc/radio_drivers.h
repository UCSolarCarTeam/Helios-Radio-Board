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

#define RADIO_FREQUENCY 91500000
#define TXADDRESS 0x00
#define RXADDRESS 0x08

#define LORA 1
#define FSK !LORA

#define TX 0

#if TX
    #define RF_HP 1
#endif

extern SUBGHZ_HandleTypeDef hsubghz;

#if LORA
    uint8_t PACKETPARAMS[] = {0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00};
    uint16_t PACKETPARAMSIZE = 9;
#elif FSK
    uint8_t PACKETPARAMS[] = {0x00, 0x08, 0x04, 0x08, 0x00, 0x00};
    uint16_t PACKETPARAMSIZE = 6;
#endif

void RadioInit();
void RadioSetupTX();
void RadioSetupRX();
int RadioTransmit(uint8_t* data, uint8_t size);
void RadioReceive(uint8_t* data, uint8_t* size);
void radioLoop();