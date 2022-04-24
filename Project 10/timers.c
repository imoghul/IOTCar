#include "msp430.h"
#include "timers.h"
#include "ports.h"
#include "macros.h"
#include "wheels.h"
#include "sm.h"

volatile unsigned int Time_Sequence;
extern volatile unsigned char update_display;
volatile unsigned long timer0Counter;
volatile unsigned int backliteCounter;
unsigned int debounce_count1, debounce_count2;
volatile unsigned int debouncing1, debouncing2;
volatile unsigned int debounce_thresh1 = SWITCH_DEBOUNCE_THRESH, debounce_thresh2 = SWITCH_DEBOUNCE_THRESH;
volatile unsigned int checkAdc;
extern volatile char state;
extern volatile unsigned int rightSwitchable, leftSwitchable;
volatile int timeElapsedSeconds, timeElapsedMilliseconds;
volatile unsigned int stopwatchUpdated;
extern char receievedFromPC;
extern char commandsReceieved;
char pingCounter;
volatile char pingFlag;

//===========================================================================
// Function name: Init_Timrs
//
// Description: This function is used to initialize all timers
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Init_Timers(void) {
    Init_Timer_B0();
    Init_Timer_B1();
    Init_Timer_B3();
}

//===========================================================================
// Function name: Init_Timer_B0
//
// Description: This function is used to initialized timer B0
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Init_Timer_B0(void) {
    /*TB0CTL = TBSSEL__SMCLK; // SMCLK source
    TB0CTL |= TBCLR; // Resets TB0R, clock divider, count direction
    TB0CTL |= MC__CONTINOUS; // Continuous up
    TB0CTL |= ID__2; // Divide clock by 2*/

    TB0CTL = TBSSEL__SMCLK | TBCLR | MC__CONTINOUS | ID__2;

    TB0EX0 = TBIDEX__8; // Divide clock by an additional 8

    TB0CCR0 = TB0CCR0_INTERVAL; // CCR0
    TB0CCTL0 |= CCIE; // CCR0 enable interrupt

    TB0CCR1 = TB0CCR1_INTERVAL; // CCR1
    //TB0CCTL1 |= CCIE; // CCR1 enable interrupt

    TB0CCR2 = TB0CCR2_INTERVAL; // CCR2
    //TB0CCTL2 |= CCIE; // CCR2 enable interrupt

    TB0CTL &= ~TBIE & ~TBIFG; // Disable Overflow Interrupt
    //TB0CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}

//===========================================================================
// Function name: Init_Timer_B1
//
// Description: This function is used to initialized timer B1
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Init_Timer_B1(void) {
    /*TB1CTL = TBSSEL__SMCLK; // SMCLK source
    TB1CTL |= TBCLR; // Resets TB0R, clock divider, count direction
    TB1CTL |= MC__CONTINOUS; // Continuous up
    TB1CTL |= ID__4; // Divide clock by 4*/

    TB1CTL = TBSSEL__SMCLK | TBCLR | MC__CONTINOUS | ID__4;

    TB1EX0 = TBIDEX__8; // Divide clock by an additional 8

    TB1CCR0 = TB1CCR0_INTERVAL; // CCR0
    TB1CCTL0 |= CCIE; // CCR0 enable interrupt

    //TB1CCR1 = TB1CCR1_INTERVAL; // CCR1
    //TB1CCTL1 |= CCIE; // CCR1 enable interrupt

    //TB1CCR2 = TB1CCR2_INTERVAL; // CCR2
    //TB1CCTL2 |= CCIE; // CCR2 enable interrupt

    TB1CTL &= ~TBIE & ~TBIFG; // Disable Overflow Interrupt
    //TB1CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}
//===========================================================================
// Function name: Init_Timer_B3
//
// Description: This function is used to initialized timer B3
//
// Passed : direction
// Locals: no locals used
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void Init_Timer_B3(void) {
    /*TB3CTL = TBSSEL__SMCLK;
    TB3CTL |= MC__UP;
    TB3CTL |= TBCLR;*/

    TB3CTL = TBCLR | MC__UP | TBSSEL__SMCLK;

    TB3CCR0 = WHEEL_PERIOD;

    TB3CCTL1 = OUTMOD_7;
    RIGHT_FORWARD_SPEED = WHEEL_OFF;

    TB3CCTL2 = OUTMOD_7;
    LEFT_FORWARD_SPEED = WHEEL_OFF;

    TB3CCTL3 = OUTMOD_7;
    RIGHT_REVERSE_SPEED = WHEEL_OFF;

    TB3CCTL4 = OUTMOD_7;
    LEFT_REVERSE_SPEED = WHEEL_OFF;
}


//===========================================================================
// Function name: Timer0_B0_ISR
//
// Description: Timer B0 isr, Increments Time_Sequence and enables adc preiodically
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
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void) {
    //------------------------------------------------------------------------------
    // TimerB0 0 Interrupt handler
    //----------------------------------------------------------------------------
    if(Time_Sequence++ == TIME_SEQUENCE_MAX) Time_Sequence = BEGINNING;

    if(++timer0Counter >= CHECK_ADC_TIMER_COUNT ) { // 56 ms
        timer0Counter = BEGINNING;
        ADCCTL0 |= ADCSC;
    }

    TB0CCR0 += TB0CCR0_INTERVAL; // Add Offset to TBCCR0
    //----------------------------------------------------------------------------
}


//===========================================================================
// Function name: TIMER0_B1_ISR
//
// Description: Timer B0 isr, Timers 1 and 2 handles switch debounce
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: debouncing1, debouncing2, debounce_count1, debounce_count2, 
// debounce_thresh1, debounce_thresh2
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void) {
    //----------------------------------------------------------------------------
    // TimerB0 1-2, Overflow Interrupt Vector (TBIV) handler
    //----------------------------------------------------------------------------
    switch(__even_in_range(TB0IV, 14)) {
        case 0:
            break; // No interrupt

        case 2: // CCR1 not used
            if(debouncing1 == TRUE) debounce_count1++;

            if (debounce_count1 > debounce_thresh1) {
                debounce_count1 = BEGINNING;
                debouncing1 = FALSE;
                P4IE |= SW1;
                TB0CCTL1 &= ~CCIE;
            }

            TB0CCR1 += TB0CCR1_INTERVAL; // Add Offset to TBCCR1

            break;

        case 4: // CCR2 not used
            if(debouncing2 == TRUE) debounce_count2++;

            if (debounce_count2 > debounce_thresh2) {
                debounce_count2 = BEGINNING;
                debouncing2 = FALSE;
                P2IE |= SW2;
                TB0CCTL2 &= ~CCIE;
            }

            TB0CCR2 += TB0CCR2_INTERVAL; // Add Offset to TBCCR2

            break;

        case 14: // overflow

            break;

        default:
            break;
    }

    //----------------------------------------------------------------------------
}

//===========================================================================
// Function name: TIMER1_B0_ISR
//
// Description: Timer B1 isr; enables IOT, updates stopwatch, updates ping
// flag, sets update_display
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: commandReceieved, state, stopwatchUpdated, timeElapsedMilliseconds
// pingCounter, pingFlag, update_display
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void) {
    //------------------------------------------------------------------------------
    // TimerB0 0 Interrupt handler
    //----------------------------------------------------------------------------
    P3OUT |= IOT_EN_CPU;

    if(commandsReceieved && state != DONE) {
        stopwatchUpdated = true;
        timeElapsedMilliseconds += STOP_WATCH_INCREMENT;

        if(timeElapsedMilliseconds >= MS_IN_SEC) {
            timeElapsedMilliseconds = BEGINNING;
            timeElapsedSeconds++;
        }
    }

    if(pingCounter++ >= PING_COUNT_MAX) {
        pingCounter = BEGINNING;
        pingFlag = true;
    }

    update_display = true;
    TB1CCR0 += TB1CCR0_INTERVAL;
    //----------------------------------------------------------------------------
}

//===========================================================================
// Function name: TIMER1_B1_ISR
//
// Description: Timer B0 isr, eanbles motor switching after delay
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: leftSwitchable, rightSwitchable
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

#pragma vector=TIMER1_B1_VECTOR
__interrupt void TIMER1_B1_ISR(void) {
    //----------------------------------------------------------------------------
    // TimerB0 1-2, Overflow Interrupt Vector (TBIV) handler
    //----------------------------------------------------------------------------
    switch(__even_in_range(TB1IV, 14)) {
        case 0:
            break; // No interrupt

        case 2: // Left Motor
            leftSwitchable = true;
            TB1CCTL1 &= ~CCIE;

            break;

        case 4: // Right Motor

            rightSwitchable = true;
            TB1CCTL2 &= ~CCIE;

            break;

        case 14: // overflow

            break;

        default:
            break;
    }

    //----------------------------------------------------------------------------
}
