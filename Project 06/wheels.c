#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "timers.h"
#include <string.h>

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
volatile unsigned int wheel_periods;
volatile char state = START;
volatile int stateCounter;
volatile char nextState = STRAIGHT;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime = 1;
extern volatile unsigned int ADC_Left_Detect,ADC_Right_Detect;
volatile unsigned int rightSwitchable=1, leftSwitchable=1;

void ShutoffMotors(void){
  ShutoffRight();
  ShutoffLeft();
}

void ShutoffRight(void){
  rightSwitchable = 0;
  RIGHT_FORWARD_SPEED = WHEEL_OFF;
  RIGHT_REVERSE_SPEED = WHEEL_OFF;
  TB1CCTL1 |= CCIE; 
  TB1CCTL1 &= ~CCIFG;
  TB1CCR1 = TB1R + TB1CCR1_INTERVAL;
}

void ShutoffLeft(void){
  leftSwitchable = 0;
  LEFT_FORWARD_SPEED = WHEEL_OFF;
  LEFT_REVERSE_SPEED = WHEEL_OFF;
  TB1CCTL2 |= CCIE; 
  TB1CCTL2 &= ~CCIFG;
  TB1CCR2 = TB1R + TB1CCR2_INTERVAL;
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


int RunRightMotor(int val){
  if(RIGHT_REVERSE_SPEED>0 && val>0 || RIGHT_FORWARD_SPEED>0 && val<0){
    ShutoffRight();
  }
  //ShutoffMotors();
  if (val>0){
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    if(rightSwitchable) {RIGHT_FORWARD_SPEED = val;
    return 1;}
    else return 0;
  }
  else if (val==0){
    ShutoffRight();
    return 1;
  }
  else{
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    if(rightSwitchable) {RIGHT_REVERSE_SPEED = -val; return 1;}
    else return 0;
  }
  //MotorSafety();
}

int RunLeftMotor(int val){
  if(LEFT_REVERSE_SPEED>0 && val>0 || LEFT_FORWARD_SPEED>0 && val<0){
    ShutoffLeft();
  }
  
  if (val>0){
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    if(leftSwitchable) {LEFT_FORWARD_SPEED = val;return 1;}
    else return 0;
  }
  else if (val==0){
    ShutoffLeft();
    return 1;
  }
  else{
    LEFT_FORWARD_SPEED = WHEEL_OFF;
    if(leftSwitchable) {LEFT_REVERSE_SPEED = -val;return 1;}
    else return 0;
  }
  //MotorSafety();
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
    RunRightMotor(speedR);
    RunLeftMotor(speedL);
    if(ticksDuration == 0) return 1;
    if (Update_Ticks(ticksDuration)){
      ShutoffMotors();
      return 1;
    }
  }
  return 0;
}


void Straight(void){
  if (stateCounter == 0) {
    strcpy(display_line[0], "SEARCHING ");
    display_changed = 1;
    stateCounter++;
  }
  if(stateCounter==1){
    if ((ADC_Left_Detect <= LEFT_LINE_DETECT && ADC_Right_Detect <= RIGHT_LINE_DETECT)){
      Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 0);
    }
    else{
      stateCounter++;
    }
  }
  if(stateCounter==2){
    if(Drive_Path(-STRAIGHT_RIGHT,-STRAIGHT_LEFT, 10)) stateCounter++;
  }
  if (stateCounter==3) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = START;
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
      break;
    case (WAIT):
      delay(delayTime,0);
      break;
    case (STRAIGHT):
      Straight();
      break;
    case (END):
      strcpy(display_line[0], "    END   ");
      display_changed = 1;
      break;
    default: break;  
  }
}
