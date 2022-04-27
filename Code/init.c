//==============================================================================
// File Name : init.c
//
// Description: This file contains initialization functions before mission code
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: 7.21.1
//==============================================================================

#include "init.h"
#include "msp430.h"
#include "macros.h"
#include "functions.h"
#include "ports.h"

extern char display_line[4][11];
extern char *display[4];
extern volatile unsigned char update_display;

//===========================================================================
// Function name: Init_LEDs
//
// Description: Initialize LED ports
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_LEDs(void) {
    //------------------------------------------------------------------------------
    // LED Configurations
    //------------------------------------------------------------------------------
    // Turns on both LEDs
    P1OUT &= ~RED_LED;
    P6OUT &= ~GRN_LED;
    //------------------------------------------------------------------------------
}

//===========================================================================
// Function name: Init_Conditions
//
// Description: Initialize values for mission code
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: display_line
// update_display
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Conditions(void) {
    //------------------------------------------------------------------------------

    int i;

    for(i = 0; i < 11; i++) {
        display_line[0][i] = RESET_STATE;
        display_line[1][i] = RESET_STATE;
        display_line[2][i] = RESET_STATE;
        display_line[3][i] = RESET_STATE;
    }

    display_line[0][10] = 0;
    display_line[1][10] = 0;
    display_line[2][10] = 0;
    display_line[3][10] = 0;

    display[0] = &display_line[0][0];
    display[1] = &display_line[1][0];
    display[2] = &display_line[2][0];
    display[3] = &display_line[3][0];
    update_display = 0;


    // Interrupts are disabled by default, enable them.
    enable_interrupts();
    //------------------------------------------------------------------------------
}
