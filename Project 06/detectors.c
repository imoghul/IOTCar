#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include <string.h>
#include "detectors.h"
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char movingDirection;
int rightVals[MEMORY_LEN];
extern volatile unsigned int ADC_Left_Detect,ADC_Right_Detect;
int lastLeft;
int lastRight;
int leftVals[MEMORY_LEN];
extern unsigned int adcUpdated;

void EmitterOn(void){
  P6OUT |= IR_LED;
  P6OUT |= GRN_LED;
  strcpy(display_line[0], "EMITTER ON");
  display_changed = 1;
}

void EmitterOff(void){
  P6OUT &= ~IR_LED;
  P6OUT &= ~GRN_LED;
  strcpy(display_line[0], "EMITER OFF");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = 1;
}

void DetectMovement(void){
  if(adcUpdated==0) return;
  adcUpdated = 0;
  int currLeft = ADC_Left_Detect;
  int currRight = ADC_Right_Detect;
  int leftDiff = currLeft-lastLeft;
  int rightDiff = currRight-lastRight;
  lastLeft = currLeft;
  lastRight = currRight;
  
  if(abs(rightDiff)<10) rightDiff = 0;
  if(abs(leftDiff)<10) leftDiff = 0;
  
  if(leftDiff)push(leftVals,currLeft);
  else clearList(leftVals);
  if(rightDiff)push(rightVals,currRight);
  else clearList(rightVals);
  
  int avgLeft = average(leftVals);
  int avgRight = average(rightVals);
  if((avgLeft != 0 && avgRight == 0) || avgLeft>avgRight) movingDirection = MOVING_LEFT;
  else if((avgLeft == 0 && avgRight != 0) || avgLeft<avgRight) movingDirection = MOVING_RIGHT;
  //else if(avgLeft == 0 && avgRight == 0) movingDirection = NOT_MOVING;
  //else movingDirection = MOVING_STRAIGHT;
}

void push(int list[], int val){
  for(int i = MEMORY_LEN-1;i>0;--i) list[i] = list[i-1];
  list[0] = val;
}

void clearList(int list[]){
  for (int i = 0;list[i]!=0;++i) list[i]=0;
}

int average(int * list){
  int sum = 0;
  for (int i = 0;i<MEMORY_LEN;++i){
    sum+=list[i];
  }
  return sum;
}

int abs(int n){
  const int ret[2] = {n,-n};
  return ret [n<0];
}