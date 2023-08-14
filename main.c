#include "nu32dip.h"                      // constants, funcs for startup and UART
#include "utilities.h"
#include "encoder.h"
#include "ina219.h"
#include "currentcontrol.h"
#include "positioncontrol.h"

#define BUF_SIZE 200


//NOTE make sure to put the motor in idle between using TRACK on 2 unique trajectories.


int main(void) {
  set_mode(IDLE);
  char buffer[BUF_SIZE];
  NU32DIP_YELLOW = 1;                     // turn off the LEDs
  NU32DIP_GREEN = 1;
  __builtin_disable_interrupts();
  // in future, initialize modules or peripherals here
  NU32DIP_Startup();                      // cache on, min flash wait, interrupts on, LED/button init, UART init
  UART2_Startup();                        //open communication with pico
  INA219_Startup();
  position_control_setup();
  currentcontrol_setup();
  __builtin_enable_interrupts();
  while(1) {
       
    NU32DIP_ReadUART1(buffer,BUF_SIZE);   // we expect the next character to be a menu command
    NU32DIP_YELLOW = 1;                   // clear the error LED

    switch (buffer[0]) {
    case 'b': {
      float curr;
      if(get_mode() == HOLD || get_mode() == TRACK){
        curr = 0;
      }
      else{
        curr = INA219_read_current();
      }
      sprintf(buffer,"%f\n\r", curr); //save current in mA to buffer
      NU32DIP_WriteUART1(buffer); // send encoder count to client
      break;
    }
    case 'c': {
      sprintf(buffer,"%d\n\r", get_encoder_count());
      NU32DIP_WriteUART1(buffer); // send encoder count to client
      break;
    }
    case 'd': {
      sprintf(buffer,"%f\n\r", get_encoder_degrees());
      NU32DIP_WriteUART1(buffer); // send encoder count to client
      break;
    }
    case 'e': {
      reset_encoder();
      break;
    }
    case 'f': {
      set_mode(PWM);
      int duty_cycle = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%d", &duty_cycle);
      set_duty_cycle(duty_cycle);
      break;
    }
    case 'g': {
      float kp_temp = 0;
      float ki_temp = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &kp_temp);
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &ki_temp);
      set_current_gains(kp_temp,ki_temp);
      break;
    }
    case 'h': {
      float kp_temp;
      float ki_temp;
      get_current_gains(&kp_temp,&ki_temp);
      sprintf(buffer,"%f\n\r", kp_temp);
      NU32DIP_WriteUART1(buffer); // send kp to client
      sprintf(buffer,"%f\n\r", ki_temp);
      NU32DIP_WriteUART1(buffer); // send ki to client
      break;
    }
    case 'i': {
      float kp_temp = 0;
      float ki_temp = 0;
      float kd_temp = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &kp_temp);
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &ki_temp);
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &kd_temp);
      set_position_gains(kp_temp,ki_temp,kd_temp);
      break;
    }
    case 'j': {
      float kp_temp;
      float ki_temp;
      float kd_temp;
      get_position_gains(&kp_temp,&ki_temp,&kd_temp);
      sprintf(buffer,"%f\n\r", kp_temp);
      NU32DIP_WriteUART1(buffer); // send kp to client
      sprintf(buffer,"%f\n\r", ki_temp);
      NU32DIP_WriteUART1(buffer); // send ki to client
      sprintf(buffer,"%f\n\r", kd_temp);
      NU32DIP_WriteUART1(buffer); // send kd to client
      break;
    }
    case 'k': {
      set_mode(ITEST);
      storingdata = 1;
      while (storingdata){
        ;//do nothing 
      }
      sprintf(buffer, "%d\r\n", NUMPOINTS);
      NU32DIP_WriteUART1(buffer);
      int i;
      for (i=0; i<NUMPOINTS; i++) { // send plot data to MATLAB
        // when first number sent = 1, MATLAB knows we’re done
        sprintf(buffer, "%d %f\r\n", REFarray[i], current_array[i]);
        NU32DIP_WriteUART1(buffer);
      } 
    
      break;
    }
    case 'l': {
      set_eint();
      float angle = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%f", &angle);
      set_mode(HOLD);
      set_angle(angle);
      break;
    }
    case 'm': {
      int N = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%d", &N);
      for (int i = 0; i < N; i++)
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &pos_ref_array[i]); //this might need to be int
      }
      
      set_N(N);
      break;
    }
    case 'n': {
      int N = 0;
      NU32DIP_ReadUART1(buffer,BUF_SIZE);
      sscanf(buffer, "%d", &N);
      for (int i = 0; i < N; i++)
      {
        NU32DIP_ReadUART1(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &pos_ref_array[i]); //this might need to be int
      }
      
      set_N(N);

      break;
    }
    case 'o': {
      set_mode(TRACK); 
      while(get_mode() == TRACK){
        ;
      }

      sprintf(buffer, "%d\r\n", get_N());
      NU32DIP_WriteUART1(buffer);
      
      for (int i = 0; i < get_N(); i++) { // send plot data to client
        // when first number sent = 1, client knows we’re done
        sprintf(buffer, "%f %f\r\n", pos_ref_array[i], angle_array[i]);
        NU32DIP_WriteUART1(buffer);
      }



      break;
    }
    case 'p': {
      set_mode(IDLE);
      break;
    }
    case 'q': {
      // handle q for quit. Later you may want to return to IDLE mode here.
      set_mode(IDLE);
      break;
    }
    case 'r': {
      sprintf(buffer, "%d\n\r", get_mode());
      NU32DIP_WriteUART1(buffer); // send mode to client
      break;
    }    
    case 'x': {
      for (int i = 0; i < get_N(); i++) { // send plot data to client
        // when first number sent = 1, client knows we’re done
        sprintf(buffer, "%f\r\n", pos_ref_array[i]);
        NU32DIP_WriteUART1(buffer);
      }
    }
    default: {
      NU32DIP_YELLOW = 0;                 // turn on LED2 to indicate an error
      break;
    }
    }
  }
    //OC1 is B15
    //A0 is IO pin for the H-bridge
  return 0;
}
