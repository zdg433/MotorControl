#ifndef encoder__H__
#define encoder__H__
#include "nu32dip.h"          


volatile int check1;

//setup function
void UART2_Startup(void);

//read encoder in encoder counts
int get_encoder_count(void);

//read encoder in degrees
float get_encoder_degrees(void);

//reset encoder to 0 degrees
void reset_encoder(void); //haven't tested!!!

int get_encoder_flag(void);

void set_encoder_flag(int set);

void WriteUART2(const char * string);





#endif // encoder__H__