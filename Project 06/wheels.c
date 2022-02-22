#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include <string.h>

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
volatile unsigned int wheel_periods;
volatile char state = START;
volatile int shapeCounter;
volatile char nextState = LINEFOLLOW;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime;
extern volatile unsigned int ADC_Left_Detect,ADC_Right_Detect;

void ShutoffMotors(void){
  P6OUT &= ~R_FORWARD;
  P6OUT &= ~L_FORWARD;
  P6OUT &= ~R_REVERSE;
  P6OUT &= ~L_REVERSE_2355;
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
}

void MotorSafety(void){
  if ((((P6IN & R_FORWARD)!=0 && (P6IN & R_REVERSE)!=0) || ((P6IN & L_FORWARD)!=0 && (P6IN & L_REVERSE_2355)!=0))
      ||
        ((RIGHT_FORWARD_SPEED!=0 && RIGHT_REVERSE_SPEED!=0) || (LEFT_FORWARD_SPEED!=0 && LEFT_REVERSE_SPEED!=0))){
    ShutoffMotors();
    P1OUT |= RED_LED;
  }
  else{
    P1OUT &= ~RED_LED;
  }
}

void RunMotor(unsigned short volatile* forwardPin, unsigned short volatile* reversePin, int val){
  //ShutoffMotors();
  if (val>0){
    *reversePin = WHEEL_OFF;
    *forwardPin = val;
  }
  else if (val==0){
    *forwardPin = WHEEL_OFF;
    *reversePin = WHEEL_OFF;
  }
  else{
    *forwardPin = WHEEL_OFF;
    *reversePin = -val;
  }
  MotorSafety();
}

int Update_Ticks(int max_tick){
  if(++wheel_periods>max_tick){
    wheel_periods=0; 
    return 1;
  }
  return 0;
}

int Drive_Path(int speedR, int speedL, int ticksDuration){
  if (time_change){
    time_change = 0;
    RunMotor(&RIGHT_FORWARD_SPEED,&RIGHT_REVERSE_SPEED,speedR);//RunRightMotor(speedR);
    RunMotor(&LEFT_FORWARD_SPEED,&LEFT_REVERSE_SPEED,speedL);//RunLeftMotor(speedL);
    if (Update_Ticks(ticksDuration)){
      ShutoffMotors();
      return 1;
    }
  }
  return 0;
}

void LineFollow(void){

  if(ADC_Left_Detect >= LEFT_LINE_DETECT && ADC_Right_Detect >= RIGHT_LINE_DETECT){
    Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT,0);
  }
  else if(ADC_Left_Detect >= LEFT_LINE_DETECT){ // left detected so rcirc
    Drive_Path(RCIRC_RIGHT,RCIRC_LEFT,0);
  }
  else if (ADC_Right_Detect >= RIGHT_LINE_DETECT){
    Drive_Path(LCIRC_RIGHT,LCIRC_LEFT,0);
  }
  else {
    Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT,0);
  }

}

// delays for a specified time and then switches state to global nextState
// make sure nextState is set to desired vlaue before the end of delay
void delay(int seconds,int cycles){
  if(stopwatch_seconds == 0 && cycle_count<=1) {
    strcpy(display_line[0], "WAITING...");
    display_changed = 1;
  }
  if(stopwatch_seconds>=seconds && cycle_count >= cycles) {
    stopwatch_seconds = 0;
    cycle_count = 0;
    state = nextState;
  }
}



void StateMachine(void){
  switch(state){
    case (START):
      strcpy(display_line[0], "WAITING...");
      display_changed = 1;
      stopwatch_seconds = 0;
      cycle_count = 0;
      state = WAIT;
      break;
    case (WAIT):
      delay(3,0);
      break;
    case (LINEFOLLOW):
      LineFollow();
      break;
    case (END):
      strcpy(display_line[0], "    END   ");
      display_changed = 1;
      break;
    default: break;  
  }
}
