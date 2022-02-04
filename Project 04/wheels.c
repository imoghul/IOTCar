#include "msp430.h"
#include "ports.h"
#include "wheels.h"

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned int wheel_tick;
volatile unsigned int wheel_periods;
volatile unsigned int right_tick;
volatile unsigned int left_tick;
volatile char state = START;
volatile int figure8count;
volatile char nextState = ARM;

void RunMotor(int pinForward, volatile unsigned int* tick, int tick_count, int val){
  if((*tick)++ >= tick_count){
    P6OUT &= ~pinForward;
    return;
    //P6OUT &= ~pinReverse;
  }
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

int Update_Ticks(volatile unsigned int* tickCounter, int max_tick, char nState){
  if(wheel_tick>=WHEEL_TICK){
    wheel_tick = 0;
    right_tick = 0;
    left_tick = 0;
    (*tickCounter)++;
  }
  if(*tickCounter>max_tick){
    *tickCounter=0;
    state = nState;
    return 1;
  }
  return 0;
}

int Drive_Path(int right_ticks, int left_ticks, int max_ticks, char endState){
  wheel_tick++;
  RunMotor(R_FORWARD,&right_tick,right_ticks,wheel_periods<max_ticks);
  RunMotor(L_FORWARD,&left_tick,left_ticks,wheel_periods<max_ticks);
  return Update_Ticks(&wheel_periods,max_ticks, endState);
}

void Drive_Straight(int ticks){
  //RunMotor(R_FORWARD,&right_tick,STRAIGHT_RIGHT,wheel_periods<ticks);
  //RunMotor(L_FORWARD,&left_tick,STRAIGHT_LEFT,wheel_periods<ticks);
  //Update_Ticks(&wheel_periods,ticks, END);
  Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, ticks, END);
}

void Left_Circle(int ticks){
  //RunMotor(R_FORWARD,&right_tick,LCIRC_RIGHT,wheel_periods<MAX_CIRCLE_TICK);
  //RunMotor(L_FORWARD,&left_tick,LCIRC_LEFT,wheel_periods<MAX_CIRCLE_TICK);
  //Update_Ticks(&wheel_periods,MAX_CIRCLE_TICK, END);
  Drive_Path(LCIRC_RIGHT,LCIRC_LEFT, ticks, END);
}

void Right_Circle(int ticks){
  //RunMotor(R_FORWARD,&right_tick,RCIRC_RIGHT,wheel_periods<MAX_CIRCLE_TICK);
  //RunMotor(L_FORWARD,&left_tick,RCIRC_LEFT,wheel_periods<MAX_CIRCLE_TICK);
  //Update_Ticks(&wheel_periods,MAX_CIRCLE_TICK, END);
  Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, ticks, END);
}

void Figure8(void){
  if(figure8count==0){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, FIGURE8)) figure8count++;
  }
  else if(figure8count==1){
    if (Drive_Path(LCIRC_RIGHT,LCIRC_LEFT, MAX_LCIRCLE_TICK, END)) figure8count++;
  }
}

void delay(int seconds,int cycles){
  if(stopwatch_seconds>=seconds && cycle_count >= cycles) {
    stopwatch_seconds = 0;
    cycle_count = 0;
    state = nextState;
  }
}

void StateMachine(void){
  switch(state){
    case (START):
      stopwatch_seconds = 0;
      cycle_count = 0;
      state = WAIT;
      break;
    case (WAIT):
      delay(1,100);
      break;
    case (ARM):
      wheel_tick = 0;
      right_tick = 0;
      left_tick = 0;
      state = CIRCLE;
      break;
    case (CIRCLE):
      Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, START);
      nextState = FIGURE8;
      break;
    case (FIGURE8):
      Figure8();
      break;
    case (END):
      break;
    default: break;  
  }
}
