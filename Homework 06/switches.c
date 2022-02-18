#include "msp430.h"
#include "switches.h"
#include "ports.h"
#include "wheels.h"
#include "macros.h"
#include "timers.h"

volatile unsigned int sw1Okay, sw2Okay;
volatile unsigned int count_debounce_SW1, count_debounce_SW2;
volatile unsigned int sw1_pos, sw2_pos;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_seconds;
extern volatile char state;
extern volatile unsigned int debounce_count1, debounce_count2;
extern volatile unsigned int debouncing1, debouncing2;
extern volatile unsigned int backliteBlinking;

//===========================================================================
// Function name: switchP4_interrupt
//
// Description: This is the switch 1 interrupt, turns off LCD_BACKLITE and its 
// blinking timer
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

#pragma vector=PORT4_VECTOR
__interrupt void switchP4_interrupt(void){
  if(P4IFG & SW1){
    P4IFG &= ~SW1;
    TB0CCTL1 |= CCIE; // CCR1 enable interrupt
    debouncing1 = TRUE;
    P4IE &= ~SW1;
    debounce_count1 = 0;
    TB0CCR1 = TB0R + TB0CCR1_INTERVAL;
    // Actual Code
    //if(state == START){
    //    stopwatch_seconds = 0;
    //    cycle_count = 0;
    //    state = WAIT;
    //}
    P3OUT &= ~LCD_BACKLITE;
    backliteBlinking = FALSE;//TB0CCTL2 &= ~CCIE;
  }
}

//===========================================================================
// Function name: switchP4_interrupt
//
// Description: This is the switch 2 interrupt, turns off LCD_BACKLITE and its 
// blinking timer
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
#pragma vector=PORT2_VECTOR
__interrupt void switchP2_interrupt(void){
  if(P2IFG & SW2){
    P2IFG &= ~SW2;
    TB0CCTL2 |= CCIE; // CCR1 enable interrupt
    debouncing2 = TRUE;
    P2IE &= ~SW2;
    debounce_count2 = 0;
    TB0CCR2 = TB0R + TB0CCR2_INTERVAL;
    // Actual Code
    //P1OUT |= RED_LED;
    P3OUT &= ~LCD_BACKLITE;
    backliteBlinking = FALSE;//TB0CCTL2 &= ~CCIE;
  }
}

