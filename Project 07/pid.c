#include "pid.h"
#include "msp430.h"

int GetOutput(PIDController* pidController, int setPoint, int current){
  pidController->error = setPoint-current;
  int integral = pidController->lastIntegral+pidController->error;
  int derivative = pidController->error-pidController->lastError;
  pidController->lastError = pidController->error;
  pidController->lastIntegral = integral;
  return (int)(pidController->error*pidController->kP + derivative*pidController->kD + integral*pidController->kI);
}