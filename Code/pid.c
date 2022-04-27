#include "pid.h"
#include "msp430.h"
#include "macros.h"
#include "detectors.h"
#include "utils.h"

int GetOutput(PIDController* pidController, int setPoint, int current) {
    int error = setPoint - current;
    pidController->error = error;
    // INTEGRAL NOT EVEN BEGIN USED
    //long long integral = pidController->lastIntegral+error;
    //int integral = additionSafe(pidController->lastIntegral, INT_MAX, INT_MIN, pidController->error);
    int derivative = error - pidController->lastError;
    pidController->lastError = error;
    //pidController->lastIntegral = integral;
    int errorTerm = (error * pidController->kP) >> 4;
    int derivTerm = (derivative * pidController->kD) >> 3;
    //long intTerm = integral, pidController->kI;
    return additionSafe(errorTerm, INT_MAX, INT_MIN, derivTerm);
}

void ClearController(PIDController* pidController) {
    pidController->error = 0;
    pidController->lastError = 0;
    //pidController->lastIntegral = 0;
}

