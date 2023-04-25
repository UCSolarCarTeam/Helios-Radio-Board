/*
 * radio_drivers.c
 *
 *  Created on: Feb. 28, 2023
 *      Author: marcelo
 */

#include "radio_drivers.h"

void RadioInit() 
{
    uint8_t status;
    uint8_t irqstatus[3];
    uint8_t read;

    osDelay(1);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t payload0[] = {SMPS_CLK_DET_ENABLE};
    uint16_t address0 = SUBGHZ_SMPSC0R;
    uint16_t size0 = 1;
    HAL_SUBGHZ_WriteRegisters(&hsubghz, address0, payload0, size0);

    uint8_t regulator_mode[] = {0x01};
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_REGULATORMODE, regulator_mode, 1);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data1[] = {0x01};
    uint16_t size1 = 1;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, data1, size1);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data2[] = {TXADDRESS, RXADDRESS};
    uint16_t size2 = 2;
    HAL_SUBGHZ_ReadRegisters(&hsubghz, 0x0802, &read, 1);
    HAL_SUBGHZ_ReadRegisters(&hsubghz, 0x0803, &read, 1);
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, data2, size2);
    HAL_SUBGHZ_ReadRegisters(&hsubghz, 0x0802, &read, 1);
    HAL_SUBGHZ_ReadRegisters(&hsubghz, 0x0803, &read, 1);

    uint8_t data3[] = {0x00};
    uint16_t size4 = 1;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, data3, size4);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data4[] = {0x00, 0x08, 0x04, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00};
    uint16_t size5 = 9;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, data4, size5);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t payload2[] = {0x11};
    uint16_t address2 = 0x6C0;
    uint16_t size6 = 1;
    HAL_SUBGHZ_WriteRegisters(&hsubghz, address2, payload2, size6);

    //uint8_t data5[] = {0x05, 0x74, 0x2D, 0xE0}; //91.5 MHz
    //uint8_t data5[] = {0x36, 0x89, 0xCA, 0xC0}; //915 MHz
    uint32_t channel = (uint32_t) ((((uint64_t) RADIO_FREQUENCY)<<25)/(XTAL_FREQ) );
    uint8_t data5[4];
    data5[0] = ( uint8_t )( ( channel >> 24 ) & 0xFF );
    data5[1] = ( uint8_t )( ( channel >> 16 ) & 0xFF );
    data5[2] = ( uint8_t )( ( channel >> 8 ) & 0xFF );
    data5[3] = ( uint8_t )( channel & 0xFF );
    uint16_t size7 = 4;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, data5, size7);

    uint8_t data8[] = {0x01, 0x90, 0x00, 0x00, 0x0B, 0x00, 0x00, 0xD2};
    uint16_t size10 = 8;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, data8, size10);

    int zero_buffer[256];
    for(int i = 0; i < 257; i++){
      zero_buffer[i] = 0
    }
    HAL_SUBGHZ_WriteBuffer(&hsubghz, 0, zero_buffer, 255);
    osDelay(100);

#if TX
    RadioSetupTX();
    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
#else
    RadioSetupRX();
    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
#endif
}

void RadioSetupTX()
{
    uint8_t status;
    uint8_t irqstatus[3];

    osDelay(1);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data6[] = {0x04, 0x07, 0x00, 0x01};
    uint16_t size8 = 4;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACONFIG, data6, size8);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data7[] = {0x16, 0x07};
    uint16_t size9 = 2;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, data7, size9);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data8[] = {0x01, 0x90, 0x00, 0x00, 0x0B, 0x00, 0x00, 0xD2};
    uint16_t size10 = 8;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, data8, size10);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

    uint8_t data9[] = {0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00};
    uint16_t size11 = 8;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, data9, size11);

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);

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
    uint8_t data9[] = {0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00};
    uint16_t size11 = 8;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, data9, size11);

    HAL_GPIO_WritePin(FE_CTRL1_GPIO_Port, FE_CTRL1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FE_CTRL2_GPIO_Port, FE_CTRL2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(FE_CTRL3_GPIO_Port, FE_CTRL3_Pin, GPIO_PIN_SET);
}

int RadioTransmit(uint8_t* data, uint8_t size)
{
    uint8_t status;
    uint8_t irqstatus[3];

    uint8_t data1[] = {0x01};
    uint16_t size1 = 1;

    if(size < RXADDRESS - TXADDRESS)  {
      HAL_SUBGHZ_WriteBuffer(&hsubghz, TXADDRESS, data, size);
      uint8_t data4[] = {0x00, 0x08, 0x04, 0x08, 0x00, 0x00, size, 0x00, 0x00};
      uint16_t size5 = 9;
      HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, data4, size5);
      HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
      HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);
    }
    else {
      return 0;
    }

    //for(int i = 0; i < size; i++)
    //{HAL_SUBGHZ_WriteRegisters(&hsubghz, TXADDRESS, data, size + i*8);}

    uint8_t data10[] = {0x00, 0x00, 0x00};
    uint16_t size12 = 3;

    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, data10, size12);
    /*
    if (!HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))
    {
      HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, data10, size12);
    }
    else 
    {
      HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, data1, size1);
    }
    */

    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
    HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);
    
    osDelay(5);

    //while (((status & 0x70) >> 4) != 0x3)
    {
      HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
      HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqstatus, 3);
    }

    uint8_t data11[] = {0xff, 0xff};
    uint16_t size13 = 2;
    HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, data11, size13);

    osDelay(10);

    return 1;
}

// ayo redbuffer is not the same as readregister?!?!?!?!?!?! shit I think i know what I did wrong
void RadioReceive(uint8_t* data, uint8_t* size) 
{
  uint8_t status;
  uint8_t irqStatus[3];
  uint8_t packetStatus[4];
  uint8_t stats[7];
  uint8_t bufferStatus[4];

  uint8_t data10[] = {0x00, 0x00, 0x00};
  uint16_t size12 = 3;
  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);

  HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RX, data10, size12);

  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_ERROR, irqStatus, 3);

  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_RXBUFFERSTATUS, bufferStatus, 4);

  HAL_SUBGHZ_ReadBuffer(&hsubghz, bufferStatus[2], data, 2);
  HAL_SUBGHZ_ReadBuffer(&hsubghz, RXADDRESS, data, 1);
  HAL_SUBGHZ_ReadBuffer(&hsubghz, 0, data, 255);

  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &status, 1);
  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_PACKETSTATUS, packetStatus, 4);
  HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATS, stats, 7);

  uint8_t data11[] = {0xff, 0xff};
  uint16_t size13 = 2;
  HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CLR_IRQSTATUS, data11, size13);

  HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, data10, size12);
  osDelay(100);
}

void radioLoop()
{
  uint8_t data[255];
  uint8_t size = 8;
  data[0] = 0xA5;
  for(int i = 1; i < 9; i++)
    data[i] = i;
#if TX
    RadioTransmit(data, size);
#else
    RadioReceive(data, &size);
#endif
}
