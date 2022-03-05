#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "adc.h"
#include "timers.h"
#include "detectors.h"
#include "pid.h"
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
unsigned int temp;
extern char movingDirection;
char enteringDirection = NOT_MOVING;
extern int leftVals[MEMORY_LEN];
extern int rightVals[MEMORY_LEN];
PIDController rightController = {
  .kP = 15,
  .kD = 0,
  .kI = 0,
  .error = 0,
  .lastError = 0,
  .lastIntegral = 0
};
PIDController leftController = {
  .kP = 15,
  .kD = 0,
  .kI = 0,
  .error = 0,
  .lastError = 0,
  .lastIntegral = 0
};


void ShutoffMotors(void){
  ShutoffRight();
  ShutoffLeft();
}

void ShutoffRight(void){
  RIGHT_FORWARD_SPEED = RIGHT_REVERSE_SPEED = WHEEL_OFF;
  rightSwitchable = 0;
  TB1CCTL1 &= ~CCIFG;
  TB1CCR1 = TB1R + TB1CCR1_INTERVAL;
  TB1CCTL1 |= CCIE;   
}

void ShutoffLeft(void){
  LEFT_FORWARD_SPEED = LEFT_REVERSE_SPEED = WHEEL_OFF;
  leftSwitchable = 0;
  TB1CCTL2 &= ~CCIFG;
  TB1CCR2 = TB1R + TB1CCR2_INTERVAL;
  TB1CCTL2 |= CCIE; 
}

void MotorSafety(void){
  
  if ((((P6IN & R_FORWARD) && (P6IN & R_REVERSE)) || ((P6IN & L_FORWARD) && (P6IN & L_REVERSE_2355)))
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
  if (val>0){
    RIGHT_REVERSE_SPEED = WHEEL_OFF;
    if(rightSwitchable) RIGHT_FORWARD_SPEED = val;
    return P6IN&R_FORWARD;
  }
  else if (val==0){
    ShutoffRight();
    return rightSwitchable;
  }
  else{
    RIGHT_FORWARD_SPEED = WHEEL_OFF;
    if(rightSwitchable) {
      RIGHT_REVERSE_SPEED = -val; 
    }
    return P6IN&R_REVERSE;
  }
  //MotorSafety();
}

int RunLeftMotor( int val){
  if(LEFT_REVERSE_SPEED>0 && val>0 || LEFT_FORWARD_SPEED>0 && val<0){
    ShutoffLeft();
  }
  if (val>0){
    LEFT_REVERSE_SPEED = WHEEL_OFF;
    if(leftSwitchable) {
      LEFT_FORWARD_SPEED = val;
    }
    return P6IN&L_FORWARD;
  }
  else if (val==0){
    ShutoffLeft();
    return leftSwitchable;
  }
  else{
    LEFT_FORWARD_SPEED = WHEEL_OFF;
    if(leftSwitchable) {
      LEFT_REVERSE_SPEED = -val;
    }
    return P6IN&L_REVERSE_2355;
  }
  //MotorSafety();
}

int LockMotors(int polR,int polL){
  return (Drive_Path(polR*STRAIGHT_RIGHT,polL*STRAIGHT_LEFT, 5));
}

int Update_Ticks(int max_tick){
  if(++wheel_periods>max_tick){
    wheel_periods=0; 
    return 1;
  }
  return 0;
}

int Drive_Path(int speedR,int speedL, unsigned int ticksDuration){  
  int successR = RunRightMotor(speedR); 
  int successL = RunLeftMotor(speedL);
  if(ticksDuration == 0) return successR && successL;
  if (time_change){
    time_change = 0;
    if (successR && successL && Update_Ticks(ticksDuration)){
      ShutoffMotors();
      return 1;
    }
  }
  return 0;
}



void Straight(void){
  
  if (stateCounter == 0) {
    EmitterOn();
    stateCounter++;
  }
  if(stateCounter==1){
    if ((ADC_Left_Detect <= LEFT_LINE_DETECT && ADC_Right_Detect <= RIGHT_LINE_DETECT)){
      Drive_Path(STRAIGHT_RIGHT,STRAIGHT_LEFT, 0);
    }
    else{
      int left = ADC_Left_Detect;
      int right = ADC_Right_Detect;
      if(left>right) enteringDirection = MOVING_LEFT;
      else if(left<right) enteringDirection = MOVING_RIGHT;
      stateCounter++;
    }
  }
  if(stateCounter==2){
    if(LockMotors(-1,-1)) stateCounter=4;
  }
  if (stateCounter==3){
    if (((ADC_Left_Detect <= LEFT_LINE_DETECT && ADC_Right_Detect <= RIGHT_LINE_DETECT))){
      Drive_Path(-STRAIGHT_RIGHT/5,-STRAIGHT_LEFT/5,0);
    }
    else stateCounter++;
  }
  else if (stateCounter==4) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = TURN;    
    delayTime = 3;
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = TURN;
    EmitterOff();
    strcpy(display_line[1], "BLACK LINE");
    strcpy(display_line[2], " DETECTED ");
    display_changed = 1;
  }
}

void Turn(){
  if (stateCounter == 0) {
    EmitterOn();
    strcpy(display_line[1], "          ");
    strcpy(display_line[2], "          ");
    display_changed = 1;
    stateCounter=2;
  }
  /*if(stateCounter==1){if(enteringDirection == MOVING_LEFT){
      if(Drive_Path(STRAIGHT_RIGHT/2,-STRAIGHT_LEFT/2,20)) stateCounter++;
    }
    else if(enteringDirection == MOVING_RIGHT){
      if(Drive_Path(-STRAIGHT_RIGHT/2,STRAIGHT_LEFT/2,20)) stateCounter++;
    }
  }*/
  if (stateCounter==2){
    if (((ADC_Left_Detect <= LEFT_LINE_DETECT || ADC_Right_Detect <= RIGHT_LINE_DETECT))){
      if(enteringDirection == MOVING_LEFT)Drive_Path(STRAIGHT_RIGHT/4,-STRAIGHT_LEFT/4,0);
      if(enteringDirection == MOVING_RIGHT)Drive_Path(-STRAIGHT_RIGHT/4,STRAIGHT_LEFT/4,0);
    }
    else stateCounter++;
  }
  else if (stateCounter==3) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = LINEFOLLOW;    
    nextState = LINEFOLLOW;
    EmitterOff();
  }
}

void LineFollow(){
  if (stateCounter == 0) {
    EmitterOn();
    if(rightSwitchable && leftSwitchable)stateCounter++;
  }
  
  if(stateCounter == 1){
    /*if(ADC_Right_Detect<RIGHT_LINE_DETECT && ADC_Left_Detect<LEFT_LINE_DETECT) {
      stateCounter = 3;
      return;
    }*/
    // METHOD 1
    /*int leftPIDOut = GetOutput(&leftController, 8, ADC_Left_Detect);
    int rightPIDOut = GetOutput(&rightController, 8, ADC_Right_Detect);
    int rSpeed = additionSafe(RIGHT_FORWARD_SPEED,RIGHT_MAX,RIGHT_MIN,leftPIDOut);
    //rSpeed = additionSafe(rSpeed,RIGHT_MAX/2,-RIGHT_MAX/2,-rightPIDOut);
    int lSpeed = additionSafe(LEFT_FORWARD_SPEED,LEFT_MAX,RIGHT_MIN,rightPIDOut);
    //lSpeed = additionSafe(lSpeed,LEFT_MAX/2,-LEFT_MIN/2,-leftPIDOut);
    */
    // METHOD 2
    /*int PIDOut = GetOutput(&leftController, ADC_Right_Detect, ADC_Left_Detect);
    unsigned int rSpeed = additionSafe(RIGHT_FORWARD_SPEED,RIGHT_MAX,RIGHT_MIN,PIDOut);
    unsigned int lSpeed = additionSafe(LEFT_FORWARD_SPEED,LEFT_MAX,LEFT_MIN,-PIDOut);
    */
    // METHOD 3 (Bang bang)
    int rSpeed = RIGHT_MAX;
    int lSpeed = LEFT_MAX;
    if(ADC_Left_Detect<LEFT_LINE_DETECT || ADC_Right_Detect<RIGHT_LINE_DETECT) stateCounter = 2;
    
    
    /*if(ADC_Left_Detect>=LEFT_LINE_DETECT && ADC_Right_Detect>=RIGHT_LINE_DETECT){
      //rSpeed = RIGHT_MAX/2;
      //lSpeed = LEFT_MAX/2;
      ClearController(&rightController);
      ClearController(&leftController);
    }
    if(abs(rSpeed)<RIGHT_MIN) rSpeed = (rSpeed/abs(rSpeed))*RIGHT_MIN;
    if(abs(lSpeed)<LEFT_MIN) lSpeed = (lSpeed/abs(lSpeed))*LEFT_MIN;
    
    
    HEXtoBCD(LEFT_FORWARD_SPEED/10, 2,0);
    HEXtoBCD(RIGHT_FORWARD_SPEED/10, 2,6);*/
    Drive_Path(rSpeed,lSpeed,0);
  }
  
  if(stateCounter == 2){
    if(ADC_Left_Detect<LEFT_LINE_DETECT && ADC_Right_Detect>=RIGHT_LINE_DETECT) // RCIRC
      stateCounter = 3;
    else if(ADC_Left_Detect>=LEFT_LINE_DETECT && ADC_Right_Detect<RIGHT_LINE_DETECT) // LCIRC
      stateCounter = 4;
    else stateCounter = 1;
  }
  
  if(stateCounter == 3){
     if(ADC_Left_Detect<LEFT_LINE_DETECT)
       Drive_Path(RCIRC_RIGHT/4,-RCIRC_LEFT, 0);
     else if (ADC_Left_Detect>=LEFT_LINE_DETECT && ADC_Right_Detect>=RIGHT_LINE_DETECT) stateCounter = 1;
     else 
       stateCounter = 4;
  }
  
  if(stateCounter == 4){
     if(ADC_Right_Detect<RIGHT_LINE_DETECT)
       Drive_Path(-LCIRC_RIGHT,LCIRC_LEFT/4, 0);
     else if (ADC_Left_Detect>=LEFT_LINE_DETECT && ADC_Right_Detect>=RIGHT_LINE_DETECT) stateCounter = 1;
     else 
       stateCounter = 3;
  }
 
  else if (stateCounter==7) {
    ShutoffMotors();
    stateCounter = 0 ;
    state = END;    
    stopwatch_seconds = 0;
    cycle_count = 0;
    nextState = LINEFOLLOW;
    EmitterOff();
  }
}


// delays for a specified time and then switches state to global nextState
// make sure nextState is set to desired vlaue before the end of delay
int delay(int seconds,int cycles){
  if(stopwatch_seconds == 0 && cycle_count<=1) {
    //strcpy(display_line[0], "WAITING...");
    //display_changed = 1;
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
    case (START):
      //strcpy(display_line[0], "WAITING...");
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
    case (END):
      strcpy(display_line[0], "    END   ");
      display_changed = 1;
      break;
    default: break;  
  }
}

/*int polL = leftPIDOut>=0 ? 1:-1;
    int polR = rightPIDOut>=0 ? 1:-1;
    unsigned int leftOut = abs(leftPIDOut);
    unsigned int rightOut = abs(rightPIDOut);
    unsigned int rSpeed;
    unsigned int lSpeed;
    
    if (polL > 0) {
      rSpeed = RIGHT_FORWARD_SPEED + leftOut;
      if(rSpeed<RIGHT_FORWARD_SPEED) rSpeed = RIGHT_MAX;
    }
    if (polL < 0) {
      rSpeed = RIGHT_FORWARD_SPEED - leftOut;
      if(rSpeed>RIGHT_FORWARD_SPEED) rSpeed = RIGHT_MIN;
    }
    if (polR > 0) {
      lSpeed = LEFT_FORWARD_SPEED + rightOut;
      if(lSpeed<LEFT_FORWARD_SPEED) lSpeed = LEFT_MAX;
    }
    if (polR < 0) {
      lSpeed = LEFT_FORWARD_SPEED - rightOut;
      if(lSpeed>LEFT_FORWARD_SPEED) lSpeed = LEFT_MIN;
    }
    
    if(rSpeed>RIGHT_MAX)rSpeed = RIGHT_MAX;
    if(rSpeed<RIGHT_MIN)rSpeed = RIGHT_MIN;
    if(lSpeed>LEFT_MAX)lSpeed = LEFT_MAX;
    if(lSpeed<LEFT_MIN)lSpeed = LEFT_MIN;
      */
