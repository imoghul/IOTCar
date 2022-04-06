#include "pid.h"
#include "msp430.h"
#include "macros.h"
#include "detectors.h"

int GetOutput(PIDController* pidController, int setPoint, int current) {
    pidController->error = setPoint - current;
    //if(abs(pidController->error)==1) pidController->error = 0;
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

int additionSafe(int val, int max, int min, int increment) {
    /*int out = abs(increment);
    int speed = val;

    if (increment > 0) {
        speed = val + out;

        if(speed < val) speed = max;
    }

    if (increment < 0) {
        speed = val - out;

        if(speed > val) speed = min;
    }


    if(speed > max)speed = max;

    if(speed < min)speed = min;*/
  
    long res = val + increment;
    
    if(res>max) res = max;
    if(res<min) res = min;

    return (int)res;
}

int multSafe(int a, int b) {
    if(a == 0 || b == 0)return 0;

    int res = a * b;

    if(a == res / b)return res;
    else return (INT_MAX-1) * (a < 0 ? -1 : 1) * (b < 0 ? -1 : 1);
}