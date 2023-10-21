#include "CAN.h"

//TODO ADD A FUNCTION THAT CHECKS FOR WHAT CHANNELS AVAILABLE FOR SENDING AND
//A FUNCTION THAT CALLS THE ABOVE FUNCTION AND THE RELEVANT SENDCANMESSAGE FUNCTION
//(extended vs regular CAN)

//TODO: Test receiving interrupt and task

//TODO: Comment the heck out of everything

//TODO:write a how to use README that includes creating CANRXInterruptTask, mutex and queue as well as adding CAN.C and CAN.h

/**
 * @brief write to registry in CAN IC
 * @param address: hex address of the register
 * 		  value: value to be written to the register
 * @retval None
 */
void CAN_IC_READ_REGISTER(uint8_t address, uint8_t* buffer)
{
	uint8_t packet[3] = {0x03, address};

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); //set CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 2, 100U); //transmit
	HAL_SPI_Receive(&hspi1, buffer, 1, 100U); //receive register contents
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET); //set CS pin high
}

/**
 * @brief write to a spicific series of bits in a register in CAN IC
 * @param address: hex address of the register
 * 		  mask: bit mask
 * 		  value: value to be written to the register
 * @retval None
 */
void CAN_IC_WRITE_REGISTER_BITWISE(uint8_t address, uint8_t mask, uint8_t value)
{
	uint8_t packet[4] = {0x05, address, mask, value};

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); //set CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 4, 100U); //transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET); //set CS pin high
}

/**
 * @brief write to registry in CAN IC
 * @param address: hex address of the register
 * 		  value: value to be written to the register
 * @retval None
 */
void CAN_IC_WRITE_REGISTER(uint8_t address, uint8_t value)
{
	uint8_t packet[3] = {0x02, address, value};

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); //set CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 3, 100U);	//transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET); //set CS pin high
}

/**
  * @brief configure CAN IC through SPI
  * @param None
  * @retval None
  * Configuration is as close to Elysia's CAN configuration whenever possible
  * TODO: add configuration verification and return value accordingly
  */
void ConfigureCANSPI(void)
{
	uint8_t resetCommand = 0xa0; //instruction to reset IC to default
	uint8_t CNF1 = 0x00; //BRP = 0 to make tq = 250ns and a SJW of 1Tq
	uint8_t CNF2 = 0xd8; //PRSEG = 0, PHSEG1 = 3, SAM = 0, BTLMODE = 1
	uint8_t CNF3 = 0x01; //WAFKIL disabled, PHSEG2 = 2 (BTL enabled) but PHSEG = 1 makes it backwards compatible???? wat

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &resetCommand, 1, 100U);  //reset IC to default
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);

	CAN_IC_WRITE_REGISTER(0x0f, 0x80); //Ensure IC is in configuration mode

	CAN_IC_WRITE_REGISTER(0x2a, CNF1); //configure CNF1
	CAN_IC_WRITE_REGISTER(0x29, CNF2); //configure CNF2
	CAN_IC_WRITE_REGISTER(0x28, CNF3); //configure CNF3

	CAN_IC_WRITE_REGISTER(0x2b, 0xff); //configure interrupts, currently enable error and and wakeup INT
	CAN_IC_WRITE_REGISTER(0x2c, 0x00); //clear INTE flags
									   //this should be a bit-wise clear in any other case to avoid unintentionally clearing flags

	CAN_IC_WRITE_REGISTER(0x0c, 0x0f); //set up RX0BF and RX1BF as interrupt pins

	CAN_IC_WRITE_REGISTER(0x60, 0x60); //accept any message on buffer 0
	CAN_IC_WRITE_REGISTER(0x70, 0x60); //accept any message on buffer 1

	CAN_IC_WRITE_REGISTER(0x0f, 0x04); //Put IC in normal operation mode with CLKOUT pin enable and 1:1 prescaler
}

/**
  * @brief send CAN message
  * @param None
  * @retval None
  */
void sendCANMessage(uint8_t channel, uint16_t ID, uint8_t DLC, uint8_t* data)
{
	uint8_t initialBufferAddress = 0x20 + 16*(channel); //0x30 for channel 1, 0x40 for channel 2, 0x50 for channel 3

	uint8_t sendCommand = 0x81; //instruction to send CAN message on buffer 1

	uint8_t TXBNSIDH = (ID & 0b11111111000) >> 3;
	uint8_t TXBNSIDL = ((ID & 0b111) << 5);
	uint8_t TXBNDLC = DLC & 0x0f;

	CAN_IC_WRITE_REGISTER(initialBufferAddress + 1, TXBNSIDH); // SD 10-3
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 2, TXBNSIDL); //SD 2-0
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 5, TXBNDLC);  //DLC

	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < DLC; i++)
	{
		CAN_IC_WRITE_REGISTER(initialDataBufferAddress + i, data[i]); //write to relevant data registers
	}

	CAN_IC_WRITE_REGISTER_BITWISE(initialBufferAddress, 0x02, 0x02); //set transmit buffer priority to 4 (max)

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &sendCommand, 1, 100U);  //Send command to transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);
}

/**
  * @brief send CAN message with extended identifier
  * @param None
  * @retval None
  */
void sendExtendedCANMessage(uint8_t channel, uint64_t ID, uint8_t DLC, uint8_t* data)
{
	uint8_t initialBufferAddress = 0x30 + 16*(channel); //0x30 for channel 1, 0x40 for channel 2, 0x50 for channel 3

	uint8_t sendCommand = 0x80 +  (1 << channel); //instruction to send CAN message on channel

	uint8_t TXBNEID0 = ID & 0xff;
	uint8_t TXBNEID8 = (ID >> 8) & 0xff;
	uint8_t TXBNSIDL = (((ID >> 18) & 0x07) << 5) | 0b00001000 | ((ID >> 16) & 0x03);
	uint8_t TXBNSIDH = (ID >> 21) & 0xff;
	uint8_t TXBNDLC = DLC & 0x0f;

	CAN_IC_WRITE_REGISTER(initialBufferAddress + 1, TXBNSIDH); // SD 10-3
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 2, TXBNSIDL); //SD 2-0, ED 17-16
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 3, TXBNEID8); //ED 15-8
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 4, TXBNEID0); //ED 7-0
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 5, TXBNDLC);  //DLC

	CAN_IC_READ_REGISTER(initialBufferAddress + 1, &TXBNSIDH); // SD 10-3
	CAN_IC_READ_REGISTER(initialBufferAddress + 2, &TXBNSIDL); //SD 2-0, ED 17-16
	CAN_IC_READ_REGISTER(initialBufferAddress + 3, &TXBNEID8); //ED 15-8
	CAN_IC_READ_REGISTER(initialBufferAddress + 4, &TXBNEID0); //ED 7-0
	CAN_IC_READ_REGISTER(initialBufferAddress + 5, &TXBNDLC);  //DLC

	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < DLC; i++)
	{
		CAN_IC_WRITE_REGISTER(initialDataBufferAddress + i, data[i]); //write to relevant data registers
	}

	CAN_IC_WRITE_REGISTER_BITWISE(initialBufferAddress, 0x02, 0x02); //set transmit buffer priority to 4 (max)

	uint8_t status;
	CAN_IC_READ_REGISTER(initialBufferAddress, &status);
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &sendCommand, 1, 100U);  //Send command to transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);
	CAN_IC_READ_REGISTER(initialBufferAddress, &status);
}



/**
  * @brief Receive CAN message
  * @param None
  * @retval None
  */
void receiveCANMessage(uint8_t channel, uint32_t* ID, uint8_t* DLC, uint8_t* data)
{
	uint8_t initialBufferAddress = 0x60 + 16*(channel); //0x60 for channel 1, 0x70 for channel 2

	uint8_t RXBNSIDH = 0;
	uint8_t RXBNSIDL = 0;
	uint8_t RXBDLC = 0;

	CAN_IC_READ_REGISTER(initialBufferAddress + 1, &RXBNSIDH); // SD 10-3
	CAN_IC_READ_REGISTER(initialBufferAddress + 2, &RXBNSIDL); //SD 2-0, IDE, ED 17-16
	CAN_IC_READ_REGISTER(initialBufferAddress + 5, &RXBDLC); //DLC

	if(RXBNSIDL & 0x08)
	{
		uint8_t RXBNEID8 = 0;
		uint8_t RXBNEID0 = 0;

		CAN_IC_READ_REGISTER(initialBufferAddress + 3, &RXBNEID8); //ED 15-8
		CAN_IC_READ_REGISTER(initialBufferAddress + 4, &RXBNEID0); //ED 7-0

		*ID = (RXBNSIDH << 21) | (((RXBNSIDL >> 5) & 0x07) << 18) | ((RXBNSIDL & 0x03) << 16) | (RXBNEID8 << 8) | (RXBNEID0);
	} else
	{
		*ID = (RXBNSIDH << 3) | (RXBNSIDL >> 5);
	}

	*DLC = RXBDLC & 0x0f;
	if(*DLC > 8){
		*DLC = 8;
	}

	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < *DLC; i++)
	{
		CAN_IC_READ_REGISTER(initialDataBufferAddress + i, (data++)); //read from relevant data registers
	}

	CAN_IC_WRITE_REGISTER_BITWISE(0x2c, 1 << channel, 0); //clear interrupts

	return;
}

void CANRXInterruptTask(void* arg)
{
	uint16_t GPIO_Pin = 0;
	for(;;)
	{
		//osMessageQueueGet(CANInterruptQueue, &GPIO_Pin, 0, osWaitForever);

		while(canReceive == 0) {}
		canReceive = 0;
		GPIO_Pin = CAN_RX0BF_Pin;

		uint32_t ID = 0;
		uint8_t DLC = 0;
		uint8_t data[8] = {0};

		if (osMutexWait(SPIMutexHandle, 0) == osOK)
		{
			if(GPIO_Pin == CAN_RX0BF_Pin)
			{
				receiveCANMessage(0, &ID, &DLC, data);
			}
			else
			{
				receiveCANMessage(1, &ID, &DLC, data);
			}
			osMutexRelease(SPIMutexHandle);
		}
	}
}
