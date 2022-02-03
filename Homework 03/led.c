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
extern volatile int cycle_count;


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
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Display_Process(void){
  if(update_display){
    update_display = 0;
    if(display_changed){
      display_changed = 0;
      Display_Update(0,0,0,0);
    }
  }
}


//===========================================================================
// Function name: Carlson_StateMachine
//
// Description: Toggle LEDs
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: one_time
// Time_Sequence
// display_changed
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Carlson_StateMachine(void){
  switch(cycle_count){
  
  }
    switch(Time_Sequence){
      case 250:                        //
        if(one_time){
          Init_LEDs();
          lcd_BIG_mid();
          display_changed = 1;
          one_time = 0;
        }
        Time_Sequence = 0;             //
        break;
      case 200:                        //
        if(one_time){
          P6OUT |= GRN_LED;            // Change State of LED 5
          one_time = 0;
        }
        break;
      case 150:                         //
        if(one_time){
          P1OUT |= RED_LED;            // Change State of LED 4
          P6OUT &= ~GRN_LED;           // Change State of LED 5
          one_time = 0;
        }
        break;
      case 100:                         //
        if(one_time){
//          lcd_4line();
          lcd_BIG_bot();
          P6OUT |= GRN_LED;            // Change State of LED 5
          display_changed = 1;
          one_time = 0;
        }
        break;
      case  50:                        //
        if(one_time){
          P1OUT &= ~RED_LED;           // Change State of LED 4
          P6OUT &= ~GRN_LED;           // Change State of LED 5
          one_time = 0;
        }
        break;                         //
      default: break;
    }
}