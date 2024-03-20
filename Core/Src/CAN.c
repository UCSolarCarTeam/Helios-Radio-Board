#include "CAN.h"

// defined this so I could say "for ever" in an  infinite for loop
#define ever (;;)

//TODO: ADD A FUNCTION THAT CHECKS FOR WHAT CHANNELS AVAILABLE FOR SENDING AND
//TODO: A FUNCTION THAT CALLS THE ABOVE FUNCTION AND THE RELEVANT SENDCANMESSAGE FUNCTION
//		(extended vs regular CAN) Done (tbc)

//TODO: Create CanTxGatekeeperTask (so only one task can access the SPI-CAN peripheral at once)
//		Test it by creating the freertos task handle

//TODO: Test receiving interrupt and task (tbd)

//TODO: Once a CanTxGatekeeperTask is added, a mutex will be needed to avoid race condition
//		of the SPI peripheral between CANTxGatekeeper and CANRxInterruptTask

//TODO: Comment the heck out of everything (tbc)

//TODO: write a how to use README that includes creating CANRXInterruptTask,
//		mutex and queue as well as adding CAN.C and CAN.h (tbd)

// TODO: Check available channels, if not there add it to queue
// if theres time ^

/*-------------------------------SPI interface instructions-------------------------------*/

/**
 * @brief write to registry in CAN IC       //FIXME: is this read or write...
 * @param address: hex address of the register
 * 		  bufffer: to store value read
 * @retval None
 */
void CAN_IC_READ_REGISTER(uint8_t address, uint8_t* buffer)
{
	// Packet includes 3 bytes
	// 1st byte: 0x03 (specifies as read instruction)
	// 2nd byte: address of register to read
	// 3rd byte: dont care byte
	uint8_t packet[3] = {0x03, address, 0x00};

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); // Initialize instruction by setting CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 2, 100U); //transmit
	HAL_SPI_Receive(&hspi1, buffer, 1, 100U); //receive register contents
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET); // Terminate instruction by setting CS pin high
}

/**
 * @brief write to registry in CAN IC
 * @param address: hex address of the register
 * 		  value: value to be written to the register
 * @retval None
 */
void CAN_IC_WRITE_REGISTER(uint8_t address, uint8_t value)
{
	// Packet includes 3 bytes
	// 1st byte: 0x02 (specifies as write instruction)
	// 2nd byte: address of register to write to
	// 3rd byte: value to write
	uint8_t packet[3] = {0x02, address, value};

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); //set CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 3, 100U);	//transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET); //set CS pin high
}

/**
 * @brief write to a specific series of bits in a register in CAN IC
 * @param address: hex address of the register
 * 		  mask: bit mask
 * 		  value: value to be written to the register
 * @retval None
 */
void CAN_IC_WRITE_REGISTER_BITWISE(uint8_t address, uint8_t mask, uint8_t value)
{
	// 0x05 specifies bit-write instruction
	// mask specifies which bits can be modified (1 means bit can be modified)
	uint8_t packet[4] = {0x05, address, mask, value};
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET); //set CS pin low
	HAL_SPI_Transmit(&hspi1, packet, 4, 100U); //transmit
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
	uint8_t CONFIG_CNF1 = 0x00; //BRP = 0 to make tq = 250ns and a SJW of 1Tq
	uint8_t CONFIG_CNF2 = 0xd8; //PRSEG = 0, PHSEG1 = 3, SAM = 0, BTLMODE = 1
	uint8_t CONFIG_CNF3 = 0x01; //WAFKIL disabled, PHSEG2 = 2 (BTL enabled) but PHSEG = 1 makes it backwards compatible???? wat

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &resetCommand, 1, 100U);  //reset IC to default
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);

	CAN_IC_WRITE_REGISTER(0x0f, 0x80); //Ensure IC is in configuration mode

	CAN_IC_WRITE_REGISTER(CNF1, CONFIG_CNF1); //configure CNF1
	CAN_IC_WRITE_REGISTER(CNF2, CONFIG_CNF2); //configure CNF2
	CAN_IC_WRITE_REGISTER(CNF3, CONFIG_CNF3); //configure CNF3

	CAN_IC_WRITE_REGISTER(CANINTE, 0xff); 	//configure interrupts, currently enable error and and wakeup INT
	CAN_IC_WRITE_REGISTER(CANINTF, 0x00); 	//clear INTE flags
									   		//this should be a bit-wise clear in any other case to avoid unintentionally clearing flags

	CAN_IC_WRITE_REGISTER(0x0c, 0x0f); //set up RX0BF and RX1BF as interrupt pins

	CAN_IC_WRITE_REGISTER(RXB0CTRL, 0x60); //accept any message on buffer 0
	CAN_IC_WRITE_REGISTER(RXB1CTRL, 0x60); //accept any message on buffer 1

	CAN_IC_WRITE_REGISTER(0x0f, 0x04); //Put IC in normal operation mode with CLKOUT pin enable and 1:1 prescaler

	#if CAN_TEST_SETUP
	CAN_IC_WRITE_REGISTER(0x0F, 0x44);	// Put IC in loop-back mode for testing as well as enable CLKOUT pin with 1:1 prescaler
	#endif
}

/*-------------------------------------------------------------------------------------------*/

uint8_t checkAvailableTXChannel()
{
    uint32_t prevWakeTime = xTaskGetTickCount(); 	//Delay is fine if we have a CanTxGatekeeperTask

    for ever
    {
        uint8_t TXB0Status;
        uint8_t TXB1Status;
        uint8_t TXB2Status;

        CAN_IC_READ_REGISTER(TXB0CTRL, &TXB0Status);
        TXB0Status = TXB0Status >> 3; //Not masking out bits

        if (!TXB0Status) {
            return 0;
        }

        CAN_IC_READ_REGISTER(TXB1CTRL, &TXB1Status);
        TXB1Status = TXB1Status >> 3; //Not masking out bits

        if (!TXB1Status) {
            return 1;
        }

        CAN_IC_READ_REGISTER(TXB2CTRL, &TXB2Status);
        TXB2Status = TXB2Status >> 3; //Not masking out bits

        if (!TXB2Status) {
            return 2;
        }

        prevWakeTime += TX_CHANNEL_CHECK_DELAY;
        osDelayUntil(prevWakeTime);

    }
}

//TODO: make sendtxtask and a queue for it like the old mcu
/**
  * @brief send CAN message
  * @param None
  * @retval None
  */
void sendCANMessage(CANMsg *msg)
{
	uint8_t channel = checkAvailableTXChannel();
    uint8_t initialBufferAddress = TXB0CTRL + 16*(channel);

	// yikes
    // osMessageQueueGet(CANTxMessageQueue, msg, NULL, osWaitForever);

	uint8_t sendCommand = 0x80 + (0x01 < channel); 	   //instruction to send CAN message on buffer 1

	uint8_t TXBNSIDH = (msg->ID & 0b11111111000) >> 3; // mask upper ID register (SD 10-3)
	uint8_t TXBNSIDL = (msg->ID & 0b111) << 5; 	   	   // mask lower ID register (SD 2-0)
	uint8_t TXBNDLC = msg->DLC & 0x0F;				   // mask DLC

	// Set Standard Identifier and DLC
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 1, TXBNSIDH); // SD 10-3
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 2, TXBNSIDL); // SD 2-0
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 5, TXBNDLC);  // DLC

	// Set data to registers
	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < msg->DLC; i++)
	{
		CAN_IC_WRITE_REGISTER(initialDataBufferAddress + i, msg->data[i]); //write to relevant data registers
	}

	// set transmit buffer priority to 3 (max)
	// write to TXBNCTRL<1:0>
	CAN_IC_WRITE_REGISTER_BITWISE(initialBufferAddress, 0x03, 0x03);

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &sendCommand, 1, 100U);  // Send command to transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);
}

/**
  * @brief send CAN message with extended identifier
  * @param None
  * @retval None
  */
void sendExtendedCANMessage(CANMsg *msg)
{
	// uint8_t initialBufferAddress = TXB0CTRL + 16*(channel); //TXB0CTRL for channel 1, TXB1CTRL for channel 2, TXB2CTRL for channel 3
    uint8_t channel = checkAvailableTXChannel();
	uint8_t initialBufferAddress = TXB0CTRL + 16*(channel);

	// delete this later
    //osMessageQueueGet(CANTxMessageQueue, msg, NULL, osWaitForever);
    //todo: FIX THIS CHANNEL!

	uint8_t sendCommand = 0x80 +  (1 << channel); //instruction to send CAN message on channel

	uint8_t TXBNSIDH = (msg->extendedID >> 21) & 0xFF;
	uint8_t TXBNSIDL = (((msg->extendedID >> 18) & 0x07) << 5) | 0x08 | ((msg->ID >> 16) & 0x03);
	uint8_t TXBNEID8 = (msg->extendedID >> 8) & 0xFF;
	uint8_t TXBNEID0 = msg->extendedID & 0xFF;
	uint8_t TXBNDLC = msg->DLC & 0x0F;

	CAN_IC_WRITE_REGISTER(initialBufferAddress + 1, TXBNSIDH); // SD 10-3
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 2, TXBNSIDL); // SD 2-0, ED 17-16
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 3, TXBNEID8); // ED 15-8
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 4, TXBNEID0); // ED 7-0
	CAN_IC_WRITE_REGISTER(initialBufferAddress + 5, TXBNDLC);  // DLC

	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < msg->DLC; i++)
	{
		CAN_IC_WRITE_REGISTER(initialDataBufferAddress + i, msg->data[i]); //write to relevant data registers
	}

	CAN_IC_WRITE_REGISTER_BITWISE(initialBufferAddress, 0x03, 0x03); //set transmit buffer priority to 3 (max)

	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &sendCommand, 1, 100U);  //Send command to transmit
	HAL_GPIO_WritePin(CAN_CS_GPIO_Port, CAN_CS_Pin, GPIO_PIN_SET);
}

/**
  * @brief Receive CAN message
  * @param None
  * @retval None
  */
void receiveCANMessage(uint8_t channel, uint32_t* ID, uint8_t* DLC, uint8_t* data)
{
	uint8_t initialBufferAddress = RXB0CTRL + 16*(channel); //RXB0CTRL for channel 1, RXB1CTRL for channel 2

	uint8_t RXBNSIDH = 0;
	uint8_t RXBNSIDL = 0;
	uint8_t RXBDLC = 0;

	CAN_IC_READ_REGISTER(initialBufferAddress + 1, &RXBNSIDH); // SD 10-3
	CAN_IC_READ_REGISTER(initialBufferAddress + 2, &RXBNSIDL); //SD 2-0, IDE, ED 17-16
	CAN_IC_READ_REGISTER(initialBufferAddress + 5, &RXBDLC);   //DLC

	if(RXBNSIDL & 0x08)	// Check RXBmSIDL.IDE to verify if CAN message has extended identifier
	{
		uint8_t RXBNEID8 = 0;
		uint8_t RXBNEID0 = 0;

		CAN_IC_READ_REGISTER(initialBufferAddress + 3, &RXBNEID8); //ED 15-8
		CAN_IC_READ_REGISTER(initialBufferAddress + 4, &RXBNEID0); //ED 7-0

		*ID = (RXBNSIDH << 21) | (((RXBNSIDL >> 5) & 0x07) << 18) | ((RXBNSIDL & 0x03) << 16) | (RXBNEID8 << 8) | (RXBNEID0);
	} else // CAN message is standard
	{
		*ID = (RXBNSIDH << 3) | (RXBNSIDL >> 5);
	}

	// Check data length of CAN message
	*DLC = RXBDLC & 0x0F; 
	if(*DLC > 8){
		*DLC = 0;
	}

	uint8_t initialDataBufferAddress = initialBufferAddress + 6;
	for(int i = 0; i < *DLC; i++)
	{
		CAN_IC_READ_REGISTER(initialDataBufferAddress + i, (data++)); //read from relevant data registers
	}

	CAN_IC_WRITE_REGISTER_BITWISE(CANINTF, channel + 1, channel + 1); //clear interrupts
	return;
}

