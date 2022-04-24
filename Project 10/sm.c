#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "sm.h"
#include <string.h>
#include "adc.h"
#include "timers.h"
#include "macros.h"
#include "detectors.h"
#include "pid.h"

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
extern volatile unsigned int wheel_periods;
volatile char state = START;
volatile int stateCounter, driveStateCounter;
volatile char nextState = STRAIGHT;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime = DELAY_BETWEEN_EVENTS;
extern volatile unsigned int ADC_Left_Detect, ADC_Right_Detect;
extern volatile unsigned int rightSwitchable, leftSwitchable;
extern unsigned int temp;
extern char movingDirection;
char enteringDirection = NOT_MOVING;
extern int leftVals[VALUES_TO_HOLD];
extern int rightVals[VALUES_TO_HOLD];
extern volatile unsigned int calibrationMode;
extern unsigned int LBDetect, LWDetect, RBDetect, RWDetect;
extern PIDController rightFollowController, rightAdjustController;
extern PIDController leftFollowController, leftAdjustController;

extern short l_LessBlack, l_LessGray, l_LessWhite, r_LessBlack, r_LessGray, r_LessWhite, l_GreaterBlack, l_GreaterGray, l_GreaterWhite, r_GreaterBlack, r_GreaterGray, r_GreaterWhite, lessWhiteOr, lessWhiteAnd, greaterWhiteOr, greaterWhiteAnd, lessWhiteOr, lessWhiteAnd, greaterWhiteOr, greaterWhiteAnd, lessGrayOr, lessGrayAnd, greaterGrayOr, greaterGrayAnd, lessGrayOr, lessGrayAnd, greaterGrayOr, greaterGrayAnd, lessBlackOr, lessBlackAnd, greaterBlackOr, greaterBlackAnd, lessBlackOr, lessBlackAnd, greaterBlackOr, greaterBlackAnd;

int speedRight, speedLeft;
unsigned int driveTime;


//===========================================================================
// Function name: Straight
//
// Description: This function is used to increcept the black line as such:
//
//           --------------->|
//           ^               |
//           |               v
//           |          black line
//           |
//
// Passed : no variables passed
// Locals: rightTurn,leftTurn
// Returned: no values returned
// Globals: stateCounter,display_line,display_changed,lessWhiteOr,
// stopwatch_seconds,cycle_count,state,nextState
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Straight(char direction) {
    int rightTurn = direction ? STRAIGHT_RIGHT : -STRAIGHT_RIGHT;
    int leftTurn = direction ? -STRAIGHT_LEFT : STRAIGHT_LEFT;

    switch(stateCounter) {
        case 0:
            stateCounter++;
            strcpy(LINE1, " BL START ");
            display_changed = true;
            break;

        case 1:
            if(Drive(STRAIGHT_RIGHT, STRAIGHT_LEFT, LEG1))stateCounter++;

            break;

        case 2:
            if(Drive(rightTurn, leftTurn, TURN90))stateCounter++;

            break;

        case 3:
            if(Drive(STRAIGHT_RIGHT, STRAIGHT_LEFT, LEG2))stateCounter++;

            break;

        case 4:
            if(Drive(rightTurn, leftTurn, TURN90))stateCounter++;

            break;

        /*case 5:
        case 6:
            if (greaterWhiteOr) {
                Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, 0);
            } else stateCounter++;

            break;*/

        case 5:
            if(Drive(STRAIGHT_RIGHT, STRAIGHT_LEFT, LEG3))stateCounter++;

            break;


        case 6:
            EMITTER_ON;

            if (lessWhiteOr) {
                Drive_Path(RIGHT_MID, RIGHT_MID, false);
            } else stateCounter++;

            break;


        case 7:
            if(LockMotors(-1, -1)) stateCounter++;

            break;

        case 8:
            stateCounter = BEGINNING;
            stopwatch_seconds = BEGINNING;
            cycle_count = BEGINNING;
            state = WAIT;
            nextState = TURN;
            strcpy(LINE1, "INTERCEPT ");
            display_changed = true;
            EMITTER_OFF;
            break;
    }
}

//===========================================================================
// Function name: Turn
//
// Description: This function is used to turn onto the black line
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: stateCounter,display_line,display_changed,lessWhiteOr,
// stopwatch_seconds,cycle_count,state,nextState
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Turn(char direction) {
    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            stateCounter++;
            strcpy(LINE1, " BL TURN ");
            display_changed = true;
            break;

        case 1: 
            if(direction) {
                if(Drive_Path(-RIGHT_MAX, LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;
            } else if(Drive_Path(RIGHT_MAX, -LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;

            break;

        case 2:
            if (lessWhiteOr) { 
                if(direction)Drive_Path(-RIGHT_MID, LEFT_MID, false);

                else Drive_Path(RIGHT_MID, -LEFT_MID, false);
            } else stateCounter++;

            break;

        case 3:
            ShutoffMotors();
            stateCounter = BEGINNING;
            stopwatch_seconds = BEGINNING;
            cycle_count = BEGINNING;
            state = WAIT;
            nextState = LINEFOLLOW;
            EMITTER_OFF;
            display_changed = true;
            break;
    }
}

//===========================================================================
// Function name: LineFolow
//
// Description: This function is used to follow the black line
//
// Passed : direction
// Locals: rFollowSpeed,lFollowSpeed,rAdjustSpeed,lAdjustSpeed
// Returned: no values returned
// Globals: stateCounter,display_line,display_changed,lessWhiteOr,
// stopwatch_seconds,cycle_count,state,nextState, leftFollowController,
// rightFollowControler, l_LesssWhite,r_LessWhite,lessWhiteAnd,
// greaterWhiteAnd
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void LineFollow(char direction) {
    int rFollowSpeed, lFollowSpeed, rAdjustSpeed, lAdjustSpeed;

    rFollowSpeed = additionSafe(RIGHT_FORWARD_SPEED, RIGHT_MAX, RIGHT_MIN, GetOutput(&leftFollowController, LEFT_BLACK_DETECT, ADC_Left_Detect)); // swapped b/c they are physically swapped
    lFollowSpeed = additionSafe(LEFT_FORWARD_SPEED, LEFT_MAX, RIGHT_MIN, GetOutput(&rightFollowController, RIGHT_BLACK_DETECT, ADC_Right_Detect));// swapped b/c they are physically swapped
    rAdjustSpeed = (RIGHT_MID - LF_TURN_DECREMENT);
    lAdjustSpeed = (LEFT_MID - LF_TURN_DECREMENT);

    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            strcpy(LINE1, "BL TRAVEL ");
            display_changed = true;

            if(rightSwitchable && leftSwitchable)stateCounter++;
            else return;

            stopwatch_seconds = BEGINNING;
            cycle_count = BEGINNING;

            break;

        case 1:
            if(l_LessWhite ^ r_LessWhite) stateCounter = 10;
            else if (lessWhiteAnd) {
                rFollowSpeed = -RIGHT_MIN;
                lFollowSpeed = -LEFT_MIN;
            }

            if(delay(CIRCLING_TIME, false))  stateCounter = 5;

            if(stopwatch_seconds >= TIME_TO_CIRCLE) strcpy(LINE1, "BL CIRCLE ");

            Drive_Path(rFollowSpeed, lFollowSpeed, false);
            break;


        case 2:
            if(l_LessWhite && r_GreaterWhite) stateCounter = 3;
            else if(l_GreaterWhite && r_LessWhite) stateCounter = 4;
            else stateCounter = 1;

            break;

        case 3:// turn left ()
            if(l_LessWhite) Drive_Path(rAdjustSpeed, -lAdjustSpeed, false);
            else if (greaterWhiteAnd) stateCounter = 1;
            else stateCounter = 4;

            break;

        case 4:
            if(r_LessWhite) Drive_Path(-rAdjustSpeed, lAdjustSpeed, false);
            else if (greaterWhiteAnd) stateCounter = 1;
            else stateCounter = 3;

            break;

        case 10:
            if(LockMotorsTime(-1, -1, LF_LOCK_TIME)) stateCounter = 2;

            break;

        case 5:
            ShutoffMotors();
            stateCounter = BEGINNING;
            state = START;
            EMITTER_OFF;
            break;
    }

}

//===========================================================================
// Function name: Exit
//
// Description: This function is used to turn and exit the black line after 
// finishing following it
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: stateCounter,display_line,display_changed, stopwatch_seconds,
// cycle_count,state
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Exit(int direction) {
    switch(stateCounter) {
        case 0:
            strcpy(LINE1, " BL STOP  ");
            stateCounter++;
            break;

        case 1:
            if ( Drive(direction ? -STRAIGHT_RIGHT : STRAIGHT_RIGHT, direction ? STRAIGHT_LEFT : -STRAIGHT_LEFT, TURN90) )
                stateCounter++;

            break;

        case 2:
            if(Drive(STRAIGHT_RIGHT, STRAIGHT_LEFT, EXITING_TIME)) stateCounter++;

            break;

        case 3:
            ShutoffMotors();
            stateCounter = BEGINNING;
            state = DONE;
            stopwatch_seconds = BEGINNING;
            cycle_count = BEGINNING;
            break;
    }
}

//===========================================================================
// Function name: Drive
//
// Description: This function is used to drive a certain path and then 
// electronically brake
//
// Passed : r, l, time
// Locals: no locals used
// Returned: completion
// Globals: driveStateCounter
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int Drive(int r, int l, unsigned int time) {
    switch(driveStateCounter) {

        case 0 :
            driveStateCounter++;
            break;

        case 1 :
            if(Drive_Path(r, l, time))driveStateCounter++;

            break;

        case 2 :
            if(LockMotors(-r, -l)) driveStateCounter++;

            break;

        case 3 :
            ShutoffMotors();
            driveStateCounter = BEGINNING;
            //state = START;
            return true;
            break;
    }

    return false;
}

//===========================================================================
// Function name: delay
//
// Description: This function is used as a customizable timer to choose 
// how long to delay before returning true
//
// Passed : seconds, cycles
// Locals: no locals used
// Returned: no values returned
// Globals: stopwatch_seconds,cycle_count, display_changed
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

// delays for a specified time make sure stopwatch_seconds and cycle_count are 0 before calling
int delay(int seconds, int cycles) {
    if(stopwatch_seconds == BEGINNING && cycle_count <= (BEGINNING + 1)) {
        display_changed = true;
    }

    if(stopwatch_seconds >= seconds && cycle_count >= cycles) {
        stopwatch_seconds = BEGINNING;
        cycle_count = FALSE;
        return true;
    } else return false;
}



//===========================================================================
// Function name: StateMachine
//
// Description: This function is the main state machine for wheel processes
//
// Passed : no variables passed
// Locals: no locals used
// Returned: no values returned
// Globals: stateCounter, stopwatch_seconds,cycle_count,state,nextState
// delayTime, speedRight, speedLeft
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void StateMachine(void) {
    updateDetectors();

    switch(state) {
        /*case (CALIBRATE):
            calibrate();

            if(calibrationMode >= 2) {
                state = START;
            }

            break;*/

        case (START):
            stopwatch_seconds = BEGINNING;
            cycle_count = BEGINNING;
            break;

        case (WAIT):

            if (delay(delayTime, false)) state = nextState;

            break;

        case (STRAIGHT):
            Straight(speedRight);
            break;

        case (TURN):
            Turn(speedRight);
            break;

        case (LINEFOLLOW):
            LineFollow(speedRight);
            break;

        case (EXIT):
            Exit(speedRight);
            break;

        case (DRIVE):
            if(Drive(speedRight, speedLeft, driveTime)) state = START;

            break;

        case (DONE):
            break;

        default:
            break;
    }
}
