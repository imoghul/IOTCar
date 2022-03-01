#include "msp430.h"
#include "switches.h"
#include "ports.h"
#include "wheels.h"
volatile unsigned int sw1Okay, sw2Okay;
volatile unsigned int count_debounce_SW1, count_debounce_SW2;
volatile unsigned int sw1_pos, sw2_pos;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_seconds;
extern volatile char state;


#pragma vector=PORT4_VECTOR
__interrupt void switchP4_interrupt(void) {
    if(P4IFG & SW1) {
        P4IFG &= ~SW1;

        if(state == START) {
            stopwatch_seconds = 0;
            cycle_count = 0;
            state = WAIT;
        }
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void switchP2_interrupt(void) {
    if(P2IFG & SW2) {
        P2IFG &= ~SW2;
        P1OUT |= RED_LED;
    }
}

