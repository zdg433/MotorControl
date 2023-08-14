#ifndef positioncontrol__H__
#define positioncontrol__H__
#include "nu32dip.h"    

#define ARRAYLENGTH 5000 //extra long array length because I'm too short on time to look into arraylists

float pos_ref_array[ARRAYLENGTH];
float angle_array[ARRAYLENGTH];

//setup function
void position_control_setup(void);

//load trajectory from client

//load position control gains from client
void set_position_gains(float kp, float ki, float kd);

//send position control gains to client
void get_position_gains(float *kp, float *ki, float *kd);

void set_N(int n);

int get_N(void);

void set_angle(float ang);

void set_eint(void);

#endif // positioncontrol__H__