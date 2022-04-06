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
volatile int stateCounter;
volatile char nextState = STRAIGHT;
extern volatile unsigned int Time_Sequence;
extern volatile unsigned int Last_Time_Sequence;
extern volatile unsigned int time_change;
volatile unsigned int delayTime = 1;
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

int polarityRight, polarityLeft;
unsigned int driveTime;


void Straight(void) {
    switch(stateCounter) {
        case 0:
            EmitterOn();
            stateCounter = 1;
            break;

        case 1:
            if ((ADC_Left_Detect < LEFT_WHITE_DETECT || ADC_Right_Detect < RIGHT_WHITE_DETECT)) {
                Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, 0);
            } else {
                if(ADC_Left_Detect > ADC_Right_Detect) enteringDirection = MOVING_LEFT;
                else enteringDirection = MOVING_RIGHT;

                stateCounter++;
            }

            break;

        case 2:
            if(LockMotors(-1, -1)) stateCounter++;

            break;

        case 3:
            stateCounter = 0 ;
            state = WAIT;
            nextState = TURN;
            EmitterOff();
            break;
    }
}

void Turn() {
    switch(stateCounter) {
        case 0:
            EmitterOn();
            stateCounter = 1;
            break;

        case 1: // gotta remove this
            if(enteringDirection == MOVING_LEFT)
                if(Drive_Path(STRAIGHT_RIGHT >> 1, -STRAIGHT_LEFT >> 1, 20)) stateCounter++;
                else if(enteringDirection == MOVING_RIGHT)
                    if(Drive_Path(-STRAIGHT_RIGHT >> 1, STRAIGHT_LEFT >> 1, 20)) stateCounter++;

            break;

        case 2:
            if (((ADC_Left_Detect <= LEFT_GRAY_DETECT || ADC_Right_Detect <= RIGHT_GRAY_DETECT))) {
                if(enteringDirection == MOVING_LEFT)Drive_Path(STRAIGHT_RIGHT >> 2, -STRAIGHT_LEFT >> 2, 0);

                if(enteringDirection == MOVING_RIGHT)Drive_Path(-STRAIGHT_RIGHT >> 2, STRAIGHT_LEFT >> 2, 0);
            } else stateCounter++;

            break;

        case 3:
            ShutoffMotors();
            stateCounter = 0 ;
            state = WAIT;
            nextState = LINEFOLLOW;
            EmitterOff();
            break;
    }
}

void LineFollow() {

    int rFollowSpeed, rAdjustSpeed;
    int lFollowSpeed, lAdjustSpeed;

    int leftPIDOut = GetOutput(&leftFollowController, LEFT_WHITE_DETECT, ADC_Left_Detect);
    int rightPIDOut = GetOutput(&rightFollowController, RIGHT_WHITE_DETECT, ADC_Right_Detect);
    rFollowSpeed = additionSafe(RIGHT_FORWARD_SPEED, RIGHT_MAX, RIGHT_MIN >> 1, leftPIDOut); // swapped b/c they are physically swapped
    lFollowSpeed = additionSafe(LEFT_FORWARD_SPEED, LEFT_MAX, LEFT_MIN >> 1, rightPIDOut); // swapped b/c they are physically swapped

    switch(stateCounter) {
        case 0:
            EmitterOn();
            stopwatch_seconds = 0;
            cycle_count = 0;

            if(rightSwitchable && leftSwitchable)stateCounter++;
            else return;

            break;

        case 1:
            if(ADC_Left_Detect < (LEFT_GRAY_DETECT) ^ ADC_Right_Detect < (RIGHT_GRAY_DETECT)) stateCounter = 2;
            else if (ADC_Left_Detect < (LEFT_GRAY_DETECT) && ADC_Right_Detect < (RIGHT_GRAY_DETECT)) {
                rFollowSpeed = -RIGHT_MIN;
                lFollowSpeed = -LEFT_MIN;
            } else {
                ClearController(&rightFollowController);
                ClearController(&leftFollowController);
            }

            if(delay(70, 0)) stateCounter = 5;

            Drive_Path(rFollowSpeed, lFollowSpeed, 0);
            break;


        case 2:
            if(ADC_Left_Detect < LEFT_BLACK_DETECT && ADC_Right_Detect >= RIGHT_BLACK_DETECT) stateCounter = 3;
            else if(ADC_Left_Detect >= LEFT_BLACK_DETECT && ADC_Right_Detect < RIGHT_BLACK_DETECT) stateCounter = 4;
            else stateCounter = 1;

            break;

        case 3:// turn left ()
            if(ADC_Left_Detect < LEFT_BLACK_DETECT) Drive_Path((RIGHT_MIN - LF_TURN_DECREMENT), -(LEFT_MIN - LF_TURN_DECREMENT), 0);
            else if (ADC_Left_Detect >= LEFT_WHITE_DETECT && ADC_Right_Detect >= RIGHT_WHITE_DETECT) stateCounter = 1;
            else stateCounter = 4;

            break;

        case 4:
            if(ADC_Right_Detect < RIGHT_BLACK_DETECT) Drive_Path(-(RIGHT_MIN - LF_TURN_DECREMENT), (LEFT_MIN - LF_TURN_DECREMENT), 0);
            else if (ADC_Left_Detect >= LEFT_WHITE_DETECT && ADC_Right_Detect >= RIGHT_WHITE_DETECT) stateCounter = 1;
            else stateCounter = 3;

            break;

        //case 10:
        //    if(LockMotorsTime(-1, -1, 1)) stateCounter = 2;
        //    break;

        case 5:
            ShutoffMotors();
            stateCounter = 0 ;
            state = START;
            EmitterOff();
            break;
    }


}

void Drive(int polR, int polL, unsigned int time) {
    switch(stateCounter) {

        case 0 :
            EmitterOn();
            stateCounter++;
            break;

        case 1 :
            if(Drive_Path(polR * STRAIGHT_RIGHT, polL * STRAIGHT_LEFT, time))stateCounter++;

            break;

        case 2 :
            if(LockMotors(-polR, -polL)) stateCounter++;

            break;

        case 3 :
            ShutoffMotors();
            stateCounter = 0 ;
            state = START;
            EmitterOff();
            break;
    }
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
    switch(state) {
        case (CALIBRATE):
            calibrate();

            if(calibrationMode >= 2) {
                state = START;
            }

            break;

        case (START):
            stopwatch_seconds = 0;
            cycle_count = 0;
            break;

        case (WAIT):
            strcpy(display_line[0], "WAITING...");

            if (delay(delayTime, 0)) state = nextState;

            break;

        case (STRAIGHT):
            Straight();
            break;

        case (TURN):
            Turn();
            break;

        case (LINEFOLLOW):
            LineFollow();
            break;

        case (DRIVE):
            Drive(polarityRight, polarityLeft, driveTime);

        //if(Drive_Path(polarityRight*STRAIGHT_RIGHT,polarityLeft*STRAIGHT_LEFT, driveTime))state = START;

        default:
            break;
    }
}
