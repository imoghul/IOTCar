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


void Straight(char direction) {
  int rightTurn = direction?STRAIGHT_RIGHT:-STRAIGHT_RIGHT;
  int leftTurn = direction?-STRAIGHT_LEFT:STRAIGHT_LEFT;
    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            stateCounter = 1;
            strcpy(display_line[0], "          ");
            display_changed = 1;
            break;

        case 1:
            if(Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, LEG1)) stateCounter++; // straight
            break;
            
        case 2:
            if(LockMotors(-1, -1)) stateCounter++;
            break;
            
        case 3:
          if(Drive_Path(rightTurn, leftTurn, TURN90)) stateCounter++;  // turn
          break;
        
        case 4:
            if(LockMotors(-rightTurn, -leftTurn)) stateCounter++;
            break;
            
        case 5:
            if(Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, LEG2)) stateCounter++;// straight
            break;
            
        case 6:
            if(LockMotors(-1, -1)) stateCounter++;
            break;
            
        case 7:
            if(Drive_Path(rightTurn, leftTurn, TURN90)) stateCounter++; // turn
            break;
        case 8:
            if(LockMotors(-rightTurn, -leftTurn)) stateCounter++;
            break;
            
        case 9:
            if ((ADC_Left_Detect > LEFT_WHITE_DETECT || ADC_Right_Detect > RIGHT_WHITE_DETECT)) {
                Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, 0);
            }
            else stateCounter++;

            break;
            
        case 10:
            if ((ADC_Left_Detect < LEFT_WHITE_DETECT || ADC_Right_Detect < RIGHT_WHITE_DETECT)) {
                Drive_Path(RIGHT_MIN, RIGHT_MIN, 0);
            }
            else stateCounter++;

            break;

        case 11:
            if(LockMotors(-1, -1)) stateCounter++;

            break;

        case 12:
            stateCounter = 0 ;
            stopwatch_seconds = 0;
            cycle_count = 0;
            state = WAIT;
            nextState = TURN;
            strcpy(display_line[0],"INTERCEPT ");
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
            strcpy(display_line[0], "          ");
            display_changed = 1;
            break;

        case 1: // gotta remove this
          if(direction){
                if(Drive_Path(-RIGHT_MAX, LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;
          }else/* if(direction == MOVING_RIGHT)*/
                    if(Drive_Path(RIGHT_MAX, -LEFT_MAX, PRELIMINARY_TURN)) stateCounter++;

            break;

        case 2:
            if (((ADC_Left_Detect < LEFT_WHITE_DETECT || ADC_Right_Detect < RIGHT_WHITE_DETECT))) {
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
            strcpy(display_line[0],"   TURN   ");
            display_changed = 1;
            break;
    }
}

void LineFollow(char direction) {
    //HEXtoBCD(ADC_Left_Detect, 1, 6);
    //HEXtoBCD(ADC_Right_Detect, 1, 0);
    
    int rFollowSpeed,lFollowSpeed;

    //rFollowSpeed = RIGHT_MIN>>1;
    //lFollowSpeed = LEFT_MIN>>1;
    
    /*if(ADC_Left_Detect>LEFT_GRAY_DETECT)rFollowSpeed = 3000;
    else*/ rFollowSpeed = direction?additionSafe(RIGHT_FORWARD_SPEED, RIGHT_MAX, 4000, GetOutput(&leftFollowController, LEFT_BLACK_DETECT, ADC_Left_Detect)):RIGHT_MIN>>1; // swapped b/c they are physically swapped
    /*if(ADC_Right_Detect>RIGHT_GRAY_DETECT)lFollowSpeed = 3000;
    else*/ lFollowSpeed = direction?LEFT_MIN>>1:additionSafe(LEFT_FORWARD_SPEED, LEFT_MAX, 4000, GetOutput(&rightFollowController, RIGHT_BLACK_DETECT, ADC_Right_Detect));// swapped b/c they are physically swapped
    
    switch(stateCounter) {
        case 0:
            EMITTER_ON;
            stopwatch_seconds = 0;
            cycle_count = 0;
            strcpy(display_line[0],"          ");
            display_changed = 1;
            if(rightSwitchable && leftSwitchable)stateCounter++;
            else return;

            break;

        case 1:
            if(ADC_Left_Detect < (LEFT_WHITE_DETECT) ^ ADC_Right_Detect < (RIGHT_WHITE_DETECT)) stateCounter = 2;
            else if (ADC_Left_Detect < (LEFT_WHITE_DETECT) && ADC_Right_Detect < (RIGHT_WHITE_DETECT)) {
                rFollowSpeed = -RIGHT_MIN;
                lFollowSpeed = -LEFT_MIN;
            } else {
                ClearController(&rightFollowController);
                ClearController(&leftFollowController);
            }

            if(delay(CIRCLING_TIME, 0))  stateCounter = 5;
            if(stopwatch_seconds>=TIME_TO_CIRCLE) strcpy(display_line[0]," CIRCLING ");

            Drive_Path(rFollowSpeed, lFollowSpeed,0);
            break;


        case 2:
            if(ADC_Left_Detect < LEFT_WHITE_DETECT && ADC_Right_Detect >= RIGHT_WHITE_DETECT) stateCounter = 3;
            else if(ADC_Left_Detect >= LEFT_WHITE_DETECT && ADC_Right_Detect < RIGHT_WHITE_DETECT) stateCounter = 4;
            else stateCounter = 1;

            break;

        case 3:// turn left ()
            if(ADC_Left_Detect < LEFT_WHITE_DETECT) Drive_Path((RIGHT_MIN - LF_TURN_DECREMENT), -(LEFT_MIN - LF_TURN_DECREMENT), 0);
            else if (ADC_Left_Detect >= LEFT_WHITE_DETECT && ADC_Right_Detect >= RIGHT_WHITE_DETECT) stateCounter = 1;
            else stateCounter = 4;

            break;

        case 4:
            if(ADC_Right_Detect < RIGHT_WHITE_DETECT) Drive_Path(-(RIGHT_MIN - LF_TURN_DECREMENT), (LEFT_MIN - LF_TURN_DECREMENT), 0);
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
            EMITTER_OFF;
            strcpy(display_line[0],"          ");
            break;
    }

    if(rFollowSpeed!=lFollowSpeed && stateCounter==1) P6OUT|=GRN_LED;
    else P6OUT&=~GRN_LED;
}

void Exit(int direction) {
    if (stateCounter == 0) {
        strcpy(display_line[0]," EXITING  ");
        if(rightSwitchable && leftSwitchable)stateCounter++;
    }

    if (stateCounter == 1) {
        if(direction) {
            if(Drive_Path(-STRAIGHT_RIGHT, STRAIGHT_LEFT, TURN90)) stateCounter++;
        } else {
            if(Drive_Path(STRAIGHT_RIGHT, -STRAIGHT_LEFT, TURN90)) stateCounter++;
        }
    }

    if (stateCounter == 2) {
        if(direction) {
          if(LockMotors(1, -1)) stateCounter++;  
        } else {
            if(LockMotors(-1, 1)) stateCounter++;
        }
    }

    if (stateCounter == 3) {
        if(Drive_Path(STRAIGHT_RIGHT, STRAIGHT_LEFT, 5000)) stateCounter++;
    }

    if (stateCounter == 4) {
        if(LockMotors(-1, -1)) stateCounter++;
    }

    else if (stateCounter == 5) {
        ShutoffMotors();
        stateCounter = 0 ;
        state = START;
        stopwatch_seconds = 0;
        cycle_count = 0;
    }
}

void Drive(int polR, int polL, unsigned int time) {
    switch(stateCounter) {

        case 0 :
            stateCounter++;
            break;

        case 1 :
          if(Drive_Path(polR>0?STRAIGHT_RIGHT:-STRAIGHT_RIGHT, polL>0?STRAIGHT_LEFT:-STRAIGHT_LEFT, time))stateCounter++;

            break;

        case 2 :
            if(LockMotors(-polR, -polL)) stateCounter++;

            break;

        case 3 :
            ShutoffMotors();
            stateCounter = 0 ;
            state = START;
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
            Straight(polarityRight);
            break;

        case (TURN):
            Turn(polarityRight);
            break;

        case (LINEFOLLOW):
            LineFollow(polarityRight);
            break;
            
        case (EXIT):
            Exit(polarityRight);
            break;
            
        case (DRIVE):
            Drive(polarityRight, polarityLeft, driveTime);
            break;

        default:
            break;
    }
}
