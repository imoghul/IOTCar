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
    .kP = -1,
    .kD = 1,
    .kI = 0,
    .error = 0,
    .lastError = 0,
    .lastIntegral = 0
};
PIDController leftFollowController = {
    .kP = -1,
    .kD = 1,
    .kI = 0,
    .error = 0,
    .lastError = 0,
    .lastIntegral = 0
};

PIDController rightAdjustController = {
    .kP = 20,
    .kD = 0,
    .kI = 5,
    .error = 0,
    .lastError = 0,
    .lastIntegral = 0
};
PIDController leftAdjustController = {
    .kP = 20,
    .kD = 0,
    .kI = 5,
    .error = 0,
    .lastError = 0,
    .lastIntegral = 0
};


void ShutoffMotors(void) {
    ShutoffRight();
    ShutoffLeft();
}

void ShutoffRight(void) {
    RIGHT_FORWARD_SPEED = RIGHT_REVERSE_SPEED = WHEEL_OFF;
    rightSwitchable = 0;
    TB1CCTL1 &= ~CCIFG;
    TB1CCR1 = TB1R + TB1CCR1_INTERVAL;
    TB1CCTL1 |= CCIE;
}

void ShutoffLeft(void) {
    LEFT_FORWARD_SPEED = LEFT_REVERSE_SPEED = WHEEL_OFF;
    leftSwitchable = 0;
    TB1CCTL2 &= ~CCIFG;
    TB1CCR2 = TB1R + TB1CCR2_INTERVAL;
    TB1CCTL2 |= CCIE;
}

void MotorSafety(void) {

    if ((((P6IN & R_FORWARD) && (P6IN & R_REVERSE)) || ((P6IN & L_FORWARD) && (P6IN & L_REVERSE_2355)))
            ||
            ((RIGHT_FORWARD_SPEED != 0 && RIGHT_REVERSE_SPEED != 0) || (LEFT_FORWARD_SPEED != 0 && LEFT_REVERSE_SPEED != 0))) {
        ShutoffMotors();
        P1OUT |= RED_LED;
    } else {
        P1OUT &= ~RED_LED;
    }
}

int RunRightMotor(int val) {
    if(RIGHT_REVERSE_SPEED > 0 && val > 0 || RIGHT_FORWARD_SPEED > 0 && val < 0) {
        ShutoffRight();
    }

    if (val > 0) {
        RIGHT_REVERSE_SPEED = WHEEL_OFF;

        if(rightSwitchable) RIGHT_FORWARD_SPEED = val;

        return P6IN & R_FORWARD;
    } else if (val == 0) {
        ShutoffRight();
        return rightSwitchable;
    } else {
        RIGHT_FORWARD_SPEED = WHEEL_OFF;

        if(rightSwitchable) {
            RIGHT_REVERSE_SPEED = -val;
        }

        return P6IN & R_REVERSE;
    }

    //MotorSafety();
}

int RunLeftMotor( int val) {
    if(LEFT_REVERSE_SPEED > 0 && val > 0 || LEFT_FORWARD_SPEED > 0 && val < 0) {
        ShutoffLeft();
    }

    if (val > 0) {
        LEFT_REVERSE_SPEED = WHEEL_OFF;

        if(leftSwitchable) {
            LEFT_FORWARD_SPEED = val;
        }

        return P6IN & L_FORWARD;
    } else if (val == 0) {
        ShutoffLeft();
        return leftSwitchable;
    } else {
        LEFT_FORWARD_SPEED = WHEEL_OFF;

        if(leftSwitchable) {
            LEFT_REVERSE_SPEED = -val;
        }

        return P6IN & L_REVERSE_2355;
    }

    //MotorSafety();
}

int LockMotors(int polR, int polL) {
    return (Drive_Path(polR * STRAIGHT_RIGHT, polL * STRAIGHT_LEFT, 5));
}

int LockMotorsTime(int polR, int polL, int duration) {
    return (Drive_Path(polR * STRAIGHT_RIGHT, polL * STRAIGHT_LEFT, duration));
}

int Update_Ticks(int milliseconds) { // each tick is 4ms so 250*ticks = milliseconds
    if(++wheel_periods > TIME_SEQUENCE_MAX) {
        wheel_periods = 0;
        stopwatch_milliseconds++;
    }
    if(stopwatch_milliseconds>=milliseconds){
      stopwatch_milliseconds = 0;
      return 1;
    }

    return 0;
}

int Drive_Path(int speedR, int speedL, unsigned int ticksDuration) {
    int successR = RunRightMotor(speedR);
    int successL = RunLeftMotor(speedL);

    if(ticksDuration == 0) return successR && successL;

    if (time_change) {
        time_change = 0;

        if (successR && successL && Update_Ticks(ticksDuration)) {
            ShutoffMotors();
            return 1;
        }
    }

    return 0;
}


