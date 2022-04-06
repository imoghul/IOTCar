//==============================================================================
// File Name : led.c
//
// Description: This file contains mission code
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: 7.21.1
//==============================================================================

#include "led.h"
#include "init.h"
#include "functions.h"
#include "ports.h"


extern volatile unsigned char update_display;
extern volatile unsigned char display_changed;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;


//===========================================================================
// Function name: Display_Process
//
// Description: Display output to LCD
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: display_changed
// update_display
//
// Author: Jim Carlson
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Display_Process(void) {
    if(update_display) {
        update_display = 0;

        if(display_changed) {
            display_changed = 0;
            Display_Update(0, 0, 0, 0);
        }
    }
}

