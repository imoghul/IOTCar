#include "pid.h"
#include "msp430.h"

int GetOutput(PIDController* pidController, int setPoint, int current){
  pidController->error = setPoint-current;
  int integral = pidController->lastIntegral+pidController->error;
  int derivative = pidController->error-pidController->lastError;
  pidController->lastError = pidController->error;
  pidController->lastIntegral = integral;
  int errorTerm = pidController->error*pidController->kP;
  return (int)(errorTerm + derivative*pidController->kD + integral*pidController->kI);
}

void ClearController(PIDController* pidController){
  pidController->error = 0;
  pidController->lastError = 0;
  pidController->lastIntegral = 0;
}