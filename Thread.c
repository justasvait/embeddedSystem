#include "stm32f4xx.h"
#include <cmsis_os.h>                                           // CMSIS RTOS header file
#include "my_headers.h"

// Creates definitions for LED control
#define LED_ON 1			// LED_state: turn on LED
#define LED_OFF  0		// LED_state: turn off LED
#define LED_GREEN 12 	// LED_ID: green LED
#define LED_ORANGE 13 // LED_ID: orange LED
#define LED_RED 14 		// LED_ID: red LED
#define LED_BLUE 15 	// LED_ID: blue LED

// Thread Declarations
void TiltX_Thread (void const *argument);                             // thread function
osThreadId tid_TiltX_Thread;                                          // thread id
osThreadDef (TiltX_Thread, osPriorityNormal, 1, 0);                   // thread object

void TiltY_Thread (void const *argument);                             // thread function
osThreadId tid_TiltY_Thread;                                          // thread id
osThreadDef (TiltY_Thread, osPriorityNormal, 1, 0);                   // thread object

void Button_Thread (void const *argument);														// thread function
osThreadId tid_Button_Thread;																					// thread id
osThreadDef (Button_Thread, osPriorityNormal, 1, 0); 									// thread object

void Button_Pause_Thread (void const *argument); 											// thread function
osThreadId tid_Button_Pause_Thread; 																	// thread id
osThreadDef (Button_Pause_Thread, osPriorityNormal, 1, 0);						// thread object


/*----------------------------------------------------------------------------
 *      Thread 1 'TiltX_Thread': Turns the LED that is tilted down in X axis.
 *---------------------------------------------------------------------------*/

// Initialises TiltX_Thread
int Init_TiltX_Thread (void) {

  tid_TiltX_Thread = osThreadCreate (osThread(TiltX_Thread), NULL);
  if(!tid_TiltX_Thread) return(-1);
  
  return(0);
}

// TiltX_Thread function
void TiltX_Thread (void const *argument) {
	
	uint8_t address = 0x80|0x29;	// Address of X accelerometer higher 8-bit value
	uint8_t X_Reg_H;	// Variable to store accelerometer output value
	
	osSignalSet(tid_TiltX_Thread,0x01); // Sets TiltX_Thread flag to 0x01 to resume the thread after wait function
	
  while (1) {
		osSignalWait(0x01,osWaitForever);	// Pauses the TiltX_Thread until its flag is set to 0x01
		osSignalSet(tid_TiltX_Thread,0x01); // Sets TiltX_Thread flag to 0x01 to resume the thread after wait function

		// Read X accelerometer higher 8-bit value
		X_Reg_H = Read_data(address);
		
		if ( (X_Reg_H&0x70) != 0x0){ // Checks if the accelerometer value is within threshold which are 5-7 higher 8-bit register values
			if ( (X_Reg_H&0x80) == 0x80){	// Checks the sign of accelerometer value to dtermine which LED to light on
				
				// Turn on green LED and turn off red LED
				Blink_LED(LED_ON, LED_GREEN);
				Blink_LED(LED_OFF, LED_RED);
			}
			else{
				
				// Turn on red LED and turn off green LED
				Blink_LED(LED_ON, LED_RED);
				Blink_LED(LED_OFF, LED_GREEN);
			}
		}
		else{ // The accelerometer values are out of threshold range, thus no LEDs are on
			
			// Turns off red and green LEDs
			Blink_LED(LED_OFF, LED_GREEN);
			Blink_LED(LED_OFF, LED_RED);
		}
		
    osThreadYield();                                            // suspend thread
  }
}


/*----------------------------------------------------------------------------
 *      Thread 2 'TiltY_Thread': Turns the LED that is tilted down in Y axis.
 *---------------------------------------------------------------------------*/

// Initialise TiltY_Thread
int Init_TiltY_Thread (void) {

  tid_TiltY_Thread = osThreadCreate (osThread(TiltY_Thread), NULL);
  if(!tid_TiltY_Thread) return(-1);
  
  return(0);
}

// TiltY_Thread function
void TiltY_Thread (void const *argument) {
	
	uint8_t address = 0x80|0x2B; // Address of Y accelerometer higher 8-bit value
	uint8_t Y_Reg_H;	// Variable to store accelerometer output value
	
	osSignalSet(tid_TiltY_Thread,0x01); // Sets TiltY_Thread flag to 0x01 to resume the thread after wait function
	
  while (1) {
		osSignalWait(0x01,osWaitForever); // Pauses the TiltY_Thread until its flag is set to 0x01
		osSignalSet(tid_TiltY_Thread,0x01); // Sets TiltY_Thread flag to 0x01 to resume the thread after wait function
		
		Y_Reg_H = Read_data(address); // Read Y accelerometer higher 8-bit value
		
		if ( (Y_Reg_H&0x70) != 0x0){ // Checks if the accelerometer value is within threshold which are 5-7 higher 8-bit register values
			if ( (Y_Reg_H&0x80) == 0x80){ // Checks the sign of accelerometer value to dtermine which LED to light on
				
				// Turn on blue LED and turn off orange LED
				Blink_LED(LED_OFF, LED_ORANGE);
				Blink_LED(LED_ON, LED_BLUE);
			}
			else{ 
				
				// Turn on orange LED and turn off blue LED
				Blink_LED(LED_OFF, LED_BLUE);
				Blink_LED(LED_ON, LED_ORANGE);
			}
		}
		else{ // The accelerometer values are out of threshold range, thus no LEDs are on
			
			// Turns off orange and blue LEDs
			Blink_LED(LED_OFF, LED_ORANGE);
			Blink_LED(LED_OFF, LED_BLUE);
		}
		
    osThreadYield();                                            // suspend thread
  }
}


/*-------------------------------------------------------------------------
*      Thread 3 'Button_Thread': Switches between 2 modes: accelerometer X
*																 and Y tilt indication mode and pause mode.
*-----------------------------------------------------------------------*/

// Initialise Button_Thread
int Init_Button_Thread (void) {
	tid_Button_Thread = osThreadCreate (osThread(Button_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
	if(!tid_Button_Thread) return(-1); // Checks to make sure the thread has been created.
	return(0);
}
// Button_Thread function
void Button_Thread (void const *argument) {
	
	uint8_t button_state = 0; // Stores button state: 0- unpressed 1- pressed
	uint8_t pause_state = 0;	// Stores pause state: 0- unpause 1- paused
	
	while (1) { // Infinite loop
		
	// Checks user button state with a 1 milisecond delay to ensure button was properly pressed
	if(((GPIOA->IDR & 0x00000001) == 0x00000001) ){
		osDelay(1);	// 1 milisecond delay
		if(((GPIOA->IDR & 0x00000001) == 0x00000001) ){	// Re-check user button state
			button_state = 1;	// Changes button state to 1
		}
	}
	
	// Checks if button was previously pressed and now is unpressed
	if(((GPIOA->IDR & 0x00000001) != 0x00000001) && (button_state==1) ){
		
		button_state = 0;	// Changes button state to 0
	
		// Turn off all LEDs before going to pause mode
		Blink_LED(LED_OFF,LED_GREEN);
		Blink_LED(LED_OFF,LED_ORANGE);
		Blink_LED(LED_OFF,LED_RED);
		Blink_LED(LED_OFF,LED_BLUE);
		
		// Checks to which state go to after user button is pressed and released
		if (pause_state==0){
			
			pause_state = 1; // Change pause state to 1
			
			// Stop accelerometer threads and go to pause thread
			osSignalClear(tid_TiltX_Thread,0x01); // Clear TiltX_Thread flag 0x01 to pause thread
			osSignalClear(tid_TiltY_Thread,0x01); // Clear TiltY_Thread flag 0x01 to pause thread
			
			osSignalSet(tid_Button_Pause_Thread,0x01); // Sets Button_Pause_Thread flag to 0x01 to resume the thread after wait function
		}
		else{
			pause_state = 0; // Change pause state to 0
			
			// Resume accelerometer threads and pause Button_Pause_Thread
			osSignalSet(tid_TiltX_Thread,0x01); // Sets TiltX_Thread flag to 0x01 to resume the thread after wait function
			osSignalSet(tid_TiltY_Thread,0x01); // Sets TiltY_Thread flag to 0x01 to resume the thread after wait function
			osSignalClear(tid_Button_Pause_Thread,0x01); // Clear Button_Pause_Thread flag 0x01 to pause thread
		}
	}
	osThreadYield(); 																							// suspend thread
	}
}


/*-------------------------------------------------------------------------
*      Thread 4 'Button_Pause_Thread': Pause mode. Flashes red LED on and
*																			 off with 1 second intervals.
*-----------------------------------------------------------------------*/

// Initialise Button_Pause_Thread
int Init_Button_Pause_Thread (void) {
	tid_Button_Pause_Thread = osThreadCreate (osThread(Button_Pause_Thread), NULL); // Creates the main thread object that we have declared and assigns it the thread ID that we have declared.
	if(!tid_Button_Pause_Thread) return(-1); // Checks to make sure the thread has been created.
	return(0);
}
// Button_Pause_Thread function
void Button_Pause_Thread (void const *argument) {
			
	while (1) { // Infinite loop
		
		osSignalWait(0x01,osWaitForever); // Pauses the Button_Pause_Thread until its flag is set to 0x01
		osSignalSet(tid_Button_Pause_Thread,0x01); // Sets Button_Pause_Thread flag to 0x01 to resume the thread after wait function
		
		// Turn red LED on and keep it for 1 second, then turn of of for 1 additional second
		Blink_LED(LED_ON, LED_RED); // Turn on red LED
		osDelay(1000); // 1 second delay
			
		Blink_LED(LED_OFF, LED_RED); // Turn off red LED
		osDelay(1000); // 1 second delay
		
		osThreadYield(); 																							// suspend thread
	}
}


