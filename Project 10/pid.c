#include "pid.h"
#include "msp430.h"
#include "macros.h"
#include "detectors.h"
#include "utils.h"

int GetOutput(PIDController* pidController, int setPoint, int current) {
    pidController->error = setPoint - current;
    //if(abs(pidController->error)<=1) pidController->error = 0;
    int integral = additionSafe(pidController->lastIntegral, INT_MAX, INT_MIN, pidController->error);
    int derivative = additionSafe(pidController->error, INT_MAX, INT_MIN, -pidController->lastError);
    pidController->lastError = pidController->error;
    pidController->lastIntegral = integral;
    int errorTerm = multSafe(pidController->error, pidController->kP);
    int derivTerm = multSafe(derivative, pidController->kD);
    int intTerm = multSafe(integral, pidController->kI);
    return additionSafe(additionSafe(errorTerm, INT_MAX, INT_MIN, derivTerm), INT_MAX, INT_MIN, intTerm);
}

void ClearController(PIDController* pidController) {
    pidController->error = 0;
    pidController->lastError = 0;
    pidController->lastIntegral = 0;
}

