#include "msp430.h"
#include "ports.h"
#include "adc.h"
#include "wheels.h"
#include <string.h>
#include "sm.h"
#include "detectors.h"
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char movingDirection;
int rightVals[VALUES_TO_HOLD];
extern volatile unsigned int ADC_Left_Detect,ADC_Right_Detect;
int lastLeft;
int lastRight;
int leftVals[VALUES_TO_HOLD];
unsigned int leftBlackVal, rightBlackVal, leftWhiteVal, rightWhiteVal;
extern volatile unsigned int adcUpdated;
extern volatile unsigned int calibratingMode;

void EmitterOn(void){
  P6OUT |= IR_LED;
  //P6OUT |= GRN_LED;
  //strcpy(display_line[0], "EMITTER ON");
  //display_changed = 1;
}

void EmitterOff(void){
  P6OUT &= ~IR_LED;
  //P6OUT &= ~GRN_LED;
  //strcpy(display_line[0], "EMITER OFF");
  //strcpy(display_line[2], "          ");
  //strcpy(display_line[3], "          ");
  //display_changed = 1;
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
  
  if(abs(rightDiff)<2) rightDiff = 0;
  if(abs(leftDiff)<2) leftDiff = 0;
  
  if(leftDiff)push(leftVals,currLeft);
  else clearList(leftVals);
  if(rightDiff)push(rightVals,currRight);
  else clearList(rightVals);
  
  if(validList(rightVals) || validList(leftVals)){
    char dirR = getDirection(rightVals);
    char dirL= getDirection(leftVals);
    
    if(dirR==INCREASING || dirL==DECREASING) movingDirection = MOVING_RIGHT;
    else if(dirR==DECREASING || dirL==INCREASING) movingDirection = MOVING_LEFT;
    else movingDirection = NOT_MOVING;
  }
  else movingDirection = NOT_MOVING;
}

void push(int list[], int val){
  for(int i = VALUES_TO_HOLD-1;i>0;--i) list[i] = list[i-1];
  list[0] = val;
}

void clearList(int list[]){
  for (int i = 0;list[i]!=0;++i) list[i]=0;
}

int validList(int* list){
  for(int i = 0;i<VALUES_TO_HOLD;++i) if(list[i]==0)return 0;
  return 1;
}

int rollingSum(int * list){
  int sum = 0;
  for (int i = 0;i<VALUES_TO_HOLD;++i){
    sum+=list[i];
  }
  return sum;
}

char getDirection(int* list){
  int increasing = 0,decreasing = 0;
  for(int i = 1;i<VALUES_TO_HOLD && list[i]!=0;++i){
    if(abs(list[i]-list[i-1])>0){
      if(list[i]>list[i-1]) decreasing++;
      else increasing++;
    }
  }
  return increasing>decreasing?INCREASING:(increasing==decreasing?NEUTRAL:DECREASING);
}

unsigned int abs(int n){
  const int ret[2] = {n,-n};
  return (unsigned int)(ret [n<0]);
}