/*
 * radio_drivers.c
 *
 *  Created on: Feb. 28, 2023
 *      Author: marcelo
 */

#include "radio_drivers.h"

#if LORA
    uint8_t PACKETPARAMS[] = {0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00};
    uint16_t PACKETPARAMSIZE = 9;
#elif FSK
    uint8_t PACKETPARAMS[] = {0x00, 0x08, 0x04, 0x08, 0x00, 0x00};
    uint16_t PACKETPARAMSIZE = 6;
#endif

void RadioSetCommand(SUBGHZ_RadioSetCmd_t Command, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_ExecSetCmd(&hsubghz, Command, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioGetCommand(SUBGHZ_RadioGetCmd_t Command, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_ExecGetCmd(&hsubghz, Command, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioWriteBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_WriteBuffer(&hsubghz, Offset, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioReadBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_ReadBuffer(&hsubghz, Offset, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioWriteRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_WriteRegisters(&hsubghz, Address, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioReadRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_ReadRegisters(&hsubghz, Address, pBuffer, Size);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioWriteRegister(uint16_t Address, uint8_t Value) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_WriteRegister(&hsubghz, Address, Value);
    }

    osMutexRelease(SUBGHZMutexHandle);
}
void RadioReadRegister(uint16_t Address, uint8_t *pValue) {
    if(osMutexWait(SUBGHZMutexHandle, 0) == osOK) {
        HAL_SUBGHZ_ReadRegister(&hsubghz, Address, pValue);
    }

    osMutexRelease(SUBGHZMutexHandle);
}

void RadioInit() 
{
    osDelay(1);

    uint8_t payload0[] = {SMPS_CLK_DET_ENABLE};
    uint16_t address0 = SUBGHZ_SMPSC0R;
    uint16_t size0 = 1;
    RadioWriteRegisters(address0, payload0, size0);

    uint8_t regulator_mode[] = {0x01};
    RadioSetCommand(RADIO_SET_REGULATORMODE, regulator_mode, 1);

    uint8_t data1[] = {0x00};
    uint16_t size1 = 1;
    RadioSetCommand(RADIO_SET_STANDBY, data1, size1);

    uint8_t data2[] = {TXADDRESS, RXADDRESS};
    uint16_t size2 = 2;
    RadioSetCommand(RADIO_SET_BUFFERBASEADDRESS, data2, size2);


#if LORA
    uint8_t data3[] = {0x01};
#elif FSK
    uint8_t data3[] = {0x00};
#endif
    uint16_t size4 = 1;
    RadioSetCommand(RADIO_SET_PACKETTYPE, data3, size4);

    RadioSetCommand(RADIO_SET_PACKETPARAMS, PACKETPARAMS, PACKETPARAMSIZE);

#if LORA
    //sync conf, not sure what these are so all disabled
    uint8_t SUBGHZ_GBSYNCR[] = {0x00};
    uint16_t SUBGHZ_GBSYNCR_ADDRESS = 0x6AC;
    uint16_t SUBGHZ_GBSYNCR_SIZE = 1;
    RadioWriteRegisters(SUBGHZ_GBSYNCR_ADDRESS, SUBGHZ_GBSYNCR, SUBGHZ_GBSYNCR_SIZE);

    //sync word
    uint8_t SUBGHZ_LSYNCRH[] = {0xA5};
    uint16_t SUBGHZ_LSYNCRH_ADDRESS = 0x740;
    uint16_t SUBGHZ_LSYNCRH_SIZE = 1;
    RadioWriteRegisters(SUBGHZ_LSYNCRH_ADDRESS, SUBGHZ_LSYNCRH, SUBGHZ_LSYNCRH_SIZE);

    uint8_t SUBGHZ_LSYNCRL[] = {0xA5};
    uint16_t SUBGHZ_LSYNCRL_ADDRESS = 0x741;
    uint16_t SUBGHZ_LSYNCRL_SIZE = 1;
    RadioWriteRegisters(SUBGHZ_LSYNCRL_ADDRESS, SUBGHZ_LSYNCRL, SUBGHZ_LSYNCRL_SIZE);
#endif

    //uint8_t data5[] = {0x05, 0x74, 0x2D, 0xE0}; //91.5 MHz
    //uint8_t data5[] = {0x36, 0x89, 0xCA, 0xC0}; //915 MHz
    uint32_t channel = (uint32_t) ((((uint64_t) RADIO_FREQUENCY)<<25)/(XTAL_FREQ) );
    uint8_t data5[4];
    data5[0] = ( uint8_t )( ( channel >> 24 ) & 0xFF );
    data5[1] = ( uint8_t )( ( channel >> 16 ) & 0xFF );
    data5[2] = ( uint8_t )( ( channel >> 8 ) & 0xFF );
    data5[3] = ( uint8_t )( channel & 0xFF );
    uint16_t size7 = 4;
    RadioSetCommand(RADIO_SET_RFFREQUENCY, data5, size7);

#if LORA
    //SF of 8, BW of 62.5, CR 4/5
    uint8_t data8[] = {0x08, 0x03, 0x01, 0x00};
    uint16_t size10 = 4;
#elif FSK
    uint8_t data8[] = {0x01, 0x90, 0x00, 0x00, 0x0B, 0x00, 0x00, 0xD2};
    uint16_t size10 = 8;
#endif
    RadioSetCommand(RADIO_SET_MODULATIONPARAMS, data8, size10);

    uint8_t zero_buffer[256];
    for(int i = 0; i < 256; i++)
      zero_buffer[i] = 0;
    RadioWriteBuffer(0, zero_buffer, 255);
    osDelay(100);

    uint8_t IRQClear[] = {0x03, 0xFF};
    RadioSetCommand(RADIO_CLR_IRQSTATUS, IRQClear, 2);

#if TX
    RadioSetupTX();
    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
#elif RX
    RadioSetupRX();
    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
#endif
}

void RadioSetupTX()
{
    osDelay(1);

    uint8_t data6[] = {0x04, 0x07, 0x00, 0x01};
    uint16_t size8 = 4;
    RadioSetCommand(RADIO_SET_PACONFIG, data6, size8);

    //not sure how ramp up time affects performance, must research
    uint8_t data7[] = {0x16, 0x07};
    uint16_t size9 = 2;
    RadioSetCommand(RADIO_SET_TXPARAMS, data7, size9);

    uint8_t data9[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //enable RX done, TX done, and RX/TX timeout interrupts on IRQ line 1 (from my understanding, an IRQ line can only halt processor once at a time)
    uint16_t size11 = 8;
    RadioSetCommand(RADIO_CFG_DIOIRQ, data9, size11);

#if RF_HP
    HAL_GPIO_WritePin(FE_CTRL1_GPIO_Port, FE_CTRL1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FE_CTRL2_GPIO_Port, FE_CTRL2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FE_CTRL3_GPIO_Port, FE_CTRL3_Pin, GPIO_PIN_SET);
#else
    HAL_GPIO_WritePin(FE_CTRL1_GPIO_Port, FE_CTRL1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FE_CTRL2_GPIO_Port, FE_CTRL2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FE_CTRL3_GPIO_Port, FE_CTRL3_Pin, GPIO_PIN_SET);
#endif
}

void RadioSetupRX()
{   
    uint8_t data9[] = {0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //enable RX done, TX done, and RX/TX timeout interrupts on IRQ line 1 (from my understanding, an IRQ line can only halt processor once at a time)
    uint16_t size11 = 8;
    RadioSetCommand(RADIO_CFG_DIOIRQ, data9, size11);

    HAL_GPIO_WritePin(FE_CTRL1_GPIO_Port, FE_CTRL1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FE_CTRL2_GPIO_Port, FE_CTRL2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FE_CTRL3_GPIO_Port, FE_CTRL3_Pin, GPIO_PIN_SET);

    uint8_t timeout[] = {0xFF, 0xFF, 0xFF}; //no timeout, continuous receive
    RadioSetCommand(RADIO_SET_RX, timeout, 3);

    osDelay(100);
    uint8_t bufferStatus[2];
    RadioGetCommand(RADIO_GET_RXBUFFERSTATUS, bufferStatus, 2);
}

void RadioSendTXContinuousWave() {
    RadioSetCommand(RADIO_SET_TXCONTINUOUSWAVE, NULL, 0); //used to debug and create a wave and output freq
}

int RadioTransmit(uint8_t* data, uint8_t size)
{

    if(size <= RXADDRESS - TXADDRESS)  {
      RadioWriteBuffer(TXADDRESS, data, size);
#if LORA
      PACKETPARAMS[3] = size;
#elif FSK
      PACKETPARAMS[6] = size;
#endif
      RadioSetCommand(RADIO_SET_PACKETPARAMS, PACKETPARAMS, PACKETPARAMSIZE);
    }
    else {
      return 0;
    }

    uint8_t timeout[] = {0x00, 0x00, 0x00}; //no timeout
    RadioSetCommand(RADIO_SET_TX, timeout, 3);

    return 1;
}

void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    uint8_t messageReceived = 1;
    osMessageQueuePut(RadioReceiveInterruptQueue, &messageReceived, 0, 0); //use sempahore instead, for the future
}

void RadioReceive() {
    uint8_t receive;
    //osMessageQueueGet(RadioReceiveInterruptQueue, &receive, 0, osWaitForever);
    uint8_t IRQClear[] = {0x00, 0x02};
    //RadioSetCommand(RADIO_CLR_IRQSTATUS, IRQClear, 2);

    uint8_t bufferStatus[2];
    RadioGetCommand(RADIO_GET_RXBUFFERSTATUS, bufferStatus, 2);
    uint8_t data[bufferStatus[0]];
    RadioReadBuffer(bufferStatus[1], data, bufferStatus[0]);

    //throw it into a queue here instead
    if(data[0]) {
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(BLUE_LED_GPIO_Port, BLUE_LED_Pin, GPIO_PIN_RESET);
    }
}

void RadioReceiveStats() 
{
  uint8_t status;
  uint8_t irqStatus[3];
  uint8_t error[3];
  uint8_t packetStatus[4];
  uint8_t stats[7];

  //RadioGetCommand(RADIO_GET_ERROR, error, 3);
  //RadioGetCommand(RADIO_GET_IRQSTATUS, irqStatus, 3);
  //RadioGetCommand(RADIO_GET_STATUS, &status, 1);
  //RadioGetCommand(RADIO_GET_PACKETSTATUS, packetStatus, 4);
  //RadioGetCommand(RADIO_GET_STATS, stats, 7);
}

void radioLoop()
{
  uint8_t data[255];
  uint8_t size = 8;
#if TX
    if(HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)) {
        data[0] = 1;
    } else {
        data[0] = 0;
    }
    size = 1;
    RadioTransmit(data, size);
#elif RX
    RadioReceive();
#endif
}
