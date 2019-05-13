#include "stm32f4xx.h"
#include "my_headers.h"

// Global variables
SPI_HandleTypeDef SPI_Params; // Holds SPI1 parameters

// Initialise the all LEDs and user button
void Initialise_LEDs_UserButton(void){

	// Enable clock on port D and set pins 12-15 to output for LEDs
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;	// enable clock
	GPIOD->MODER |= GPIO_MODER_MODER12_0; // green LED
	GPIOD->MODER |= GPIO_MODER_MODER13_0; // orange LED
	GPIOD->MODER |= GPIO_MODER_MODER14_0; // red LED
	GPIOD->MODER |= GPIO_MODER_MODER15_0; // blue LED
	
	// Enable clock on port A for user button
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
}

// Function for turning on and off one of the LEDs
void Blink_LED(uint8_t LED_state, uint8_t LED_ID){

	// Checks user request state: 1-turn on LED, 0-turn off LED.
	if(LED_state == 1){
		GPIOD->BSRR = 1<<LED_ID; // Turn on specified LED	
	}
	else{		
		GPIOD->BSRR = 1<<(LED_ID+16); // Turn off specified LED	
	}
}


// Initialise accelerometer
void Initialise_Accelerometer (void){
	
	// Configure the SPI
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enables the clock for SPI1
	
	SPI_Params.Instance = SPI1; // Choose SPI1 interface
	SPI_Params.Init.Mode = SPI_MODE_MASTER; // Sets microcontroller to master mode
	SPI_Params.Init.NSS = SPI_NSS_SOFT; // Software controlled slave
	SPI_Params.Init.Direction = SPI_DIRECTION_2LINES; // Sets the SPI to full-duplex
	SPI_Params.Init.DataSize = SPI_DATASIZE_8BIT; // Sets SPI data size to 8-bit
	SPI_Params.Init.CLKPolarity = SPI_POLARITY_HIGH; // Sets SPI idle polarity to high
	SPI_Params.Init.CLKPhase = SPI_PHASE_2EDGE; // Sets data transition at second clock edge
	SPI_Params.Init.FirstBit = SPI_FIRSTBIT_MSB; // Sends data starting from MSB
	SPI_Params.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // Sets the clock prescaler to get SPI clock speed < 10MHz (maximum value)
	
	HAL_SPI_Init(&SPI_Params); // Initialises SPI with specified parameters
	
	// Code to initialise pins 5-7 of GPIOA
	GPIO_InitTypeDef GPIOA_Params; // Holds GPIOA parameters
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // Enable GPIOA clock
	GPIOA_Params.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; // Selects pins 5,6 and 7
	GPIOA_Params.Alternate = GPIO_AF5_SPI1; // Sets SPI1 as alternative function
	GPIOA_Params.Mode = GPIO_MODE_AF_PP; // Sets GPIOA to push-pull mode
	GPIOA_Params.Speed = GPIO_SPEED_FAST; // Sets GPIOA to fast speed mode
	GPIOA_Params.Pull = GPIO_NOPULL; // Sets GPIOA to no pull activation
	
	HAL_GPIO_Init(GPIOA, &GPIOA_Params); // Initialises GPIOA with specified parameters
	
	// Code to initialise pin 3 of GPIOE
	GPIO_InitTypeDef GPIOE_Params; // Holds GPIOE parameters
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; //Enable GPIOE clock
	GPIOE_Params.Pin = GPIO_PIN_3; // Selects pin 3
	GPIOE_Params.Mode = GPIO_MODE_OUTPUT_PP; // Sets GPIOE to push-pull mode
	GPIOE_Params.Speed = GPIO_SPEED_FAST; // Sets GPIOE to fast speed mode
	GPIOE_Params.Pull = GPIO_PULLUP; //Selects pull-up activation
	
	HAL_GPIO_Init(GPIOE, &GPIOE_Params); // Initialises GPIOE with specified parameters
	
	// Sets serial port to idle mode and enables SPI
	GPIOE->BSRR |= GPIO_PIN_3;
	__HAL_SPI_ENABLE(&SPI_Params);
	
	
	// Establish communication for X and Y accelerometer
	uint8_t data_to_send[1]; // Array to store register address and communication data
	uint16_t data_size=1; // Specifies data size
	uint32_t data_timeout=1000; // Sets a maximum communication timeout to complete transmission
		
	// write the value from the Register 4 of the LIS3DSH
	data_to_send[0] = 0x20; // CTRL_REG4 register address
	GPIOE->BSRR |= GPIO_PIN_3<<16; // Enables SPI communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send,data_size,data_timeout); // Send the register address to establish connection with the controller
	data_to_send[0] = 0x23; // Sets data variable to select CTRL_REG4 output data rate to 6.25Hz, enables X and Y axis accelerometers.
	HAL_SPI_Transmit(&SPI_Params,data_to_send,data_size,data_timeout); // Sets CTRL_REG4 with specified configurations
	GPIOE->BSRR |= GPIO_PIN_3; // Sets pin 3 to high to indicate end of communication
	
}

uint8_t Read_data(uint8_t address)
{
	uint8_t data_to_send[1]; // Array to store register address and communication data
	uint16_t data_size=1; // Specifies data size
	uint32_t data_timeout=1000; // Sets a maximum communication timeout to complete transmission
	uint8_t output_8bit; // 8-bit variable to store communication data
	
	// Reads data from specified address
	data_to_send[0] = address; // Selects specified address
	GPIOE->BSRR = GPIO_PIN_3<<16; // Enables SPI communication
	HAL_SPI_Transmit(&SPI_Params,data_to_send,data_size,data_timeout); // Sends the register address to establish connection with the controller
	data_to_send[0] = 0x00; // Sets address to read data
	HAL_SPI_Receive(&SPI_Params,data_to_send,data_size,data_timeout); // Sends request of a data
	GPIOE->BSRR = GPIO_PIN_3; // Sets pin 3 to high to indicate end of communication
	output_8bit = *SPI_Params.pRxBuffPtr; // Store data read data to a variable from SPI buffer
	
	return output_8bit;	// Return read data as function output
}
