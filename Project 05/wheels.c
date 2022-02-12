#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include <string.h>

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
volatile unsigned int wheel_tick;
volatile unsigned int wheel_periods;
volatile unsigned int right_tick;
volatile unsigned int left_tick;
volatile char state = START;
volatile int shapeCounter;
volatile char nextState = FORWARD1;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime;

void ShutoffMotors(void){
  P6OUT &= ~R_FORWARD;
  P6OUT &= ~L_FORWARD;
  P6OUT &= ~R_REVERSE;
  P6OUT &= ~L_REVERSE_2355;
}

void MotorSafety(void){
  if (((P6IN & R_FORWARD) && (P6IN & R_REVERSE)) || ((P6IN & L_FORWARD) && (P6IN & L_REVERSE_2355))){
    ShutoffMotors();
    P1OUT |= RED_LED;
  }
  else{
    P1OUT &= ~RED_LED;
  }
}

void RunMotor(int pinForward, int pinReverse, volatile unsigned int* tick, int tick_count, int val){
  //ShutoffMotors();
  if((*tick)++ >= tick_count){
    P6OUT &= ~pinForward;
    P6OUT &= ~pinReverse;
    return;
  }
  if (val>0){
    P6OUT &= ~pinReverse;
    P6OUT |= pinForward;
  }
  else if (val==0){
    P6OUT &= ~pinForward;
    P6OUT &= ~pinReverse;
  }
  else{
    P6OUT &= ~pinForward;
    P6OUT |= pinReverse;
  }
  //MotorSafety();
}

int Update_Ticks(int max_tick){
    if(wheel_tick>=WHEEL_TICK){
        wheel_tick = 0;
        right_tick = 0;
        left_tick = 0;
        wheel_periods++;
      }
      if(wheel_periods>max_tick){
        wheel_periods=0; // max_tick FOR STOP, 0 FOR CONTINUOUS
        return 1;
      }
 
  return 0;
}

int Drive_Path(int right_ticks, int left_ticks, int max_ticks, int polarityr, int polarityl/*, char endState*/){
  if (time_change){
    time_change = 0;
    wheel_tick++;
    RunMotor(R_FORWARD,R_REVERSE,&right_tick,right_ticks,(wheel_periods<max_ticks) * polarityr);
    RunMotor(L_FORWARD,L_REVERSE_2355,&left_tick,left_ticks,(wheel_periods<max_ticks) * polarityl);
    if (Update_Ticks(max_ticks)){
      //state = endState;
      ShutoffMotors();
      return 1;
    }
  }
  return 0;
}

int Drive_Straight(int ticks, int polarity){
  return Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, ticks, polarity, polarity);
}


void Forward(int polarity, int ticks, const char * disp){
  if (shapeCounter == 0) {
    strcpy(display_line[0], disp);
    display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1){
    if (Drive_Straight(ticks,polarity)) shapeCounter++;
  }
  if (shapeCounter==2) {
    state = START;
    shapeCounter = 0;
  }
}
void Spin(int direction, int ticks, const char * disp){
  if (shapeCounter == 0) {
    strcpy(display_line[0], disp);
    display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, ticks,direction,-direction)) shapeCounter++;
  }
  if (shapeCounter==2) {
    state = direction == SPIN_CK?START:ARM;
    shapeCounter = 0;
  }
}

// delays for a specified time and then switches state to global nextState
// make sure nextState is set to desired vlaue before the end of delay
void delay(int seconds,int cycles){
  if(stopwatch_seconds == 0 && cycle_count<=1) display_changed = 1;
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
      delayTime = 1;
      break;
    case (ARM):
      stopwatch_seconds = 0;
      cycle_count = 0;
      state = WAIT;
      break;
    case (WAIT):
      delay(delayTime,0);
      strcpy(display_line[0], "WAITING...");
      break;
    case (FORWARD1):
      Forward(1, ONESEC_STRAIGHT, " FORWARD  ");
      nextState = REVERSE;
      delayTime = 1;
      break;
    case (REVERSE):
      Forward(-1, TWOSEC_STRAIGHT, " REVERSE  ");
      nextState = FORWARD2;
      break;
    case (FORWARD2):
      Forward(1, ONESEC_STRAIGHT, " FORWARD  ");
      nextState = SPINCK;
      delayTime = 1;
      break;
    case (SPINCK):
      Spin(SPIN_CK, SPINR_TICKS,"  SPINCK  ");
      nextState = SPINCCK;
      delayTime = 2;
      break;
    case (SPINCCK):
      Spin(SPIN_CCK, SPINL_TICKS," SPINCCK  ");
      nextState = END;
      delayTime = 2;
      break;
    case (END):
      strcpy(display_line[0], "    END   ");
      display_changed = 1;
      break;
    default: break;  
  }
}
