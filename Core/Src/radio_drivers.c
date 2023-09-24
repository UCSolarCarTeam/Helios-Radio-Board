/*
 * radio_drivers.c
 *
 *  Created on: Feb. 28, 2023
 *      Author: Marcelo
 */

#include "radio_drivers.h"

RadioConfig radioConfig = { .preambleSymbols = 0xC,
                            .payloadLength = 8,
                            .headerType = 0,
                            .CRCenable = 0,
                            .invertIQ = 0,
                            .spreadingFactor = 8,
                            .bandwith = 3,
                            .cr = 1,
                            .ldrOptimization = 0,
                            .frequency = 915000000,
                            .SMPSEenable = ((uint8_t) (1<<6)), //register value
                            .regulatorMode = 1,
                            .TXaddress = 0,
                            .RXaddress = 8,
                            .power = 0x16,
                            .rampTime = 0x05,
                            .paDutyCycle = 0x04,
                            .hpMax = 0x07,
                            .paSel = 0,
                            .gbsyncr = 0,   //register value
                            .lsyncrH = 0xA5, //register value
                            .lsyncrL = 0xA5 //register value
                            };

//TODO: There must be a nicer way to have Mutexes or change SUBGHZ_Handle name be used or unused with a single macro (perhaps macro function declaration)
void RadioSetCommand(SUBGHZ_RadioSetCmd_t Command, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_ExecSetCmd(&hsubghz, Command, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioGetCommand(SUBGHZ_RadioGetCmd_t Command, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_ExecGetCmd(&hsubghz, Command, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioWriteBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_WriteBuffer(&hsubghz, Offset, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioReadBuffer(uint8_t Offset, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_ReadBuffer(&hsubghz, Offset, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioWriteRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_WriteRegisters(&hsubghz, Address, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioReadRegisters(uint16_t Address, uint8_t *pBuffer, uint16_t Size) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_ReadRegisters(&hsubghz, Address, pBuffer, Size);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioWriteRegister(uint16_t Address, uint8_t Value) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_WriteRegister(&hsubghz, Address, Value);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}
void RadioReadRegister(uint16_t Address, uint8_t *pValue) {
    //if(osMutexWait(SUBGHZMutexHandle, 0) == osOK)
    {
        HAL_SUBGHZ_ReadRegister(&hsubghz, Address, pValue);
        //osMutexRelease(SUBGHZMutexHandle);
    }
}

//Send radio to deep sleep, erases all configuration registers
void RadioDeinit() 
{
    uint8_t data = 0x00;
    RadioSetCommand(RADIO_SET_SLEEP, &data, 1);
}

void RadioInit() 
{
    osDelay(1);

    //enable SMPS clock detection, to use external TCXO
    uint8_t SMPSC0R[] = {radioConfig.SMPSEenable};
    uint16_t address = SUBGHZ_SMPSC0R;
    uint16_t size = 1;
    RadioWriteRegisters(SUBGHZ_SMPSC0R, SMPSC0R, size);

    //SMPS mode used
    uint8_t regulator_mode[] = {radioConfig.regulatorMode};
    RadioSetCommand(RADIO_SET_REGULATORMODE, regulator_mode, size);

    //set standby with RC13 clock
    uint8_t standbyCfg[] = {0x00};
    RadioSetCommand(RADIO_SET_STANDBY, standbyCfg, size);

    //set address for relevant buffers
    uint8_t bufferBaseAddress[] = {radioConfig.TXaddress, radioConfig.RXaddress};
    size = 2;
    RadioSetCommand(RADIO_SET_BUFFERBASEADDRESS, bufferBaseAddress, size);

#if LORA
    //LORA packet type
    uint8_t packetType[] = {0x01};
#elif FSK
    //FSK packet type
    uint8_t packetType[] = {0x00};
#endif
    size = 1;
    RadioSetCommand(RADIO_SET_PACKETTYPE, packetType, size);

#if LORA
    // 12 preamble symbols, explicit header (variable size), size (overwritten), CRC disabled, standard IQ setup (no idea)
    uint8_t packetParameters[] = {radioConfig.preambleSymbols >> 8, radioConfig.payloadLength & 0xFF, radioConfig.headerType, radioConfig.CRCenable, radioConfig.invertIQ};
    size = 6;
#elif FSK
    //12 preamble symbols, preamble detection disabled, 8 bit sync word,addres comparison/filtering disabled, fixed payload, payload_length (overwritten), CRC disabled, whitening disabled
    uint8_t packetParameters[] = {0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00};
    size = 9;
#endif
    RadioSetCommand(RADIO_SET_PACKETPARAMS, packetParameters, size);

#if LORA
    //sync conf, not sure what these are so all disabled
    uint8_t GBSYNCR[] = {radioConfig.gbsyncr};
    address = 0x6AC;
    RadioWriteRegisters(address, GBSYNCR, size);

    //sync words (must the same on receiver and transmitter)
    uint8_t LSYNCRH[] = {radioConfig.lsyncrH};
    address = 0x740;
    RadioWriteRegisters(address, LSYNCRH, size);

    uint8_t LSYNCRL[] = {radioConfig.lsyncrL};
    address = 0x741;
    RadioWriteRegisters(address, LSYNCRL, size);
#endif

    //used to set frequency, copied from stm32wl code package
    uint32_t channel = (uint32_t) ((((uint64_t) radioConfig.frequency)<<25)/(XTAL_FREQ) );
    uint8_t RFfreq[4];
    RFfreq[0] = ( uint8_t )( ( channel >> 24 ) & 0xFF );
    RFfreq[1] = ( uint8_t )( ( channel >> 16 ) & 0xFF );
    RFfreq[2] = ( uint8_t )( ( channel >> 8 ) & 0xFF );
    RFfreq[3] = ( uint8_t )( channel & 0xFF );
    size = 4;
    RadioSetCommand(RADIO_SET_RFFREQUENCY, RFfreq, size);

#if LORA
    //SF of 8, BW of 62.5, CR 4/5
    uint8_t modulationParamaters[] = {radioConfig.spreadingFactor, radioConfig.bandwith, radioConfig.cr, radioConfig.ldrOptimization};
    size = 4;
#elif FSK
    uint8_t data8[] = {0x01, 0x90, 0x00, 0x00, 0x0B, 0x00, 0x00, 0xD2};
    uint16_t size10 = 8;
#endif
    RadioSetCommand(RADIO_SET_MODULATIONPARAMS, modulationParamaters, size);

    //Clear up data buffer in radio module (unnecessary, but used for debugging)
    uint8_t zero_buffer[256];
    for(int i = 0; i < 256; i++)
      zero_buffer[i] = 0;
    RadioWriteBuffer(0, zero_buffer, 255);
    osDelay(100);

    //Clear interrupts
    uint8_t IRQClear[] = {0x03, 0xFF};
    RadioSetCommand(RADIO_CLR_IRQSTATUS, IRQClear, 2);

#if TX
    RadioSetupTX();
    //Debug lights
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);
#elif RX
    RadioSetupRX();
    //Debug lights
    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
#endif
}

#if TX
void RadioSetupTX()
{
    osDelay(1);

    // HP PA mode, HP PA max power, last byte must be 0x01 according to datasheet
    uint8_t paConfig[] = {radioConfig.paDutyCycle, radioConfig.hpMax, radioConfig.paSel, 0x01};
    uint16_t size = 4;
    RadioSetCommand(RADIO_SET_PACONFIG, paConfig, size);

    //22db power, 800 μs ramp time. not sure how ramp up time affects performance, must research
    uint8_t txParameters[] = {radioConfig.power, radioConfig.rampTime};
    size = 2;
    RadioSetCommand(RADIO_SET_TXPARAMS, txParameters, size);

    //enable RX done, TX done, and RX/TX timeout interrupts on IRQ line 1 (from my understanding, an IRQ line can only halt processor once at a time)
    uint8_t dioIRQConfig[] = {0x02, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; 
    size = 8;
    RadioSetCommand(RADIO_CFG_DIOIRQ, dioIRQConfig, size);

#if RF_HP
    //Relay setup for HP TX according to schematic
    HAL_GPIO_WritePin(RF_CTRL1_GPIO_Port, RF_CTRL1_Pin, GPIO_PIN_SET);
#else
    //Relay setup cannot be done for radio board according to implementation
    HAL_GPIO_WritePin(RF_CTRL1_GPIO_Port, RF_CTRL1_Pin, GPIO_PIN_SET);
#endif
}

void RadioSendTXContinuousWave() {
    //used to debug and create a wave and output freq
    RadioSetCommand(RADIO_SET_TXCONTINUOUSWAVE, NULL, 0);
}

int RadioTransmit(uint8_t* data, uint8_t size)
{
    //Check if packet fits in the current TX buffer size
    if(size <= radioConfig.RXaddress - radioConfig.TXaddress)  {
      RadioWriteBuffer(radioConfig.TXaddress, data, size);
#if LORA
    // 12 preamble symbols, explicit header (variable size), size (overwritten), CRC disabled, standard IQ setup (no idea)
    uint8_t packetParameters[] = {radioConfig.preambleSymbols >> 8, radioConfig.preambleSymbols & 0xFF, radioConfig.headerType, size, radioConfig.CRCenable, radioConfig.invertIQ};
    uint8_t commandSize = 6;
#elif FSK
    //12 preamble symbols, preamble detection disabled, 8 bit sync word,addres comparison/filtering disabled, fixed payload, payload_length (overwritten), CRC disabled, whitening disabled
    uint8_t packetParameters[] = {0x00, 0x0C, 0x00, 0x08, 0x00, 0x00, 0x02, 0x00, 0x00};
    uint8_t commandSize = 9;
#endif
      RadioSetCommand(RADIO_SET_PACKETPARAMS, packetParameters, commandSize);
    }
    else {
      return SOLAR_FALSE;
    }

    //TX oneshot with no timeout
    uint8_t timeout[] = {0x00, 0x00, 0x00}; 
    RadioSetCommand(RADIO_SET_TX, timeout, 3);

    //Add handle of failed TX ()
    uint8_t status;
    RadioGetCommand(RADIO_GET_STATUS, &status, 1);
    while((status & 0b01110000) == 0b01100000)
        RadioGetCommand(RADIO_GET_STATUS, &status, 1);
    if((status & 0b00001110) == 0b00001100) {
        solarPrint("blinky blink %d\n", data[0]);
        HAL_GPIO_TogglePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin);
    }

    //warm sleep, cfg maintained
    uint8_t sleepCommand = 0x04;
    RadioSetCommand(RADIO_SET_SLEEP, &sleepCommand, 1);
    return SOLAR_TRUE;
}
#elif RX
void RadioSetupRX()
{   

    uint8_t status;

    //enable RX done, TX done, and RX/TX timeout interrupts on IRQ line 1 (from my understanding, an IRQ line can only halt processor once at a time)
    uint8_t dioIRQConfig[] = {0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00};
    uint16_t size = 8;
    RadioSetCommand(RADIO_CFG_DIOIRQ, dioIRQConfig, size);

    //Relay setup for RX according to dev-board datasheet
    HAL_GPIO_WritePin(RF_CTRL1_GPIO_Port, RF_CTRL1_Pin, GPIO_PIN_RESET);

    //no timeout, continuous receive
    uint8_t timeout[] = {0xFF, 0xFF, 0xFF};
    RadioSetCommand(RADIO_SET_RX, timeout, 3);
}

//TODO: use semaphore instead, for the future
void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz) {
    RadioCommand radioCommand = {0};
    radioCommand.command = RECEIVE;
    osMessageQueuePut(radioCommandQueue, &radioCommand, 0, 0);
}

void RadioReceive() {
    //TODO: Problem, if more packets are sent than we can handle, we could read the same packet over and over again
    //since our current method uses GET_RXBUFFERSTATUS, which only gives you the newest packet
    //probably make our method to keep track of the buffer read and interate until you get to the newest packet

    //clear interrupt since IRQ_CLR from handler can fail
    uint8_t IRQClear[] = {0x00, 0x02};
    RadioSetCommand(RADIO_CLR_IRQSTATUS, IRQClear, 2);

    //receive newest packet length and address
    uint8_t bufferStatus[2];
    RadioGetCommand(RADIO_GET_RXBUFFERSTATUS, bufferStatus, 2);

    //read newest packet from data buffer
    uint8_t data[8];
    RadioReadBuffer(bufferStatus[1], data, bufferStatus[0]);

    //throw it into the data queue
    RadioData radioData = {0};
    radioData.size = bufferStatus[0] - 2; //minus two cause 2 of those are from ID
    memcpy(&(radioData.ID), data, 2);
    memcpy(&(radioData.data), &(data[2]), radioData.size);
    osMessageQueuePut(radioDataQueue, &radioData, 0, osWaitForever);
}
#endif

//obtain radio stats
void RadioReceiveStats() 
{
  uint8_t status;
  uint8_t irqStatus[3];
  uint8_t error[3];
  uint8_t packetStatus[4];
  uint8_t stats[7];

  RadioGetCommand(RADIO_GET_STATUS, &status, 1);
  RadioGetCommand(RADIO_GET_IRQSTATUS, irqStatus, 3);
  RadioGetCommand(RADIO_GET_ERROR, error, 3);
  RadioGetCommand(RADIO_GET_PACKETSTATUS, packetStatus, 4);
  RadioGetCommand(RADIO_GET_STATS, stats, 7);

  //Clear interrupts
  uint8_t IRQClear[] = {0x03, 0xFF};
  RadioSetCommand(RADIO_CLR_IRQSTATUS, IRQClear, 2);
}

void radioHandleCommand(RadioCommand *radioCommand)
{
    switch(radioCommand->command)
    {
        case SET_COMMAND:
            RadioSetCommand((SUBGHZ_RadioSetCmd_t)radioCommand->address, radioCommand->data, radioCommand->size);
            /*TODO: add check for correct size of buffer input and valid SUBGHZ_RadioSetCmd_t enum*/
            break;
        case GET_COMMAND:
            uint8_t commandReadback[10];
            RadioGetCommand((SUBGHZ_RadioSetCmd_t)radioCommand->address, commandReadback, radioCommand->size);
            solarPrint("readback Values: ");
            for(int i = 0; i < radioCommand->size; i++)
            {
                solarPrint("%d, ", commandReadback[i]);
            }
            solarPrint("\n");
            /*TODO: add check for correct size of buffer input and valid SUBGHZ_RadioSetCmd_t enum*/
            break;
        case WRITE_BUFFER:
            RadioWriteBuffer(radioCommand->address, radioCommand->data, radioCommand->size);
            break;
        case READ_BUFFER:
            RadioReadBuffer(radioCommand->address, radioCommand->data, radioCommand->size);
            break;
        case WRITE_REGISTER:
            RadioWriteRegister(radioCommand->address, *radioCommand->data);
            break;
        case READ_REGISTER:
            uint8_t register_readback;
            RadioReadRegister(radioCommand->address, &register_readback);
            break;
    #if TX
        case TRANSMIT:
            RadioTransmit(radioCommand->data, (uint8_t)radioCommand->size);
            break;
    #elif RX
        case RECEIVE:
            RadioReceive();
            break;
    #endif
        case STOP_RADIO:
            RadioDeinit();
            break;
        case START_RADIO:
            RadioInit();
            break;
        case RESTART_RADIO:
            RadioDeinit();
            RadioInit();
            break;
        default:
            solarPrint("invalid Radio Command");
    }
    solarFree(radioCommand->data);
}

//main radio function, called by the radioTask
void RadioLoop()
{
    RadioCommand radioCommand = {0};

    osStatus_t ret = osMessageQueueGet(radioCommandQueue, &radioCommand, NULL, 1000);
    if(ret == osOK) {
    	radioHandleCommand(&radioCommand);
    }
}

/**
  * @brief  Function implementing the RadioTask thread.
  * @param  argument: Not used
  * @retval None
*/
void RadioTask(void *argument)
{
  /* Infinite loop */
  for(;;)
  {
    RadioLoop();
  }
}
