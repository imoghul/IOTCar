#include "msp430.h"
#include "switches.h"
#include "ports.h"
#include "wheels.h"
#include "macros.h"
#include "sm.h"
#include "timers.h"
#include "detectors.h"
#include "serial.h"
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
extern volatile char USB0_Char_Rx_Ring[],USB1_Char_Rx_Ring[];
extern unsigned volatile int tx0_index,tx1_index;

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
        
        //strcpy((char*)USB0_Char_Tx,(char*)USB0_Char_Rx_Ring);
        //strcpy((char*)USB1_Char_Tx,(char*)USB1_Char_Rx_Ring);
        for(int i = 0;i<10;++i)
          display_line[1][i] = ' ';
        
        int i;
        for(i = 0; i<10 && display_line[3][i]!=0;++i)
           display_line[1][i] = display_line[3][i];
        for(;i<10;++i)
           display_line[1][i] = ' ';
        display_line[1][10] = 0;
        
        strcpy(display_line[3], "          ");
        tx0_index=0;
        UCA0IE |= UCTXIE;
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

