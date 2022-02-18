#include "msp430.h"
#include "timers.h"
#include "ports.h"
#include "macros.h"

volatile unsigned int Time_Sequence;
extern volatile unsigned char update_display;
volatile unsigned int timer0Counter;
volatile unsigned int backliteCounter;
volatile unsigned int debounce_count1, debounce_count2;
volatile unsigned int debouncing1, debouncing2;
volatile unsigned int debounce_thresh1=10, debounce_thresh2=10;
void Init_Timers(void){
  Init_Timer_B0();
}

void Init_Timer_B0(void) {
  TB0CTL = TBSSEL__SMCLK; // SMCLK source
  TB0CTL |= TBCLR; // Resets TB0R, clock divider, count direction
  TB0CTL |= MC__CONTINOUS; // Continuous up
  TB0CTL |= ID__2; // Divide clock by 2
  TB0EX0 = TBIDEX__8; // Divide clock by an additional 8
  TB0CCR0 = TB0CCR0_INTERVAL; // CCR0
  TB0CCTL0 |= CCIE; // CCR0 enable interrupt
  TB0CCR1 = TB0CCR1_INTERVAL; // CCR1
  //TB0CCTL1 |= CCIE; // CCR1 enable interrupt
  TB0CCR2 = TB0CCR2_INTERVAL; // CCR2
  TB0CCTL2 |= CCIE; // CCR2 enable interrupt
  TB0CTL &= ~TBIE; // Disable Overflow Interrupt
  TB0CTL &= ~TBIFG; // Clear Overflow Interrupt flag
}


//===========================================================================
// Function name: Timer0_B0_ISR
//
// Description: Increments Time_Sequence and update_display
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
__interrupt void Timer0_B0_ISR(void){
//------------------------------------------------------------------------------
// TimerB0 0 Interrupt handler
//----------------------------------------------------------------------------
  if(++timer0Counter==UPDATE_DISPLAY_TIMER_COUNT+1) timer0Counter = 1;
  if(timer0Counter%TIME_SEQUENCE_TIMER_COUNT==0)
    if(Time_Sequence++ == TIME_SEQUENCE_MAX) Time_Sequence = 0;
  if(timer0Counter%UPDATE_DISPLAY_TIMER_COUNT==0)
    update_display=1;
  
  
  TB0CCR0 += TB0CCR0_INTERVAL; // Add Offset to TBCCR0
//----------------------------------------------------------------------------
}


//===========================================================================
// Function name: TIMER0_B1_ISR
//
// Description: Timer 1 handles switch debounce, and Timer 2 handles 
// LCD blinking
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
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void){
  //----------------------------------------------------------------------------
  // TimerB0 1-2, Overflow Interrupt Vector (TBIV) handler
  //----------------------------------------------------------------------------
  switch(__even_in_range(TB0IV,14)){
    case 0: break; // No interrupt
    case 2: // CCR1 not used
      if(debouncing1) debounce_count1++;
      if(debouncing2) debounce_count2++;
      if (debounce_count1 >= debounce_thresh1){
        debounce_count1 = 0;
        debouncing1 = FALSE;
        P4IE |= SW1;
      }
      if (debounce_count2 >= debounce_thresh2){
        debounce_count2 = 0;
        debouncing2 = FALSE;
        P2IE |= SW2;
      }
      if(debouncing1==FALSE && debouncing2==FALSE) {
        TB0CCTL1 &= ~CCIE; // CCR1 disable interrupt
        TB0CCTL2 |= CCIE;
        backliteCounter = 0;
      }
      TB0CCR1 += TB0CCR1_INTERVAL; // Add Offset to TBCCR1
      break;
    case 4: // CCR2 not used

        if (++backliteCounter==10){
          backliteCounter = 0;
          P3OUT ^= LCD_BACKLITE;
        }
      
      TB0CCR2 += TB0CCR2_INTERVAL; // Add Offset to TBCCR2
      break;
    case 14: // overflow
      
      break;
    default: break;
  }
  //----------------------------------------------------------------------------
}