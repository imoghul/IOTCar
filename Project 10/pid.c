#include "pid.h"
#include "msp430.h"
#include "macros.h"
#include "detectors.h"
#include "utils.h"

//===========================================================================
// Function name: GetOutput
//
// Description: This function retruns the pid output based on the controller,
// set point, and current value
//
// Passed : pidController, setPoint, current
// Locals: no variables declared
// Returned: pid ouput
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int GetOutput(PIDController* pidController, int setPoint, int current) {
    int error = setPoint - current;
    pidController->error = error;
    // INTEGRAL NOT EVEN BEGIN USED
    //long long integral = pidController->lastIntegral+error;
    //int integral = additionSafe(pidController->lastIntegral, INT_MAX, INT_MIN, pidController->error);
    int derivative = error - pidController->lastError;
    pidController->lastError = error;
    //pidController->lastIntegral = integral;
    int errorTerm = (error * pidController->kP) >> KP_SHIFT;
    int derivTerm = (derivative * pidController->kD) >> KD_SHIFT;
    //long intTerm = integral, pidController->kI;
    return additionSafe(errorTerm, INT_MAX, INT_MIN, derivTerm);
}

//===========================================================================
// Function name: ClearController
//
// Description: This function clears the error of a pid controller
//
// Passed : pidController
// Locals: no variables declared
// Returned: no values returned
// Globals: not globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void ClearController(PIDController* pidController) {
    pidController->error = BEGINNING;
    pidController->lastError = BEGINNING;
    //pidController->lastIntegral = 0;
}

