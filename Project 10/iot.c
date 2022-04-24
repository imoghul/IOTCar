#include "iot.h"
#include "msp430.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include "wheels.h"
#include "utils.h"
#include "serial.h"
#include "macros.h"
#include "ports.h"
#include "sm.h"

char iot_setup_state = BOOT_UP;
extern volatile char USB0_Char_Tx[];
extern unsigned volatile int pb0_buffered;
extern volatile char USB0_Char_Rx_Process[];
extern volatile char receievedFromPC;
char SSID[SSID_LEN + 1];
char IP[IP_LEN + 1];
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char dotFound;
int midIndex;
command CommandBuffer[COMMAND_BUFFER_LEN];
char cb_index;
extern volatile int stateCounter;
char commandsReceieved;
char currentStation;
extern int commandDisplayCounter;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;

extern volatile char state;
extern volatile int stateCounter, driveStateCounter;
extern volatile char nextState;

extern int speedRight, speedLeft;
extern unsigned int driveTime;

extern volatile char pingFlag;

command emptyCommand = {0, 0};
command currCommand;


//===========================================================================
// Function name: Init_IOT
//
// Description: This function initializes the IOT, and gets it connected to
// the local network, and ready to communicate with other devices, it also
// ensures it stays connected with periodic pings
//
// Passed : no variables passed
// Locals: isTransmitting
// Returned: whether the iot module is available or not
// Globals: iot_setup_state, pingFlag,pb0_buffered
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int Init_IOT(void) {
    int isTransmitting = UCA0IE & UCTXIE;

    switch(iot_setup_state) {
        case (BOOT_UP):
            waitForReady();
            break;

        case CIPMUX_Tx:
            SendIOTCommand(ALLOW_MULTIPLE_CONNECTIONS, CIPMUX_Rx);
            break;

        case CIPMUX_Rx:
            if(isTransmitting) break; // wait for the Tx to completely transmit

            if(pb0_buffered) { // wait for pb to finish buffering
                iot_setup_state = CIPSERVER_Tx;
                clearProcessBuff_0();
            }

            break;

        case CIPSERVER_Tx:
            SendIOTCommand(START_SERVER, CIPSERVER_Rx);
            break;

        case CIPSERVER_Rx:
            if(isTransmitting) break;

            if(pb0_buffered) {
                iot_setup_state = GET_SSID_Tx;
                clearProcessBuff_0();
            }

            break;

        case GET_SSID_Tx:
            SendIOTCommand(SSID_COMMAND, GET_SSID_Rx);
            break;

        case GET_SSID_Rx:
            if(isTransmitting) break;

            getSSID();

            break;

        case GET_IP_Tx:
            SendIOTCommand(IP_COMMAND, GET_IP_Rx);
            break;

        case GET_IP_Rx:
            if(isTransmitting) break;

            getIP();
            displayNetworkInfo();

            break;

        default:
            if(pingFlag) {
                pingFlag = 0;
                SendIOTCommand(PING_COMMAND, IOT_SETUP_FINISHED);
            }

            return 1;
            break;
    }

    return 0;
}

//===========================================================================
// Function name: waitForRead
//
// Description: This function waits for the final boot up output from the iot
// before moving on in the initialization process for the iot
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: pb0_buffered, USB0_Char_Rx_Process, iot_setup_state
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void waitForReady(void) {
    if(pb0_buffered) {
        if(!strcmp((char*)USB0_Char_Rx_Process, BOOT_RESPONSE)) iot_setup_state = CIPMUX_Tx;

        clearProcessBuff_0();
    }
}

//===========================================================================
// Function name: SendIOTCommand
//
// Description: This function loads a command to transmit to the IOT
// into the tx buffer, and enables the interrupt to begin the transmition
// it then transitions to the next state in the setup process
//
// Passed : command, nextState
// Locals: no variables declared
// Returned: no values returned
// Globals: iot_setup_state
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void SendIOTCommand(char* command, char nextState) {
    strcpy((char*)USB0_Char_Tx, command);
    USCI_A0_transmit();
    iot_setup_state = nextState;
}

//===========================================================================
// Function name: getSSID
//
// Description: This function waits for and parses the SSID
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: SSID, USB0_Char_Rx_Process,iot_setup_state
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void getSSID(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, SSID_RESPONSE)) {
            int i;

            for(i = 0; i <= SSID_LEN && USB0_Char_Rx_Process[i + SSID_RESPONSE_LEN + 1] != '\"'; ++i) SSID[i] = USB0_Char_Rx_Process[i + SSID_RESPONSE_LEN + 1];

            SSID[i + SSID_RESPONSE_LEN + 2] = '\0'; // set the end of the SSID to null
            SSID[SSID_LEN] = '\0'; //  set end of the array to null

            iot_setup_state = GET_IP_Tx;
        } else iot_setup_state = GET_SSID_Tx;

        clearProcessBuff_0();
    }
}

//===========================================================================
// Function name: getIP
//
// Description: This function waits for and parses the IP address
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: IP, USB0_Char_Rx_Process,iot_setup_state, midIndex
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void getIP(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, IP_RESPONSE)) {
            int i;

            for(i = 0; i <= IP_LEN && USB0_Char_Rx_Process[i + IP_RESPONSE_LEN + 1] != '"'; ++i) {
                IP[i] = USB0_Char_Rx_Process[i + IP_RESPONSE_LEN + 1];

                if(IP[i] == '.') {
                    if(++dotFound == MID_DOT) midIndex = i;
                }
            }

            IP[i + IP_RESPONSE_LEN + 2] = '\0'; // set end of IP to null
            IP[IP_LEN] = '\0'; // set end of array to null
            IP[midIndex] = '\0'; // set mid of I to null

            iot_setup_state = IOT_SETUP_FINISHED;
        } else iot_setup_state = GET_IP_Tx;

        clearProcessBuff_0();
    }
}

//===========================================================================
// Function name: displayNetworkInfo
//
// Description: This function displays the network information
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: display_changed, SSID
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void displayNetworkInfo(void) {
    centerStringToDisplay(0, SSID);
    displayIP(1);
    display_changed = 1;
}

//===========================================================================
// Function name: displayIP
//
// Description: This function displays the IP address centered, starting at a
// desired line
//
// Passed : pos
// Locals: no variables declared
// Returned: no values returned
// Globals: display_line, IP, midIndex
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void displayIP(int pos) {
    strcpy(display_line[pos], BLANK_LINE);
    strcpy(display_line[pos + 1], BLANK_LINE);
    centerStringToDisplay(pos, IP);
    centerStringToDisplay(pos + 1, IP + midIndex + 1);
}


//===========================================================================
// Function name: IOTBufferCommands
//
// Description: This function reads the serial input from the IOT and 
// converts them into "commands" that the rest of the program can understand
// and then pushes it onto CommandBuffer, until there are no more recognzied
// command, it also checks whether the iot has disconnected and restarts the
// initialization process if so
//
// Passed : no variables passed
// Locals: pos,end_caret,end_null,end,time,c,comm
// Returned: no values returned
// Globals: USB0_Char_Rx_Process,iot_setup_state,pb0_buffered
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void IOTBufferCommands(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, DISCONNECTED_RESPONSE))
            iot_setup_state = CIPSERVER_Tx;

        char * pos = subStringPos((char*)USB0_Char_Rx_Process, CARET_SECURITY_CODE);

        while(pos) {
            pos += CARET_SECURITY_CODE_LEN; // now should be on where the command actually is
            char comm = *pos;
            pos++;
            char * end_caret = charInString(pos, CARET);
            char * end_null = charInString(pos, CR);
            char * end = end_caret ? end_caret : end_null;
            int time = stoi(pos, end - pos);
            command c = {
                .comm = comm,
                .duration = time
            };
            pushCB(c);
            pos = subStringPos(pos, CARET_SECURITY_CODE);
        }

        clearProcessBuff_0();
    }

}

//===========================================================================
// Function name: popCB
//
// Description: This function pops the oldest command for processing, then
// shifts the rest to be popped later
//
// Passed : no variables passed
// Locals: ret
// Returned: oldest command
// Globals: CommandBuffer,emptyCommand
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

command popCB(void) {
    command ret = CommandBuffer[0];

    for(int i = 0; i < COMMAND_BUFFER_LEN - 1; ++i) CommandBuffer[i] = CommandBuffer[i + 1];

    CommandBuffer[COMMAND_BUFFER_LEN - 1] = emptyCommand;
    return ret;
}

//===========================================================================
// Function name: pushCB
//
// Description: This function pushes a command onto the end of the
// CommandBuffer
//
// Passed : c
// Locals: no variables declared
// Returned: no values returned
// Globals: CommandBuffer
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void pushCB(command c) {
    int i;

    for(i = 0; i < COMMAND_BUFFER_LEN; ++i)
        if(CommandBuffer[i].comm == false && CommandBuffer[i].duration == false) break;

    if(i == COMMAND_BUFFER_LEN) {
        return;
    }

    CommandBuffer[i] = c;
}

//===========================================================================
// Function name: ProcessCommands
//
// Description: This function processes the oldest command and implements it
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: state,currCommand,stopwatch_milliseconds,stateCounter
// driveStateCounter,cycle_count,speedRight,speedLeft,driveTime,nextState,
// commandDisplayCounter
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void ProcessCommands(void) {
    //if(currCommand.comm == 0 && currCommand.duration == 0)return;
    //commandsReceieved = 1;
    if (CommandBuffer[BEGINNING].comm == STOP_COMMAND) {
        currCommand = popCB();
        state = START;
        stopwatch_milliseconds = BEGINNING;
        stateCounter = BEGINNING;
        driveStateCounter = BEGINNING;
        ShutoffMotors();
        return;
    }

    if (CommandBuffer[BEGINNING].comm == EXIT_COMMAND) {
        state = START;
        stopwatch_milliseconds = BEGINNING;
        stateCounter = BEGINNING;
        driveStateCounter = BEGINNING;
        ShutoffMotors();
    }

    if(state == START) {
        currCommand = popCB();

        if(currCommand.comm == false && currCommand.duration == false)return;

        commandsReceieved = true;
        stopwatch_seconds = BEGINNING;
        cycle_count = BEGINNING;

        //driveTime = (int)(currCommand.duration * (currCommand.comm == RIGHT_COMMAND || currCommand.comm == LEFT_COMMAND ? TURN_CONSTANT : 1));

        switch(currCommand.comm) {
            case (FORWARD_COMMAND):
                speedRight = STRAIGHT_RIGHT;
                speedLeft = STRAIGHT_LEFT;
                state = DRIVE;
                driveTime = currCommand.duration;
                break;

            case (REVERSE_COMMAND):
                speedRight = -STRAIGHT_RIGHT;
                speedLeft = -STRAIGHT_LEFT;
                state = DRIVE;
                driveTime = currCommand.duration;
                break;

            case (RIGHT_COMMAND):
                speedRight = RIGHT_MID;
                speedLeft = -(LEFT_MID);
                state = DRIVE;
                driveTime = currCommand.duration << TURN_CONSTANT;
                break;

            case (LEFT_COMMAND):
                speedRight = -(RIGHT_MID);
                speedLeft = LEFT_MID;
                state = DRIVE;
                driveTime = currCommand.duration << TURN_CONSTANT;
                break;

            case (LINEFOLLOW_COMMAND):
                state = STRAIGHT;
                speedRight = currCommand.duration;
                break;

            case (DISPLAY_NUMBER_COMMAND):
                commandDisplayCounter = DISPLAY_ARRIVAL_STATE;
                currentStation = currCommand.duration;
                break;

            case (EXIT_COMMAND):
                state = WAIT;
                nextState = EXIT;
                speedRight = currCommand.duration;
                break;
        }
    }

}