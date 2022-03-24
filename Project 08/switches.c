#include "msp430.h"
#include "switches.h"
#include "ports.h"
#include "wheels.h"
#include "macros.h"
#include "sm.h"
#include "timers.h"
#include "detectors.h"
#include <string.h>

volatile unsigned int sw1Okay, sw2Okay;
volatile unsigned int count_debounce_SW1, count_debounce_SW2;
volatile unsigned int sw1_pos, sw2_pos;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_seconds;
extern volatile char state;
extern volatile unsigned int debounce_count1, debounce_count2;
extern volatile unsigned int debouncing1, debouncing2;
extern volatile unsigned int backliteBlinking;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
volatile unsigned int calibrationMode;
extern volatile char USB0_Char_Tx[],USB1_Char_Tx[];
extern volatile char USB0_Char_Rx[],USB1_Char_Rx[];
extern unsigned volatile UCA0_index,UCA1_index;

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
__interrupt void switchP4_interrupt(void) {
    if(P4IFG & SW1 && debouncing1 == FALSE) {
        P4IE &= ~SW1;
        P4IFG &= ~SW1;
        TB0CCTL1 &= ~CCIFG;
        TB0CCR1 = TB0R + TB0CCR1_INTERVAL;
        TB0CCTL1 |= CCIE; // CCR1 enable interrupt
        debouncing1 = TRUE;
        //debounce_count1 = 0;

        // Actual Code
        
        strcpy((char*)USB0_Char_Tx,(char*)USB0_Char_Rx);
        strcpy((char*)USB1_Char_Tx,(char*)USB1_Char_Rx);
        strcpy(display_line[1],display_line[3]);
        strcpy(display_line[3], "          ");
        
        UCA0_index = 0;
        UCA0IE |= UCTXIE;
        UCA0TXBUF = USB0_Char_Tx[0];
        
        UCA1_index = 0;
        UCA1IE |= UCTXIE;
        UCA1TXBUF = USB1_Char_Tx[0];
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
__interrupt void switchP2_interrupt(void) {
    if(P2IFG & SW2 && debouncing2 == FALSE) {
        P2IE &= ~SW2;
        P2IFG &= ~SW2;
        TB0CCTL2 &= ~CCIFG;
        TB0CCR2 = TB0R + TB0CCR2_INTERVAL;
        TB0CCTL2 |= CCIE; // CCR1 enable interrupt
        debouncing2 = TRUE;
        // Actual Code
        // 460800
        if(UCA0BRW == 4 && UCA0MCTLW == 0x5551){ // if is 11520 change to 460800
          UCA0BRW = 1;
          UCA0MCTLW = 0x4A11;
          UCA1BRW = 1;
          UCA1MCTLW = 0x4A11;
        }
        else if(UCA0BRW == 1 && UCA0MCTLW == 0x4A11){ // if is 460800 change to 115200
          UCA0BRW = 4;
          UCA0MCTLW = 0x5551;
          UCA1BRW = 4;
          UCA1MCTLW = 0x5551;
        }
    }
}

