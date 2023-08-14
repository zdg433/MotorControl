#ifndef utilities__H__
#define utilities__H__
#include "nu32dip.h"          

enum mode_t {IDLE, PWM, ITEST, HOLD, TRACK};

//set operating mode
void set_mode(enum mode_t s);

//return the currently in use operating mode
volatile enum mode_t get_mode(void);

//recive the number N of samples to save 
//into the data buffers during the next TRACK or ITEST

//write data to the buffers if N is not yet reached

//send buffer data to client when N samples have been collected
//(for TRACK or ITEST)

#endif // utilities__H__