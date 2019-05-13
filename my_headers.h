#include "stm32f4xx.h"

// Global variable
extern SPI_HandleTypeDef SPI_Params; // Holds SPI1 parameters. Used in external funcitons

void Initialise_LEDs_UserButton(void); // Initialise the all LEDs and the user button

void intialise_SPI(void); // Initialise SPI

void Blink_LED(uint8_t, uint8_t); // Function to turn on and off specified LED

uint8_t Read_data(uint8_t address); // Function to read data from specified address
