#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "sm.h"
#include <string.h>
#include "adc.h"
#include "timers.h"
#include "detectors.h"
#include "pid.h"

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
extern volatile unsigned int wheel_periods;
volatile char state = CALIBRATE;
volatile int stateCounter;
volatile char nextState = STRAIGHT;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime = 1;
extern volatile unsigned int ADC_Left_Detect,ADC_Right_Detect;
extern volatile unsigned int rightSwitchable, leftSwitchable;
extern unsigned int temp;
extern char movingDirection;
char enteringDirection = NOT_MOVING;
extern int leftVals[VALUES_TO_HOLD];
extern int rightVals[VALUES_TO_HOLD];
extern volatile unsigned int calibrationMode;
extern unsigned int LBDetect, LWDetect, RBDetect, RWDetect;
extern PIDController rightController;
extern PIDController leftController;

void Straight(void){
  
  if (stateCounter == 0) {
    strcpy(display_line[0], "INTERCEPT ");
    display_changed = 1;
    EmitterOn();
    stateCounter++;
  }
  if(stateCounter==1){
    if ((ADC_Left_Detect < LEFT_WHITE_DETECT || ADC_Right_Detect < RIGHT_WHITE_DETECT)){
      Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 0);
    }
    else{
      int left = ADC_Left_Detect;
      int right = ADC_Right_Detect;
      if(left>right) enteringDirection = MOVING_LEFT;
      else enteringDirection = MOVING_RIGHT;
      stateCounter++;
    }
  }
  if(stateCounter==2){
    if(LockMotors(-1,-1)) stateCounter++;
  }
  
  else if (stateCounter==3) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = WAIT;    
    delayTime = 3;
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = TURN;
    EmitterOff();
    //strcpy(display_line[1], "BLACK LINE");
    //strcpy(display_line[2], " DETECTED ");
    //display_changed = 1;
  }
}

void Turn(){
  if (stateCounter == 0) {
    EmitterOn();
    strcpy(display_line[0], "  TURNING ");
    strcpy(display_line[1], "          ");
    strcpy(display_line[2], "          ");
    display_changed = 1;
    stateCounter=1;
  }
  if(stateCounter==1){
    if(enteringDirection == MOVING_LEFT){
      if(Drive_Path(STRAIGHT_RIGHT/2,-STRAIGHT_LEFT/2,20)) stateCounter++;
    }
    else if(enteringDirection == MOVING_RIGHT){
      if(Drive_Path(-STRAIGHT_RIGHT/2,STRAIGHT_LEFT/2,20)) stateCounter++;
    }
  }
  if (stateCounter==2){
    if (((ADC_Left_Detect <= LEFT_GRAY_DETECT/2 || ADC_Right_Detect <= RIGHT_GRAY_DETECT/2))){
      if(enteringDirection == MOVING_LEFT)Drive_Path(STRAIGHT_RIGHT/4,-STRAIGHT_LEFT/4,0);
      if(enteringDirection == MOVING_RIGHT)Drive_Path(-STRAIGHT_RIGHT/4,STRAIGHT_LEFT/4,0);
    }
    else stateCounter++;
  }
  else if (stateCounter==3) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = WAIT;    
    delayTime = 3;
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = LINEFOLLOW;
    EmitterOff();
  }
}

void LineFollow(){
  if(stateCounter==1) P6OUT|=GRN_LED;
  else P6OUT&=~GRN_LED;
  
  if (stateCounter == 0) {
    EmitterOn();
    strcpy(display_line[0], " CIRCLING ");
    display_changed = 1;
    stopwatch_seconds = 0;
    cycle_count = 0;
    if(rightSwitchable && leftSwitchable)stateCounter++;
  }
  
  int rSpeed;
  int lSpeed;
  int leftPIDOut = GetOutput(&leftController,LEFT_GRAY_DETECT,ADC_Left_Detect);
  int rightPIDOut = GetOutput(&rightController,RIGHT_GRAY_DETECT,ADC_Right_Detect);
  rSpeed = additionSafe(RIGHT_FORWARD_SPEED,RIGHT_MIN,RIGHT_MIN>>1,leftPIDOut); // swapped b/c they are physically swapped
  lSpeed = additionSafe(LEFT_FORWARD_SPEED,LEFT_MIN,LEFT_MIN>>1,rightPIDOut); // swapped b/c they are physically swapped
  
  if(stateCounter == 1){
    if(ADC_Left_Detect<(LEFT_GRAY_DETECT>>1) ^ ADC_Right_Detect<(RIGHT_GRAY_DETECT>>1)) stateCounter = 2;
    else if (ADC_Left_Detect<(LEFT_GRAY_DETECT>>1) && ADC_Right_Detect<(RIGHT_GRAY_DETECT>>1)){
      rSpeed = -RIGHT_MIN;
      lSpeed = -LEFT_MIN;
    }
    else {
      ClearController(&rightController);
      ClearController(&leftController);
    }
    
    if(delay(70,0)) stateCounter = 5;
    Drive_Path(rSpeed,lSpeed,0);
  }
  
  if(stateCounter==10)
    if(LockMotorsTime(-1,-1,1)) stateCounter = 2;
  
  if(stateCounter == 2){
    if(ADC_Left_Detect<LEFT_GRAY_DETECT && ADC_Right_Detect>=RIGHT_GRAY_DETECT) // 
      stateCounter = 3;
    else if(ADC_Left_Detect>=LEFT_GRAY_DETECT && ADC_Right_Detect<RIGHT_GRAY_DETECT) // LCIRC
      stateCounter = 4;
    else stateCounter = 1;
  }
  
  
  if(stateCounter == 3){ // turn left ()
     if(ADC_Left_Detect<LEFT_GRAY_DETECT)Drive_Path((RIGHT_MIN-2000),-(LEFT_MIN-2000), 0);
     else if (ADC_Left_Detect>=LEFT_WHITE_DETECT && ADC_Right_Detect>=RIGHT_WHITE_DETECT) stateCounter = 1;
     else stateCounter = 4;
  }
  
  if(stateCounter == 4){
     if(ADC_Right_Detect<RIGHT_GRAY_DETECT)Drive_Path(-(RIGHT_MIN-2000),(LEFT_MIN-2000), 0);
     else if (ADC_Left_Detect>=LEFT_WHITE_DETECT && ADC_Right_Detect>=RIGHT_WHITE_DETECT) stateCounter = 1;
     else stateCounter = 3;
  }
 
  else if (stateCounter==5) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = WAIT;    
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = EXIT;
    EmitterOff();
  }
}

void Exit(){
  if (stateCounter == 0) {
    EmitterOn();
    strcpy(display_line[0], "  EXITING ");
    display_changed = 1;
    if(rightSwitchable && leftSwitchable)stateCounter++;
  }
  
  if (stateCounter == 1){
    if(enteringDirection == MOVING_RIGHT){
      if(Drive_Path(-STRAIGHT_RIGHT,STRAIGHT_LEFT,90)) stateCounter++;
    }
    else if (enteringDirection == MOVING_LEFT){
      if(Drive_Path(STRAIGHT_RIGHT,-STRAIGHT_LEFT,90)) stateCounter++;
    }
  }
  
  if (stateCounter == 2){
    if(enteringDirection == MOVING_LEFT){
      if(LockMotors(1,-1)) stateCounter++;
    }
    else if (enteringDirection == MOVING_RIGHT){
      if(LockMotors(-1,1)) stateCounter++;
    }
  }
  
  if (stateCounter == 3){
    if(Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT,300)) stateCounter++;
  }
 
  if (stateCounter == 4){
    if(LockMotors(-1,-1)) stateCounter++;
  }
  
  else if (stateCounter==5) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = END;    
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = END;
    EmitterOff();
  }
}


// delays for a specified time and then switches state to global nextState
// make sure nextState is set to desired vlaue before the end of delay
int delay(int seconds,int cycles){
  if(stopwatch_seconds == 0 && cycle_count<=1) {
    display_changed = 1;
  }
  if(stopwatch_seconds>=seconds && cycle_count >= cycles) {
    stopwatch_seconds = 0;
    cycle_count = 0;
    return 1;
  }
  else return 0;
}



void StateMachine(void){
  switch(state){
    case (CALIBRATE):
      calibrate();
      if(calibrationMode>=2) {
        state=START;
      }
      break;
    case (START):
      strcpy(display_line[0], "WAITING...");
      //display_changed = 1;
      stopwatch_seconds = 0;
      cycle_count = 0;
      break;
    case (WAIT):
      if (delay(delayTime,0)) state = nextState;
      break;
    case (STRAIGHT):
      Straight();
      break;
    case (TURN):
      Turn();
      break;
    case (LINEFOLLOW):
      LineFollow();
      break;
    case (EXIT):
      Exit();
      break;
    case (END):
      strcpy(display_line[0], "  STOPPED ");
      display_changed = 1;
      break;
    default: break;  
  }
}