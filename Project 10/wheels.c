#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "macros.h"
#include "adc.h"
#include "timers.h"
#include "detectors.h"
#include "pid.h"
#include "sm.h"
#include <string.h>

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
volatile unsigned int wheel_periods;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
extern volatile unsigned int ADC_Left_Detect, ADC_Right_Detect;
volatile unsigned int rightSwitchable = 1, leftSwitchable = 1;
unsigned int temp;
extern char movingDirection;
extern int leftVals[VALUES_TO_HOLD];
extern int rightVals[VALUES_TO_HOLD];


PIDController rightFollowController = {
    .kP = KP,// /16
    .kD = KD,// /8
    //.kI = 0,
    .error = 0,
    .lastError = 0
    //.lastIntegral = 0
};
PIDController leftFollowController = {
    .kP = KP,// /16
    .kD = KD,// /8
    //.kI = 0,
    .error = 0,
    .lastError = 0
    //.lastIntegral = 0
};

//===========================================================================
// Function name: ShutoffMotors
//
// Description: This function shuts off the motors and all related tasks
// such as starting switching timer
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void ShutoffMotors(void) {
    ShutoffRight();
    ShutoffLeft();
}

//===========================================================================
// Function name: ShutoffRight
//
// Description: This function shuts off the right motor and starts its 
// switching timer
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: rightSwitchable
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void ShutoffRight(void) {
    RIGHT_FORWARD_SPEED = RIGHT_REVERSE_SPEED = WHEEL_OFF;
    rightSwitchable = false;

    TB1CCTL2 &= ~CCIFG;
    TB1CCR2 = TB1R + TB1CCR2_INTERVAL;
    TB1CCTL2 |= CCIE;
}

//===========================================================================
// Function name: ShutoffLeft
//
// Description: This function shuts off the left motor and stats its
// switching timer
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: leftSwitchable
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void ShutoffLeft(void) {
    LEFT_FORWARD_SPEED = LEFT_REVERSE_SPEED = WHEEL_OFF;
    leftSwitchable = false;

    TB1CCTL1 &= ~CCIFG;
    TB1CCR1 = TB1R + TB1CCR1_INTERVAL;
    TB1CCTL1 |= CCIE;
}

//===========================================================================
// Function name: MotorSafety
//
// Description: This function checks if the motors are in forward and reverse 
// at the same time an turns them off
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void MotorSafety(void) {
    if ((RIGHT_FORWARD_SPEED != OFF && RIGHT_REVERSE_SPEED != OFF) || (LEFT_FORWARD_SPEED != OFF && LEFT_REVERSE_SPEED != OFF)) {
        ShutoffMotors();
        //P1OUT |= RED_LED;
    } else {
        //P1OUT &= ~RED_LED;
    }
}

//===========================================================================
// Function name: RunRightMotor
//
// Description: This function sets the right motor with the desired speed and
// direction, taking into account switching directions
//
// Passed : val
// Locals: no variables declared
// Returned: success
// Globals: rightSwitchable
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int RunRightMotor(int val) {
    if(RIGHT_REVERSE_SPEED > OFF && val > OFF || RIGHT_FORWARD_SPEED > OFF && val < OFF) {
        ShutoffRight();
    }

    if (val > OFF) {
        RIGHT_REVERSE_SPEED = WHEEL_OFF;

        if(rightSwitchable) RIGHT_FORWARD_SPEED = val;

        return true;//P6IN & R_FORWARD;
    } else if (val == OFF) {
        ShutoffRight();
        return rightSwitchable;
    } else {
        RIGHT_FORWARD_SPEED = WHEEL_OFF;

        if(rightSwitchable) RIGHT_REVERSE_SPEED = -val;

        return true;//P6IN & R_REVERSE;
    }
}

//===========================================================================
// Function name: RunRightMotor
//
// Description: This function sets the left motor with the desired speed and
// direciton, taking into account switching directions
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: success
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int RunLeftMotor( int val) {
    if(LEFT_REVERSE_SPEED > OFF && val > OFF || LEFT_FORWARD_SPEED > OFF && val < OFF) {
        ShutoffLeft();
    }

    if (val > OFF) {
        LEFT_REVERSE_SPEED = WHEEL_OFF;

        if(leftSwitchable) LEFT_FORWARD_SPEED = val;

        return true;//P6IN & L_FORWARD;
    } else if (val == OFF) {
        ShutoffLeft();
        return leftSwitchable;
    } else {
        LEFT_FORWARD_SPEED = WHEEL_OFF;

        if(leftSwitchable) LEFT_REVERSE_SPEED = -val;

        return true;//P6IN & L_REVERSE_2355;
    }
}

/*int RunMotor(int val, volatile unsigned short * forwardReg, volatile unsigned short * revReg,unsigned short forwardPin, unsigned short revPin,volatile unsigned int switchable,void (*shutoff)()){
  if(*revReg > 0 && val > 0 || *forwardReg > 0 && val < 0) {
        shutoff();
    }

    if (val > 0) {
        *revReg = WHEEL_OFF;

        if(switchable) {
            *forwardReg = val;
        }

        return P6IN & forwardPin;
    } else if (val == 0) {
        shutoff();
        return switchable;
    } else {
        *forwardReg = WHEEL_OFF;

        if(switchable) *revReg = -val;

        return P6IN & revPin;
    }
}

int RunRightMotor( int val){
  return RunMotor(val, &RIGHT_FORWARD_SPEED, &RIGHT_REVERSE_SPEED, R_FORWARD, R_REVERSE,rightSwitchable,ShutoffRight);
}

int RunLeftMotor( int val){
  return RunMotor(val, &LEFT_FORWARD_SPEED, &LEFT_REVERSE_SPEED, L_FORWARD, L_REVERSE_2355,leftSwitchable,ShutoffLeft);
}*/

//===========================================================================
// Function name: LockMotors
//
// Description: This function electronically brakes the motors, for the 
// predetermined time that is configured to stop the car at full speed
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: completion
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int LockMotors(int polR, int polL) {
    return LockMotorsTime(polR,polL,LOCK_TIME);//(Drive_Path(polR > OFF ? STRAIGHT_RIGHT : -STRAIGHT_RIGHT, polL > OFF ? STRAIGHT_LEFT : -STRAIGHT_LEFT, LOCK_TIME));
}

//===========================================================================
// Function name: LockMotorsTime
//
// Description: This function electronically brakes the motors, for a 
// desired duration
//
// Passed : polR, polL, duration
// Locals: no variables declared
// Returned: completion
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int LockMotorsTime(int polR, int polL, int duration) {
    return (Drive_Path(polR > OFF ? STRAIGHT_RIGHT : -STRAIGHT_RIGHT, polL > OFF ? STRAIGHT_LEFT : -STRAIGHT_LEFT, duration));
}

//===========================================================================
// Function name: Update_Ticks
//
// Description: This function updates duration the car has been driving a 
// specific path
//
// Passed : milliseconds
// Locals: no variables declared
// Returned: completion
// Globals: stopwatch_milliseconds
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int Update_Ticks(int milliseconds) { // each tick is 4ms
    stopwatch_milliseconds += MS_PER_TICK;

    if(stopwatch_milliseconds >= milliseconds) {
        stopwatch_milliseconds = BEGINNING;
        return true;
    }

    return false;
}

/*int Drive_Path_Definite(int speedR, int speedL, unsigned int ticksDuration) {
    int successR = RunRightMotor(speedR);
    int successL = RunLeftMotor(speedL);

    if (time_change) {
        time_change = 0;

        if (successR && successL && Update_Ticks(ticksDuration)) {
            ShutoffMotors();
            return 1;
        }
    }

    return 0;
}


int Drive_Path_Indefinite(int speedR, int speedL) {
    int successR = RunRightMotor(speedR);
    int successL = RunLeftMotor(speedL);
    return successR && successL;
}*/

//===========================================================================
// Function name: Drive_Path
//
// Description: This function sets the motors to different speeds for a 
// certain time
//
// Passed : speedR, speedL, ticksDuration
// Locals: no variables declared
// Returned: completion
// Globals: time_change
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int Drive_Path(int speedR, int speedL, unsigned int ticksDuration) {
    /*int successR = RunRightMotor(speedR);
    int successL = RunLeftMotor(speedL);
    int success = successR && successL;*/
    RunRightMotor(speedR);
    RunLeftMotor(speedL);

    if(ticksDuration == false) return false;

    if (time_change) {
        time_change = BEGINNING;

        if (/*success && */Update_Ticks(ticksDuration)) {
            ShutoffMotors();
            return true;
        }
    }

    return false;
}

