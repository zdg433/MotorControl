#include "nu32dip.h"          // constants, funcs for startup and UART
#include "utilities.h"

volatile enum mode_t mode;

// function content in the utilities c file:
volatile enum mode_t get_mode(void){
	return mode;
}
void set_mode(enum mode_t s){
	mode = s;
}


