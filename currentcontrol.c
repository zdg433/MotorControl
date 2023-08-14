#include "nu32dip.h"          // constants, funcs for startup and UART
#include <stdlib.h>
#include "currentcontrol.h"
#include "utilities.h"
#include "ina219.h"

#define PH LATAbits.LATA0 
#define ROLLOVER 2399 //2399
#define EINTMAX 100

volatile int set_OC1RS = 0;
volatile int set_PH = 0;
volatile float kp_curr = 0;
volatile float ki_curr = 0;
volatile float ref_curr = 0;
volatile int ref_not_sent = 1;

void __ISR(_TIMER_2_VECTOR, IPL4SOFT) current_controller(void) { // _TIMER_2_VECTOR = 8
  static int curr_count = 0;
  static float e = 0;       // error
  static float current = 0;
  static float u = 0;       // forcing term
  static float unew = 0;
  static float r = 200;       // reference
  static float Eint = 0;
  switch (get_mode()) {
  case 0: { //IDLE
    OC1RS = 0;                       // duty cycle = OC1RS/(PR3+1) = 0%
    set_OC1RS = 0;
    ref_not_sent  = 1;
    //(brake)
    break;
  }
  case 1: { //PWM (user sets this so taking from volatile set_OC1RS which is set through set_duty_cycle())
    PH = set_PH;
    OC1RS = set_OC1RS;
    break;
  }
  case 2: { //ITEST 
    if (curr_count == 0){
        PH = 0;
    }
    if (curr_count % 25 == 0){
        PH = (PH ^ 1);
        r = r * -1;
        Eint = 0;

    }
    current = INA219_read_current();
    e = r - current;
    if (r < 0){
        e = e *-1;
    }
    Eint = Eint + e;
    if (Eint > EINTMAX) { // ADDED: integrator anti-windup
        Eint = EINTMAX;
    } else if (Eint < -EINTMAX) { // ADDED: integrator anti-windup
        Eint = -EINTMAX;
    }
    u = kp_curr * e + ki_curr * Eint;

    unew = u + 40;
    if (unew > 100.0) {
        unew = 100.0;
    } 
    else if (u < 0.0) { 
        unew = 0.0;
    }
    
    OC1RS = (unsigned int) ((unew/100.0) * (ROLLOVER + - 50)); //this gave me so much grief. my motor can not go up to 100% duty cycle for some weird reason, thats why I need -50 term. took me 2 hours to figure this out
    //unsigned int checking = (unsigned int) ((unew/100.0) * (ROLLOVER + 1));

    current_array[curr_count] = current;
    REFarray[curr_count] = r;
    if (curr_count == 99){
        curr_count = 0;
        storingdata = 0;
        Eint = 0;
        set_mode(IDLE);
        r = 200;
    }
    else{
        curr_count++;
    }
    
    break;
  }
  case 3: { //HOLD
    current = INA219_read_current();
    e = ref_curr - current;
    if (ref_curr < 0){
        PH = 1;
        e = e *-1;
    }
    else{
        PH = 0;
    }
    
    Eint = Eint + e;

    if (Eint > EINTMAX) { // ADDED: integrator anti-windup
        Eint = EINTMAX;
    } else if (Eint < -EINTMAX) { // ADDED: integrator anti-windup
        Eint = -EINTMAX;
    }

    u = kp_curr * e + ki_curr * Eint;

    if (u > 100.0) {
        u = 100.0;
    } 
    else if (u < 0.0) { 
        u = 0.0;
    }

    
    
    OC1RS = (unsigned int) ((u/100.0) * (ROLLOVER + - 50)); //this gave me so much grief. my motor can not go up to 100% duty cycle for some weird reason, thats why I need -50 term. took me 2 hours to figure this out
    break;
  }
  case 4: { //TRACK
    current = INA219_read_current();
    e = ref_curr - current;
    if (ref_curr < 0){
        PH = 1;
        e = e *-1;
    }
    else{
        PH = 0;
    }
    
    Eint = Eint + e;

    if (Eint > EINTMAX) { // ADDED: integrator anti-windup
        Eint = EINTMAX;
    } else if (Eint < -EINTMAX) { // ADDED: integrator anti-windup
        Eint = -EINTMAX;
    }

    u = kp_curr * e + ki_curr * Eint;

    if (u > 100.0) {
        u = 100.0;
    } 
    else if (u < 0.0) { 
        u = 0.0;
    }

    
    
    OC1RS = (unsigned int) ((u/100.0) * (ROLLOVER + - 50)); //this gave me so much grief. my motor can not go up to 100% duty cycle for some weird reason, thats why I need -50 term. took me 2 hours to figure this out
    break;

  }
  }
  // insert line to clear interrupt flag
  IFS0bits.T2IF = 0;
}

void currentcontrol_setup(void){
    //configure OC1 and Timer3
    RPB15Rbits.RPB15R = 0b0101;   //use B15 as OC1
    OC1CONbits.OCTSEL = 1;        //use timer3
    T3CONbits.TCKPS = 0;          // Timer3 prescaler N=1 (1:1)
    PR3 = ROLLOVER;                   // period = (PR3+1) * N * 20.833 ns = 50 us, 20 kHz
    TMR3 = 0;                     // initial TMR3 count is 0
    OC1CONbits.OCM = 0b110;       // PWM mode without fault pin; other OC1CON bits are defaults
    OC1RS = 0;                    // duty cycle = OC1RS/(PR3+1) = 0%
    OC1R = 0;                     // initialize before turning OC1 on; afterward it is read-only
    T3CONbits.ON = 1;             // turn on Timer3
    OC1CONbits.ON = 1;            // turn on OC1

    //set A0 as an output and set high
    TRISAbits.TRISA0 = 0;
    PH = 1;

    __builtin_disable_interrupts();
    PR2 = 9599; //should be 9599
    TMR2 = 0;
    T2CONbits.TCKPS = 0;          // set prescaler to 1
    T2CONbits.ON = 1;             // turn on Timer2
    IPC2bits.T2IP = 4;            // INT step 4: priority 4
    IPC2bits.T2IS = 0;            // subpriority 0
    IFS0bits.T2IF = 0;            // INT step 5: clear interrupt flag
    IEC0bits.T2IE = 1;            // INT step 6: enable interrupt
    __builtin_enable_interrupts();
}

void set_duty_cycle(int duty){
    float duty_c = duty/100.0;
    if (duty_c < 0){
        set_PH = 1;
        duty_c = duty_c*(-1);
    }
    else{
        set_PH = 0;
    }
    

    if (duty_c > .02){
        set_OC1RS = (int)(duty_c * (ROLLOVER + 1)) - 50; //for some reason the motor slowed down a lot as OC1RS got really close to PR3
    }
    else
    {
        set_OC1RS = (int)(duty_c * (ROLLOVER + 1));

    }
}

void set_current_gains(float kp, float ki){
    kp_curr = kp;
    ki_curr = ki;
}

void get_current_gains(float *kp, float *ki){
    *kp = kp_curr;
    *ki = ki_curr;
}

void set_ref_curr(float ref){
    ref_curr = ref;
    ref_not_sent = 0;
}
