#include "msp430.h"
#include "ports.h"
#include "wheels.h"
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
    .kP = 1,// /16
    .kD = 250,// /8
    //.kI = 0,
    .error = 0,
    .lastError = 0
    //.lastIntegral = 0
};
PIDController leftFollowController = {
    .kP = 1,// /16
    .kD = 250,// /8
    //.kI = 0,
    .error = 0,
    .lastError = 0
    //.lastIntegral = 0
};


void ShutoffMotors(void) {
    ShutoffRight();
    ShutoffLeft();
}

void ShutoffRight(void) {
    RIGHT_FORWARD_SPEED = RIGHT_REVERSE_SPEED = WHEEL_OFF;
    rightSwitchable = 0;

    TB1CCTL2 &= ~CCIFG;
    TB1CCR2 = TB1R + TB1CCR2_INTERVAL;
    TB1CCTL2 |= CCIE;
}

void ShutoffLeft(void) {
    LEFT_FORWARD_SPEED = LEFT_REVERSE_SPEED = WHEEL_OFF;
    leftSwitchable = 0;

    TB1CCTL1 &= ~CCIFG;
    TB1CCR1 = TB1R + TB1CCR1_INTERVAL;
    TB1CCTL1 |= CCIE;
}

void MotorSafety(void) {
    if ((RIGHT_FORWARD_SPEED != 0 && RIGHT_REVERSE_SPEED != 0) || (LEFT_FORWARD_SPEED != 0 && LEFT_REVERSE_SPEED != 0)) {
        ShutoffMotors();
        //P1OUT |= RED_LED;
    } else {
        //P1OUT &= ~RED_LED;
    }
}

int RunRightMotor(int val) {
    if(RIGHT_REVERSE_SPEED > 0 && val > 0 || RIGHT_FORWARD_SPEED > 0 && val < 0) {
        ShutoffRight();
    }

    if (val > 0) {
        RIGHT_REVERSE_SPEED = WHEEL_OFF;

        if(rightSwitchable) RIGHT_FORWARD_SPEED = val;

        return 1;//P6IN & R_FORWARD;
    } else if (val == 0) {
        ShutoffRight();
        return rightSwitchable;
    } else {
        RIGHT_FORWARD_SPEED = WHEEL_OFF;

        if(rightSwitchable) RIGHT_REVERSE_SPEED = -val;

        return 1;//P6IN & R_REVERSE;
    }
}

int RunLeftMotor( int val) {
    if(LEFT_REVERSE_SPEED > 0 && val > 0 || LEFT_FORWARD_SPEED > 0 && val < 0) {
        ShutoffLeft();
    }

    if (val > 0) {
        LEFT_REVERSE_SPEED = WHEEL_OFF;

        if(leftSwitchable) LEFT_FORWARD_SPEED = val;

        return 1;//P6IN & L_FORWARD;
    } else if (val == 0) {
        ShutoffLeft();
        return leftSwitchable;
    } else {
        LEFT_FORWARD_SPEED = WHEEL_OFF;

        if(leftSwitchable) LEFT_REVERSE_SPEED = -val;

        return 1;//P6IN & L_REVERSE_2355;
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

int LockMotors(int polR, int polL) {
    return (Drive_Path(polR > 0 ? STRAIGHT_RIGHT : -STRAIGHT_RIGHT, polL > 0 ? STRAIGHT_LEFT : -STRAIGHT_LEFT, LOCK_TIME));
}

int LockMotorsTime(int polR, int polL, int duration) {
    return (Drive_Path(polR > 0 ? STRAIGHT_RIGHT : -STRAIGHT_RIGHT, polL > 0 ? STRAIGHT_LEFT : -STRAIGHT_LEFT, duration));
}

int Update_Ticks(int milliseconds) { // each tick is 4ms
    stopwatch_milliseconds += MS_PER_TICK;

    if(stopwatch_milliseconds >= milliseconds) {
        stopwatch_milliseconds = 0;
        return 1;
    }

    return 0;
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

int Drive_Path(int speedR, int speedL, unsigned int ticksDuration) {
    /*int successR = RunRightMotor(speedR);
    int successL = RunLeftMotor(speedL);
    int success = successR && successL;*/
    RunRightMotor(speedR);
    RunLeftMotor(speedL);

    if(ticksDuration == 0) return 0;

    if (time_change) {
        time_change = 0;

        if (/*success && */Update_Ticks(ticksDuration)) {
            ShutoffMotors();
            return 1;
        }
    }

    return 0;
}

