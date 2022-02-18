//------------------------------------------------------------------------------
//
//  Description: This file contains the Main Routine - "While" Operating System
//
//
//  Jim Carlson
//  Jan 2022
//  Built with IAR Embedded Workbench Version: (7.21.1)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#include  "functions.h"
#include  "msp430.h"
#include <string.h>
#include "macros.h"
#include "init.h"
#include "led.h"
#include "wheels.h"
#include "ports.h"
#include "switches.h"
#include "timers.h"
// Function Prototypes
void main(void);


// Global Variables
  // Global Variables
volatile char slow_input_down;
extern char display_line[4][11];
extern char *display[4];
unsigned char display_mode;
extern volatile unsigned char display_changed;
extern volatile unsigned char update_display;
extern volatile unsigned int update_display_count;
extern volatile unsigned int Time_Sequence;
extern volatile char one_time;
extern volatile unsigned int wheel_tick;
unsigned int test_value;
char chosen_direction;
char change;
volatile unsigned int Last_Time_Sequence;
volatile unsigned int cycle_count;
volatile unsigned int stopwatch_seconds;
volatile unsigned int time_change;
extern volatile unsigned int right_tick, left_tick;
//===========================================================================
// Function name: Main
//
// Description: This function contains the while loop that runs continuously
// to act for the operating system. It also calls all the functions to
// initialize the system.
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: char* display_1
// char* display_2
// slow_input_down
// control_state[CNTL_STATE_INDEX]
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void main(void){
//------------------------------------------------------------------------------
// Main Program
// This is the main routine for the program. Execution of code starts here.
// The operating system is Back Ground Fore Ground.
//
//------------------------------------------------------------------------------
  PM5CTL0 &= ~LOCKLPM5;
// Disable the GPIO power-on default high-impedance mode to activate
// previously configured port settings

  Init_Ports();                        // Initialize Ports
  Init_Clocks();                       // Initialize Clock System
  Init_Conditions();                   // Initialize Variables and Initial Conditions
  Init_Timers();                       // Initialize Timers
  Init_LCD();                          // Initialize LCD

  // Place the contents of what you want on the display, in between the quotes
// Limited to 10 characters per line
  //strcpy(display_line[0], "   NCSU   ");
  //strcpy(display_line[1], " WOLFPACK ");
  //strcpy(display_line[2], "  ECE306  ");
  //strcpy(display_line[3], "  GP I/O  ");
  strcpy(display_line[0], "WAITING...");
  strcpy(display_line[1], "          ");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = TRUE;

//------------------------------------------------------------------------------
// Begining of the "While" Operating System
//------------------------------------------------------------------------------
  while(ALWAYS) {                       // Can the Operating system run
    Display_Process();                  // Update Display
    StateMachine();                     // Run wheels state machine
    MotorSafety();
    P3OUT ^= TEST_PROBE;               // Change State of TEST_PROBE OFF
    if(Last_Time_Sequence!=Time_Sequence){
      Last_Time_Sequence=Time_Sequence;
      cycle_count++;
      time_change = 1;
      if(cycle_count == 200){
        cycle_count = 0;
        stopwatch_seconds++;
      }
    }
   }
    
//------------------------------------------------------------------------------
}
