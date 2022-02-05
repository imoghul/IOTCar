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
    *tickCounter=0; // max_tick FOR STOP, 0 FOR CONTINUOUS
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
  Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, ticks, END);
}

void Left_Circle(int ticks){
  Drive_Path(LCIRC_RIGHT,LCIRC_LEFT, ticks, END);
}

void Right_Circle(int ticks){
  Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, ticks, END);
}

void Circle(void){
  if (shapeCounter == 0) {
    //strcpy(display_line[0], "  CIRCLE  ");
    //display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, CIRCLE)) shapeCounter++;
  }
  else if(shapeCounter==2){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_LCIRCLE_TICK, START)) shapeCounter++;
  }
  if (shapeCounter==3) shapeCounter = 0 ;
}

void Figure8(void){
  if (shapeCounter == 0) {
    //strcpy(display_line[0], "  FIGURE8 ");
    //display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, FIGURE8)) shapeCounter++;
  }
  else if(shapeCounter==2){
    if (Drive_Path(LCIRC_RIGHT,LCIRC_LEFT, MAX_LCIRCLE_TICK, START)) shapeCounter++;
  }
  if (shapeCounter==3) shapeCounter = 0 ;
}

void Triangle(void){
  if (shapeCounter == 0) {
    //strcpy(display_line[0], " TRIANGLE ");
    //display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 500, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==2){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK/3, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==3){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 1000, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==4){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK/3, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==5){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 1000, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==6){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK/3, TRIANGLE)) shapeCounter++;
  }
  else if(shapeCounter==7){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 500, END)) shapeCounter++;
  }
  if (shapeCounter==8) shapeCounter = 0 ;
}

// delays for a specified time and then switches state to global nextState
// make sure nextState is set to desired vlaue before the end of delay
void delay(int seconds,int cycles){
  //if(stopwatch_seconds == 0 && cycle_count<=1) display_changed = 1;
  if(stopwatch_seconds>=seconds && cycle_count >= cycles) {
    stopwatch_seconds = 0;
    cycle_count = 0;
    state = nextState;
  }
}

void StateMachine(void){
  switch(state){
    case (START):
      //strcpy(display_line[0], "WAITING...");
      //display_changed = 1;
      break;
    case (WAIT):
      delay(1,100);
      //strcpy(display_line[0], "WAITING...");
      break;
    case (ARM):
      wheel_tick = 0;
      right_tick = 0;
      left_tick = 0;
      state = CIRCLE;
      break;
    case (CIRCLE):
      Circle();
      nextState = FIGURE8;
      break;
    case (FIGURE8):
      Figure8();
      nextState = TRIANGLE;
      break;
    case (TRIANGLE):
      Triangle();
      nextState = END;
      break;
    case (END):
      //strcpy(display_line[0], "    END   ");
      //display_changed = 1;
      break;
    default: break;  
  }
}
