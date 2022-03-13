#include "pid.h"
#include "msp430.h"
#include "detectors.h"

int GetOutput(PIDController* pidController, int setPoint, int current){
  pidController->error = setPoint-current;
  if(abs(pidController->error)<=3) pidController->error = 0;
  int integral = additionSafe(pidController->lastIntegral,32767,-32768,pidController->error);
  int derivative = additionSafe(pidController->error,32767,-32768,-pidController->lastError);
  pidController->lastError = pidController->error;
  pidController->lastIntegral = integral;
  int errorTerm = multSafe(pidController->error,pidController->kP);
  int derivTerm = multSafe(derivative,pidController->kD);
  int intTerm = multSafe(integral,pidController->kI);
  return additionSafe(additionSafe(errorTerm,32767,-32768,derivTerm),32767,-32768,intTerm);
}

void ClearController(PIDController* pidController){
  pidController->error = 0;
  pidController->lastError = 0;
  pidController->lastIntegral = 0;
}

int additionSafe(int val, int max, int min, int increment){
    int out = abs(increment);
    int speed = val;
    
    if (increment > 0) {
      speed = val + out;
      if(speed<val) speed = max;
    }
    if (increment < 0) {
      speed = val - out;
      if(speed>val) speed = min;
    }
    
    
    if(speed>max)speed = max;
    if(speed<min)speed = min;
    
    return speed;
}

int multSafe(int a, int b){
  if(a==0||b==0)return 0;
  int res = a*b;
  if(a==res/b)return res;
  else return 32767*(a<0?-1:1)*(b<0?-1:1);
}