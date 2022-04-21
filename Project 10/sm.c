#include "msp430.h"
#include "ports.h"
#include "wheels.h"
#include "sm.h"
#include <string.h>
#include "adc.h"
#include "timers.h"
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
            stateCounter = 1;
            strcpy(display_line[0], " BL START ");
            display_changed = 1;
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
                Drive_Path(RIGHT_MIN, RIGHT_MIN, 0);
            } else stateCounter++;

            break;
      

        case 7:
            if(LockMotors(-1, -1)) stateCounter++;

            break;

        case 8:
            stateCounter = 0 ;
            stopwatch_seconds = 0;
            cycle_count = 0;
            state = WAIT;
            nextState = TURN;
            strcpy(display_line[0], "INTERCEPT ");
            display_changed = 1;
            EMITTER_OFF;
            break;
    }
}

void Turn(char direction) {
    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            stateCounter = 1;
            strcpy(display_line[0], " BL TURN ");
            display_changed = 1;
            break;

        case 1: // gotta remove this
            if(direction) {
                if(Drive_Path(-RIGHT_MAX, LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;
            } else if(Drive_Path(RIGHT_MAX, -LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;

            break;

        case 2:
            if (lessWhiteOr){//(((ADC_Left_Detect < LEFT_GRAY_DETECT || ADC_Right_Detect < RIGHT_GRAY_DETECT))) {
                if(direction)Drive_Path(-RIGHT_MIN, LEFT_MIN, 0);

                else Drive_Path(RIGHT_MIN, -LEFT_MIN, 0);
            } else stateCounter++;

            break;

        case 3:
            ShutoffMotors();
            stateCounter = 0 ;
            stopwatch_seconds = 0;
            cycle_count = 0;
            state = WAIT;
            nextState = LINEFOLLOW;
            EMITTER_OFF;
            //strcpy(display_line[0], " BL TURN ");
            display_changed = 1;
            break;
    }
}

void LineFollow(char direction) {
    //HEXtoBCD(ADC_Left_Detect, 1, 6);
    //HEXtoBCD(ADC_Right_Detect, 1, 0);

    int rFollowSpeed, lFollowSpeed, rAdjustSpeed, lAdjustSpeed;

    //rFollowSpeed = RIGHT_MIN>>1;
    //lFollowSpeed = LEFT_MIN>>1;

    /*if(ADC_Left_Detect>LEFT_GRAY_DETECT)rFollowSpeed = 3000;
    else*/ 
    rFollowSpeed = additionSafe(RIGHT_FORWARD_SPEED, RIGHT_MAX, RIGHT_MIN>>1, GetOutput(&leftFollowController, LEFT_BLACK_DETECT, ADC_Left_Detect)); // swapped b/c they are physically swapped
    /*if(ADC_Right_Detect>RIGHT_GRAY_DETECT)lFollowSpeed = 3000;
    else*/ 
    lFollowSpeed = additionSafe(LEFT_FORWARD_SPEED, LEFT_MAX, RIGHT_MIN>>1, GetOutput(&rightFollowController, RIGHT_BLACK_DETECT, ADC_Right_Detect));// swapped b/c they are physically swapped
    rAdjustSpeed = (RIGHT_MIN - LF_TURN_DECREMENT);
    lAdjustSpeed = (LEFT_MIN - LF_TURN_DECREMENT);

    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            strcpy(display_line[0], "BL TRAVEL ");
            display_changed = 1;

            if(rightSwitchable && leftSwitchable)stateCounter++;
            else return;

            stopwatch_seconds = 0;
            cycle_count = 0;

            break;

        case 1:
            if(l_LessWhite ^ r_LessWhite) stateCounter = 10;
            else if (lessWhiteAnd) {
                rFollowSpeed = -RIGHT_MIN>>1;
                lFollowSpeed = -LEFT_MIN>>1;
            }/* else {

                ClearController(&rightFollowController);
                ClearController(&leftFollowController);
            }*/

            if(delay(CIRCLING_TIME, 0))  stateCounter = 5;

            if(stopwatch_seconds >= TIME_TO_CIRCLE) strcpy(display_line[0], "BL CIRCLE ");

            Drive_Path(rFollowSpeed, lFollowSpeed, 0);
            break;


        case 2:
            if(l_LessWhite && r_GreaterWhite) stateCounter = 3;
            else if(l_GreaterWhite && r_LessWhite) stateCounter = 4;
            else stateCounter = 1;

            break;

        case 3:// turn left ()
            if(l_LessWhite) Drive_Path(rAdjustSpeed, -lAdjustSpeed, 0);
            else if (greaterWhiteAnd) stateCounter = 1;
            else stateCounter = 4;

            break;

        case 4:
            if(r_LessWhite) Drive_Path(-rAdjustSpeed, lAdjustSpeed, 0);
            else if (greaterWhiteAnd) stateCounter = 1;
            else stateCounter = 3;

            break;

        case 10:
            if(LockMotorsTime(-1, -1, 1)) stateCounter = 2;

            break;

        case 5:
            ShutoffMotors();
            stateCounter = 0 ;
            state = START;
            EMITTER_OFF;
            //strcpy(display_line[0], "          ");
            break;
    }

    if(rFollowSpeed != lFollowSpeed && stateCounter == 1) P6OUT |= GRN_LED;
    else P6OUT &= ~GRN_LED;
}

void Exit(int direction) {
    switch(stateCounter) {
        case 0:
            strcpy(display_line[0], " BL STOP  ");
            /*if(rightSwitchable && leftSwitchable)*/stateCounter++;
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
            stateCounter = 0 ;
            state = DONE;
            stopwatch_seconds = 0;
            cycle_count = 0;
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
            driveStateCounter = 0 ;
            //state = START;
            return 1;
            break;
    }

    return 0;
}



// delays for a specified time make sure stopwatch_seconds and cycle_count are 0 before calling
int delay(int seconds, int cycles) {
    if(stopwatch_seconds == 0 && cycle_count <= 1) {
        display_changed = 1;
    }

    if(stopwatch_seconds >= seconds && cycle_count >= cycles) {
        stopwatch_seconds = 0;
        cycle_count = 0;
        return 1;
    } else return 0;
}





void StateMachine(void) {
    updateDetectors();

    switch(state) {
        case (CALIBRATE):
            calibrate();

            /*if(calibrationMode >= 2) {
                state = START;
            }*/

            break;

        case (START):
            stopwatch_seconds = 0;
            cycle_count = 0;
            break;

        case (WAIT):

            //strcpy(display_line[0], "WAITING...");

            if (delay(delayTime, 0)) state = nextState;

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
