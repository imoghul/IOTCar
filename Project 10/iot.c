#include "iot.h"
#include <string.h>
#include "msp430.h"
#include "utils.h"
#include <string.h>
#include "utils.h"
#include "serial.h"
#include "ports.h"
#include "sm.h"
#include <stdlib.h>

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

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;

extern volatile char state;
extern volatile int stateCounter;
extern volatile char nextState;

extern int polarityRight, polarityLeft;
extern unsigned int driveTime;

command emptyCommand = {0, 0};


int Init_IOT(void) {
    //if(!receievedFromPC) return;

    switch(iot_setup_state) {
        case BOOT_UP:
            waitForReady();
            break;

        case CIPMUX_Tx:
            SendIOTCommand(ALLOW_MULTIPLE_CONNECTIONS, CIPMUX_Rx);
            break;

        case CIPMUX_Rx:
            if(UCA0IE & UCTXIE) break; // wait for the Tx to completely transmit

            if(pb0_buffered) { // wait for pb to finish buffering
                iot_setup_state = CIPSERVER_Tx;
                clearProcessBuff_0();
            }

            break;

        case CIPSERVER_Tx:
            SendIOTCommand(START_SERVER, CIPSERVER_Rx);
            break;

        case CIPSERVER_Rx:
            if(UCA0IE & UCTXIE) break;

            if(pb0_buffered) {
                iot_setup_state = GET_SSID_Tx;
                clearProcessBuff_0();
            }

            break;

        case GET_SSID_Tx:
            SendIOTCommand(SSID_COMMAND, GET_SSID_Rx);
            break;

        case GET_SSID_Rx:
            if(UCA0IE & UCTXIE) break;

            getSSID();

            break;

        case GET_IP_Tx:
            SendIOTCommand(IP_COMMAND, GET_IP_Rx);
            break;

        case GET_IP_Rx:
            if(UCA0IE & UCTXIE) break;

            getIP();

            break;

        default:
            return 1;
            break;
    }

    return 0;
}


void waitForRead(void) {
    if(pb0_buffered) {
        if(strcmp((char*)USB0_Char_Rx_Process, BOOT_RESPONSE) == 0) iot_setup_state = CIPMUX_Tx;

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

            SSID[i + SSID_RESPONSE_LEN + 2] = 0;
            SSID[SSID_LEN] = 0;
            centerStringToDisplay(0, SSID);
            display_changed = 1;
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

                if(USB0_Char_Rx_Process[i + IP_RESPONSE_LEN + 1] == '.') {
                    if(dotFound++ == 1) midIndex = i;
                }
            }

            IP[i + IP_RESPONSE_LEN + 2] = 0;
            IP[IP_LEN] = 0;
            IP[midIndex] = 0;
            strcpy(display_line[1], "IP ADDRESS");
            centerStringToDisplay(2, IP);
            centerStringToDisplay(3, IP + midIndex + 1);
            display_changed = 1;
            iot_setup_state = IOT_SETUP_FINISHED;
        } else iot_setup_state = GET_IP_Tx;

        clearProcessBuff_0();
    }
}


void IOTBufferCommands(void) {
    if(pb0_buffered) {
        char * pos = USB0_Char_Rx_Process + subStringPos((char*)USB0_Char_Rx_Process, CARET_SECURITY_CODE);

        while(pos) {
            pos += CARET_SECURITY_CODE_LEN; // now should be on where the command actually is
            char comm = *pos;
            pos++;
            char * end_caret = charInString(pos, '^');
            char * end_null = charInString(pos, 0);
            char * end = end_caret ? end_caret : end_null;
            int time = stoi(pos);
            command c = {
                .comm = comm,
                .duration = time
            };
            pushCB(c);
            pos += subStringPos(pos, CARET_SECURITY_CODE);
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
    if(state == START && (CommandBuffer[0].comm != 0 && CommandBuffer[0].duration != 0)) {
        command c = popCB();
        stopwatch_seconds = 0;
        cycle_count = 0;
        state = DRIVE;
        display_line[1][2] = c.comm;
        HEXtoBCD(c.duration, 1, 4);
        driveTime = (int)(c.duration * (c.comm == RIGHT_COMMAND || c.comm == LEFT_COMMAND ? TURN_CONSTANT : 1));

        switch(c.comm) {
            case (FORWARD_COMMAND):
                polarityRight = 1;
                polarityLeft = 1;
                break;

            case (REVERSE_COMMAND):
                polarityRight = -1;
                polarityLeft = -1;
                break;

            case (RIGHT_COMMAND):
                polarityRight = 1;
                polarityLeft = -1;
                break;

            case (LEFT_COMMAND):
                polarityRight = -1;
                polarityLeft = 1;
                break;

            case (LINEFOLLOW_COMMAND):
                state = STRAIGHT;
                break;

            default:
                state = START;
                break;
        }
    }
}