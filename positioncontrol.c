#include "nu32dip.h"          // constants, funcs for startup and UART
#include "positioncontrol.h"
#include "utilities.h"
#include "encoder.h"
#include "currentcontrol.h"

#define EINTMAX 100


volatile float kp_pos = 0;
volatile float ki_pos = 0;
volatile float kd_pos = 0;
volatile float angle = 0;
volatile float Eint = 0;
int N = 0;




void __ISR(_TIMER_4_VECTOR, IPL6SOFT) position_controller(void) {
  static float e = 0;       // error
  static float eprev = 0;
  static float edot = 0;
  static float u = 0;       // forcing term
  static float unew = 0;
  static float r = 0;       // reference
  static int array_count = 0;
    
  switch (get_mode()) {
  case 3: {
    r = angle;
    e = r - get_encoder_degrees();
    Eint = Eint + e;
    if (Eint > EINTMAX) { // ADDED: integrator anti-windup
        Eint = EINTMAX;
    } else if (Eint < -EINTMAX) {
        Eint = -EINTMAX;
    }
    edot = e - eprev;
    u = kp_pos * e + ki_pos * Eint + kd_pos * edot;
    eprev = e;
    set_ref_curr(u);
    break;
  }
  case 4: {
    r = pos_ref_array[array_count];
    float angle_1 = get_encoder_degrees();
    angle_array[array_count] = angle_1;
    e = r - angle_1;
    Eint = Eint + e;
    if (Eint > EINTMAX) { // ADDED: integrator anti-windup
        Eint = EINTMAX;
    } else if (Eint < -EINTMAX) {
        Eint = -EINTMAX;
    }
    edot = e - eprev;
    u = kp_pos * e + ki_pos * Eint + kd_pos * edot;
    eprev = e;
    array_count++;
    set_ref_curr(u);
    if (array_count == N){
        array_count = 0;
        set_mode(HOLD);
        angle = angle_1;
    }
    break;
  }
  }
  // insert line to clear interrupt flag
  IFS0bits.T4IF = 0;
}


void position_control_setup(void){
        __builtin_disable_interrupts();
    PR4 = 29999; //should be 29999
    TMR4 = 0;
    T4CONbits.TCKPS = 3;          // set prescaler to 8
    T4CONbits.ON = 1;             // turn on Timer4
    IPC4bits.T4IP = 6;            // INT step 4: priority 6
    IPC4bits.T4IS = 0;            // subpriority 0
    IFS0bits.T4IF = 0;            // INT step 5: clear interrupt flag
    IEC0bits.T4IE = 1;            // INT step 6: enable interrupt
    __builtin_enable_interrupts();
}

void set_position_gains(float kp, float ki, float kd){
    kp_pos = kp;
    ki_pos = ki;
    kd_pos = kd;
}

void get_position_gains(float *kp, float *ki, float *kd){
    *kp = kp_pos;
    *ki = ki_pos;
    *kd = kd_pos;
}

void set_eint(void){
    Eint = 0;
}

void set_angle(float ang){
    angle = ang;
}

void set_N(int n){
    N = n;
}

int get_N(void){
    return N;
}
//void set_pos_ref(float arr[]){
//    for(int i = 0; i < N; i++){
//        pos_ref_array[i] = arr[i];
//    }
//}