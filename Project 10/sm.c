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
volatile char state = START;//CALIBRATE;
volatile int stateCounter, driveStateCounter;
volatile char nextState = STRAIGHT;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime = 5;
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

            //strcpy(display_line[0], "WAITING...");

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
