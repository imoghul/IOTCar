#include "menu.h"
#include "msp430.h"
#include "adc.h"
#include "macros.h"
#include "functions.h"
#include "iot.h"
#include "sm.h"
#include <string.h>
#include <stdlib.h>
extern volatile unsigned int ADC_Thumb;
extern volatile unsigned char display_changed;
extern volatile unsigned int calibrationMode;
extern char display_line[4][11];
extern volatile char state;
char menuState = START_MENU;
extern unsigned int LBDetect, LWDetect, RBDetect, RWDetect;
extern volatile char transMenu, interractMenu;
unsigned int lastThumb;
extern command currCommand;
extern char commandsReceieved;
extern volatile unsigned int stopwatchUpdated;
int commandDisplayCounter;
extern char currentStation;
extern volatile int timeElapsedSeconds, timeElapsedMilliseconds;

menu mainMenu;

// menu calib = {
//     .length = 1,
//     .current = 0,
//     .name = CALIB_MENU,
//     .headers = {""},
//     .values = {""},
//     .transitions = {&mainMenu}
// };
menu start = {
    .length = 1,
    .current = 0,
    .name = START_MENU,
    .headers = {""},
    .values = {""},
    .transitions = {&mainMenu}
};
// menu commandsOutput = {
//     .length = 1,
//     .current = 0,
//     .name = COMMANDS_MENU,
//     .headers = {""},
//     .values = {""},
//     .transitions = {&mainMenu}
// };

menu* currMenu = &start;



/*void displayStartMenu() {}

void displayMainMenu() {
    strcpy(display_line[0], mainMenu.headers[mainMenu.current]);
}

void displayCalibMenu() {
    display_changed = 1;
}*/

void displayCommand() {
    if(currCommand.comm != LINEFOLLOW_COMMAND && currCommand.comm != EXIT_COMMAND) {
        LINE4[0] = currCommand.comm;
        HEXtoBCD(currCommand.duration, COMMAND_LINE, COMMAND_DURATION_BEGIN);
    }
}

void displayStatus() {
    if(currCommand.comm == LINEFOLLOW_COMMAND) {
        strcpy(LINE4, "Auto.     ");
    } else if(state == DONE) {
        strcpy(LINE4, "Time:     ");
        strcpy(LINE2, " That was ");
        strcpy(LINE3, "easy!! ;-)");

    } else if(commandsReceieved && currCommand.comm == false && currCommand.duration == false) strcpy(LINE4, BLANK_LINE);

    if(commandsReceieved) { //(stopwatchUpdated) {
        //stopwatchUpdated = 0;
        HEXtoBCD(timeElapsedSeconds, COMMAND_LINE, STOPWATCH_BEGIN5);
        LINE4[STOPWATCH_BEGIN] = ' ';
        LINE4[LINE_LEN - 1] = 's';
        //display_line[3][9] = timeElapsedMilliseconds + '0';
    }
}

void displayArrival() {

    //if(currCommand.comm == DISPLAY_NUMBER_COMMAND) {
    strcpy(LINE1, "ARRIVED 0 ");
    LINE1[LINE_LEN - 1] = currentStation + '0';
    commandDisplayCounter = BEGINNING;
    //}

}

void displayIp() {

    if(commandsReceieved) {
        if(state != DONE)displayIP(1);
    } else {
        strcpy(LINE1, " WAITING  ");
        strcpy(LINE2, " FOR INPUT");

        if(state != DONE)displayIP(DIPLAY_IP_LINE);
    }

}

void displayStopwatch() {

}

void displayCommandsMenu() {
    switch(commandDisplayCounter++) {
        case 0:
            displayCommand();
            break;

        case 100:
            displayStatus();
            break;

        case 200:
            displayIp();
            break;

        case 300:
            displayStopwatch();
            break;

        case 400:
            commandDisplayCounter = BEGINNING;
            break;

        case DISPLAY_ARRIVAL_STATE:
            displayArrival();

        default:
            break;
    }

    display_changed = true;
}


void updateMenuPos(menu* m) {
    unsigned int val = (ADC_Thumb * m->length) >> THUMB_RES;
    m->current  = val < m->length ? val : m->length - 1;
}

void interractWithMenu(void) {
    /*switch(menuState) {
        case CALIB_MENU:
            calibrationMode++;
            break;
    }*/
    if(menuState == CALIB_MENU) calibrationMode++;
}

void transitionMenu(menu* m) {
    // transitioning out code
    if(menuState == CALIB_MENU) {
        state = START;
    }

    currMenu = m->transitions[m->current];
    menuState = currMenu->name;

    // transitioning in code
    if(menuState == CALIB_MENU) {
        calibrationMode = BEGINNING;
        LBDetect = RBDetect = LWDetect = RWDetect = OFF;
        state = CALIBRATE;
    }

    strcpy(LINE1, BLANK_LINE);
    strcpy(LINE2, BLANK_LINE);
    strcpy(LINE3, BLANK_LINE);
    strcpy(LINE4, BLANK_LINE);
    display_changed = true;
}

void MenuProcess(void) {
    if(transMenu) {
        transMenu = false;
        currMenu = &mainMenu;
        menuState = MAIN_MENU;
        //transitionMenu(currMenu);
    }

    /*if(interractMenu) {
        interractMenu = 0;
        interractWithMenu();
    }*/

    switch(menuState) {
        //case START_MENU:
        //    //updateMenuPos(&start);
        //    //displayStartMenu();
        //    break;
        case MAIN_MENU:
            updateMenuPos(&mainMenu);
            strcpy(LINE1, mainMenu.headers[mainMenu.current]);
            //displayMainMenu();
            display_changed = TRUE;
            break;

            // case COMMANDS_MENU:
            //     displayCommandsMenu();
            //     break;

            // case NETWORK_MENU:
            //     displayNetworkInfo();
            //     break;

            //default:
            //    break;
    }
}

void Init_Menu(void) {
    // calib = (menu) {
    //     .length = 1,
    //     .current = 0,
    //     .name = CALIB_MENU,
    //     .headers = {""},
    //     .values = {""},
    //     .transitions = {&mainMenu}
    // };

    mainMenu  = (menu) {
        .length = 2,
        .current = 0,
        .name = MAIN_MENU,
        .headers = {"CALIBRATE ", " COMMANDS "}, //," NETWORK  "},
        .values = {""},
        .transitions = {&calib, &commandsOutput} //,&networkInfo}
    };

    // start  = (menu) {
    //     .length = 1,
    //     .current = 0,
    //     .name = START_MENU,
    //     .headers = {""},
    //     .values = {""},
    //     .transitions = {&mainMenu}
    // };
    // commandsOutput = (menu) {
    //     .length = 1,
    //     .current = 0,
    //     .name = COMMANDS_MENU,
    //     .headers = {""},
    //     .values = {""},
    //     .transitions = {&mainMenu}
    // };
    /*networkInfo = (menu){
        .length = 1,
        .current = 0,
        .name = NETWORK_MENU,
        .headers = {""},
        .values = {""},
        .transitions = {&mainMenu}
    };*/
}

