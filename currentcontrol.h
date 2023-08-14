#ifndef currentcontrol__H__
#define currentcontrol__H__
#include "nu32dip.h"          
#define NUMPOINTS 100

volatile float check;
volatile int storingdata;
volatile float current_array[NUMPOINTS];
volatile int REFarray[NUMPOINTS];

//setup function
void currentcontrol_setup(void);

//receive PWM command. range: [-100 100]
void set_duty_cycle(int duty);

//receive desired current (from position control module)

//get current control gains
void get_current_gains(float *kp, float *ki);

//set current control gains
void set_current_gains(float kp, float ki);

void set_ref_curr(float ref);


#endif // currentcontrol__H__