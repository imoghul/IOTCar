#include "pid.h"
#include "msp430.h"
#include "detectors.h"

int GetOutput(PIDController* pidController, int setPoint, int current){
  pidController->error = setPoint-current;
  //if(abs(pidController->error)<=1) pidController->error = 0;
  int integral = pidController->lastIntegral+pidController->error;
  int derivative = pidController->error-pidController->lastError;
  pidController->lastError = pidController->error;
  pidController->lastIntegral = integral;
  double errorTerm = multSafe(pidController->error,pidController->kP);
  double derivTerm = multSafe(derivative,pidController->kD);
  double intTerm = multSafe(integral,pidController->kI);
  return (int)(errorTerm + derivTerm + intTerm);
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

double multSafe(double a, double b){
  if(a==0||b==0)return 0;
  int res = a*b;
  if(a==res/b)return res;
  else return 32765*(a<0?-1:1)*(b<0?-1:1);
}