/*
 * radio_drivers.h
 *
 *  Created on: Feb. 28, 2023
 *      Author: Marcelo
 */
#ifndef RADIO_DRIVERS_H
#define RADIO_DRIVERS_H

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "cmsis_os2.h"
#include "stm32wlxx_hal.h"
#include "stm32wlxx_hal_subghz.h"
#include "system_defines.h"

#define SMPS_CLK_DET_ENABLE ((uint8_t) (1<<6))
#define SUBGHZ_SMPSC0R      0x0916
#define XTAL_FREQ ( 32000000UL )

#define RADIO_FREQUENCY 915000000
#define TXADDRESS 0x00
#define RXADDRESS 0x08

#define TX 1
#define RX !TX

#if TX
    #define RF_HP 1
#endif

#define RADIO_RECEIVE_INTERRUPT_QUEUE_COUNT 16
#define RADIO_DATA_QUEUE_COUNT 16

typedef enum RadioConfig {
    //Packet parameters command
    PREAMBLE_SYMBOLS_MSB = 0,
    PREAMBLE_SYMBOLS_LSB,
    HEADER_TYPE,
    PAYLOAD_LENGTH,
    CRC_ENABLE,
    INVERT_IQ,

    //Modulation parameters command
    SPREADING_FACTOR,
    BANDWITH,
    CR,
    LDR_OPTIMIZATION,

    //Frequency command
    //FREQUENCY

    //SMPS register value
    SMPS_ENABLE,

    //Regulator Mode command
    REGULATOR_MODE,

    //buffer address command
    TX_ADDRESS,
    RX_ADDRESS,

    //Tx Parameters command
    POWER,
    RAMP_TIME,

    //Pa Config command
    PA_DUTY_CYCLE,
    HP_MAX,
    PA_SEL,

    //synchronization config
    GBSYNCR,
    LSYNCRH,
    LSYNCRL,

    //overall array size must be last!
    SIZE 

} RadioConfig;

typedef struct RadioData {
    uint16_t ID;
    uint8_t data[8];
    uint8_t size;
} RadioData;

extern SUBGHZ_HandleTypeDef hsubghz;
extern osMutexId_t SUBGHZMutexHandle; //Mutex for radio_SPI, unused
extern osMessageQueueId_t radioDataQueue; //acts as a middle man, if in TX mode, it sends data to radioTransmit, if in RX mode, its used to receive data from RadioReceive
extern osMessageQueueId_t radioCommandQueue;
extern osMessageQueueId_t RadioReceiveInterruptQueue; //actsa semaphore to tell the receive task when to read from the RX buffer

//wrappers to use radio spi mutex
void RadioSetCommand(SUBGHZ_RadioSetCmd_t Command, uint8_t *pBuffer, uint16_t Size);
void RadioGetCommand(SUBGHZ_RadioGetCmd_t Command, uint8_t *pBuffer, uint16_t Size);
void RadioWriteBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size);
void RadioReadBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size);
void RadioWriteRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size);
void RadioReadRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size);
void RadioWriteRegister(uint16_t Address, uint8_t Value);
void RadioReadRegister(uint16_t Address, uint8_t *pValue);

void RadioInit();
void RadioSetupTX();
void RadioSetupRX();
void RadioSendTXContinuousWave();
int RadioTransmit(uint8_t* data, uint8_t size);
void RadioReceiveStats();
void radioHandleCommand(RadioCommand *radioCommand);
void RadioLoop();
void RadioTask(void *argument);

#endif /* RADIO_DRIVERS_H */
