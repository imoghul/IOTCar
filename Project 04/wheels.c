#include "msp430.h"
#include "ports.h"
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;

void RunMotor(int pinForward, int val){
  if (val>0){
    //P6OUT &= ~pinReverse;
    //P6OUT &= ~pinReverse;
    P6OUT |= pinForward;
    P6OUT |= pinForward;
  }
  else if (val==0){
    P6OUT &= ~pinForward;
    P6OUT &= ~pinForward;
    //P6OUT &= ~pinReverse;
    //P6OUT &= ~pinReverse;
  }
  else{
    P6OUT &= ~pinForward;
    P6OUT &= ~pinForward;
    //P6OUT |= pinReverse;
    //P6OUT |= pinReverse;
  }
}


void RunMotors(void){
    
  if (stopwatch_seconds==6 && cycle_count==0){
    RunMotor(R_FORWARD,0);
    RunMotor(L_FORWARD,0);
  }
  else if (stopwatch_seconds==1 && cycle_count==0){
    RunMotor(R_FORWARD,1);
    RunMotor(L_FORWARD,1);
  }
    
    
}