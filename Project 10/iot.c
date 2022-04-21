#include "iot.h"
#include "msp430.h"
#include "utils.h"
#include "menu.h"
#include <string.h>
#include "wheels.h"
#include "utils.h"
#include "serial.h"
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


void waitForReady(void) {
    if(pb0_buffered) {
        if(strcmp((char*)USB0_Char_Rx_Process, BOOT_RESPONSE)==0) iot_setup_state = CIPMUX_Tx;

        clearProcessBuff_0();
    }
}

void SendIOTCommand(char* command, char nextState) {
    strcpy((char*)USB0_Char_Tx, command);
    USCI_A0_transmit();
    iot_setup_state = nextState;
}

void getSSID(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, SSID_RESPONSE)) {
            int i;

            for(i = 0; i <= SSID_LEN && USB0_Char_Rx_Process[i + SSID_RESPONSE_LEN + 1] != '\"'; ++i) SSID[i] = USB0_Char_Rx_Process[i + SSID_RESPONSE_LEN + 1];

            SSID[i + SSID_RESPONSE_LEN + 2] = 0; // set the end of the SSID to null
            SSID[SSID_LEN] = 0; //  set end of the array to null

            iot_setup_state = GET_IP_Tx;
        } else iot_setup_state = GET_SSID_Tx;

        clearProcessBuff_0();
    }
}

void getIP(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, IP_RESPONSE)) {
            int i;

            for(i = 0; i <= IP_LEN && USB0_Char_Rx_Process[i + IP_RESPONSE_LEN + 1] != '"'; ++i) {
                IP[i] = USB0_Char_Rx_Process[i + IP_RESPONSE_LEN + 1];

                if(IP[i] == '.') {
                    if(++dotFound == 2) midIndex = i;
                }
            }

            IP[i + IP_RESPONSE_LEN + 2] = 0;
            IP[IP_LEN] = 0;
            IP[midIndex] = 0;

            iot_setup_state = IOT_SETUP_FINISHED;
        } else iot_setup_state = GET_IP_Tx;

        clearProcessBuff_0();
    }
}

void displayNetworkInfo(void) {
    centerStringToDisplay(0, SSID);
    displayIP(1);
    display_changed = 1;
}

void displayIP(int pos) {
    strcpy(display_line[pos],"          ");
    strcpy(display_line[pos+1],"          ");
    centerStringToDisplay(pos, IP);
    centerStringToDisplay(pos + 1, IP + midIndex + 1);
}


void IOTBufferCommands(void) {
    if(pb0_buffered) {
        if(subStringPos((char*)USB0_Char_Rx_Process, DISCONNECTED_RESPONSE))
            iot_setup_state = CIPSERVER_Tx;

        char * pos = subStringPos((char*)USB0_Char_Rx_Process, CARET_SECURITY_CODE);

        while(pos) {
            pos += CARET_SECURITY_CODE_LEN; // now should be on where the command actually is
            char comm = *pos;
            pos++;
            char * end_caret = charInString(pos, '^');
            char * end_null = charInString(pos, '\r');
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

command popCB(void) {
    command ret = CommandBuffer[0];

    for(int i = 0; i < COMMAND_BUFFER_LEN - 1; ++i) CommandBuffer[i] = CommandBuffer[i + 1];

    CommandBuffer[COMMAND_BUFFER_LEN - 1] = emptyCommand;
    return ret;
}
void pushCB(command c) {
    int i;

    for(i = 0; i < COMMAND_BUFFER_LEN; ++i)
        if(CommandBuffer[i].comm == 0 && CommandBuffer[i].duration == 0) break;

    if(i == COMMAND_BUFFER_LEN) {
        return;
    }

    CommandBuffer[i] = c;
}

void ProcessCommands(void) {
    //if(currCommand.comm == 0 && currCommand.duration == 0)return;
    //commandsReceieved = 1;
    if (CommandBuffer[0].comm == STOP_COMMAND) {
        currCommand = popCB();
        state = START;
        stopwatch_milliseconds = 0;
        stateCounter = 0 ;
        driveStateCounter = 0;
        ShutoffMotors();
        return;
    }

    if (CommandBuffer[0].comm == EXIT_COMMAND) {
        state = START;
        stopwatch_milliseconds = 0;
        stateCounter = 0 ;
        driveStateCounter = 0;
        ShutoffMotors();
    }

    if(state == START) {
        currCommand = popCB();

        if(currCommand.comm == 0 && currCommand.duration == 0)return;

        commandsReceieved = 1;
        stopwatch_seconds = 0;
        cycle_count = 0;

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
                speedRight = STRAIGHT_RIGHT>>1;
                speedLeft = -(STRAIGHT_LEFT>>1);
                state = DRIVE;
                driveTime = currCommand.duration << TURN_CONSTANT;
                break;

            case (LEFT_COMMAND):
                speedRight = -(STRAIGHT_RIGHT>>1);
                speedLeft = STRAIGHT_LEFT>>1;
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