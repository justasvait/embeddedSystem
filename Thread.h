extern int Init_TiltX_Thread (void); // Initialisation of X accelerometer thread

extern int Init_TiltY_Thread (void); // Initialisation of Y accelerometer thread

extern int Init_Button_Thread(void); // Initialisation of user button thread

extern int Init_Button_Pause_Thread(void); // Initialisation of pause thread

// The main thread functions.
void TiltX_Thread (void const *argument); // X accelerometer thread

void TiltY_Thread (void const *argument); // Y accelerometer thread

void Button_Thread (void const *argument); // user button thread

void Button_Pause_Thread( void const *argument); // pause thread
