/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"											// Microcontroller library
#include "my_headers.h"											// User defined functions
#include "Thread.h"													// Includes threads


int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS
	
  // initialise peripherals here
	Initialise_LEDs_UserButton();	// Initialises LEDs and user button
	Initialise_Accelerometer();	// Initialises accelerometer register and creates connection with X and Y accelerometer
	
	
	// Initialises all threads
	Init_TiltX_Thread();
	Init_TiltY_Thread();
	Init_Button_Thread();
	Init_Button_Pause_Thread();

	
	osKernelStart ();                         // start thread execution 
	
	while(1){};	// Infinite loop
}
