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
#include "adc.h"
#include "detectors.h"
#include "sm.h"
#include "menu.h"
#include "serial.h"
#include "iot.h"
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
unsigned int Last_Time_Sequence;
volatile unsigned int cycle_count;
volatile unsigned int stopwatch_seconds;
volatile unsigned int time_change;
extern volatile unsigned int right_tick, left_tick;
extern char adc_char[5];
extern volatile unsigned int ADC_Left_Detect, ADC_Right_Detect;
extern char movingDirection;
extern char enteringDirection;
extern float timeElapsed;
extern char state;
extern volatile unsigned int stopwatchUpdated;
unsigned volatile UCA0_index,UCA1_index;
extern volatile char USB0_Char_Tx[];
extern volatile unsigned int pb0_buffered;
extern volatile unsigned int usb0_rx_wr,usb0_rx_rd;
extern volatile unsigned int serialState;
extern menu resistor,mainMenu;
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

void main(void) {
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
    Init_REF();
    Init_DAC();
    Init_ADC();
    Init_Serial_UCA();
    EmitterOn();
    // Place the contents of what you want on the display, in between the quotes
    // Limited to 10 characters per line
    strcpy(display_line[0], "          ");
    strcpy(display_line[1], "          ");
    strcpy(display_line[2], "          ");
    strcpy(display_line[3], "          ");
    display_changed = TRUE;

    //------------------------------------------------------------------------------
    // Begining of the "While" Operating System
    //------------------------------------------------------------------------------
    while(ALWAYS) {                       // Can the Operating system run
        Display_Process();                  // Update Display
        SerialProcess();
        Init_IOT();
        //DetectMovement();
        //StateMachine();                     // Run wheels state machine
        //MenuProcess();
        MotorSafety();
        P3OUT ^= TEST_PROBE;               // Change State of TEST_PROBE OFF
        /*if(movingDirection == MOVING_RIGHT) strcpy(display_line[1], "  RIGHT   ");
        else if(movingDirection == MOVING_STRAIGHT) strcpy(display_line[1], " STRAIGHT ");
        else if (movingDirection == MOVING_LEFT) strcpy(display_line[1], "   LEFT   ");
        else if (movingDirection == NOT_MOVING) strcpy(display_line[1], "NOT MOVING");*/

        /*if(stopwatchUpdated){
          stopwatchUpdated = 0;
          HEXtoBCD((int)timeElapsed,3,0);
          display_line[3][4]='.';
          display_line[3][5]=(int)(10*(timeElapsed-(int)timeElapsed))+0x30;
          display_changed = 1;
        }*/
        
        //strcpy(display_line[2], "   Baud   ");
        
        
        if(UCA0BRW == 4 && UCA0MCTLW == 0x5551){
          strcpy(display_line[2], "  115200  ");
          display_changed=1;
        }
        else if(UCA0BRW == 52 && UCA0MCTLW == 0x4911){
          strcpy(display_line[2], "   9600   ");
          display_changed=1;
        }
        
        /*if(serialState==1){
          strcpy(display_line[0], " TRANSMIT ");
          display_changed=1;
        }
        else if(serialState==2) {
          strcpy(display_line[0], " RECIEVED ");
          display_changed=1;
        }*/
          
        
        if(cycle_count==0){
          display_changed = 1;
        }

        if(Last_Time_Sequence != Time_Sequence) {
            Last_Time_Sequence = Time_Sequence;
            cycle_count++;
            time_change = 1;

            if(cycle_count == TIME_SEQUENCE_MAX) {
                cycle_count = 0;
                stopwatch_seconds++;
            }
        }
    }

    //------------------------------------------------------------------------------
}

