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
volatile char nextState = CIRCLE;
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


void Circle(void){
  if (shapeCounter == 0) {
    strcpy(display_line[0], "  CIRCLE  ");
    display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1 || shapeCounter == 2){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, 1,1)) shapeCounter++;
  }
  if (shapeCounter==3) {
    shapeCounter = 0 ;
    state = START;
  }
}

void Figure8(void){
  if (shapeCounter == 0) {
    strcpy(display_line[0], "  FIGURE8 ");
    display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1 || shapeCounter==3){
    if (Drive_Path(RCIRC_RIGHT,RCIRC_LEFT, MAX_RCIRCLE_TICK, 1,1)) shapeCounter++;
  }
  else if(shapeCounter==2 || shapeCounter==4){
    if (Drive_Path(LCIRC_RIGHT,LCIRC_LEFT, MAX_LCIRCLE_TICK, 1,1)) shapeCounter++;
  }
  if (shapeCounter==5) {
    state = START;
    shapeCounter = 0 ;
  }
}

void Triangle(void){
  if (shapeCounter == 0 || shapeCounter == 6) {
    strcpy(display_line[0], " TRIANGLE ");
    display_changed = 1;
    shapeCounter++;
  }
  if(shapeCounter==1 || shapeCounter == 3 || shapeCounter==5 || shapeCounter==8 || shapeCounter==10 || shapeCounter==12){
    if (Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, TRIANGLE_LEG, 1,1)) shapeCounter++;
  }
  else if(shapeCounter==2 || shapeCounter == 4 || shapeCounter==7 || shapeCounter==9 || shapeCounter==11 || shapeCounter==13){
    if (Drive_Path(TRIANGLE_RIGHT_TICK,TRIANGLE_LEFT_TICK, TRIANGLE_TURN_TICK, 1,1)) shapeCounter++;
  }
  
  if (shapeCounter==14) {
    shapeCounter = 0;
    state = END;
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
      state = WAIT;
      break;
    case (WAIT):
      delay(3,0);
      strcpy(display_line[0], "WAITING...");
      break;
    case (ARM):
      //wheel_tick = 0;
      //right_tick = 0;
      //left_tick = 0;
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
      strcpy(display_line[0], "    END   ");
      display_changed = 1;
      break;
    default: break;  
  }
}
