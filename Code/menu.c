#include "menu.h"
#include "msp430.h"
#include "adc.h"
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

menu calib = {
    .length = 1,
    .current = 0,
    .name = CALIB_MENU,
    .headers = {""},
    .values = {""},
    .transitions = {&mainMenu}
};
menu start = {
    .length = 1,
    .current = 0,
    .name = START_MENU,
    .headers = {""},
    .values = {""},
    .transitions = {&mainMenu}
};
menu commandsOutput = {
    .length = 1,
    .current = 0,
    .name = COMMANDS_MENU,
    .headers = {""},
    .values = {""},
    .transitions = {&mainMenu}
};

menu* currMenu = &start;



/*void displayStartMenu() {}

void displayMainMenu() {
    strcpy(display_line[0], mainMenu.headers[mainMenu.current]);
}

void displayCalibMenu() {
    display_changed = 1;
}*/

void displayCommand(){
  if(currCommand.comm != LINEFOLLOW_COMMAND && currCommand.comm != EXIT_COMMAND){
    display_line[3][0] = currCommand.comm;
    HEXtoBCD(currCommand.duration, 3, 1);
  }
}

void displayStatus(){
  if(currCommand.comm == LINEFOLLOW_COMMAND) {
        strcpy(display_line[3],"Auto.     ");
    } else if(state == DONE) {
        strcpy(display_line[3],"Time:     ");
        strcpy(display_line[1], " That was ");
        strcpy(display_line[2], "easy!! ;-)");

    } else if(commandsReceieved && currCommand.comm == 0 && currCommand.duration == 0) strcpy(display_line[3],"          ");//display_line[3][0] = display_line[3][1] = display_line[3][2] = display_line[3][3] = display_line[3][4] = ' ';
     if(commandsReceieved){//(stopwatchUpdated) {
        //stopwatchUpdated = 0;
        HEXtoBCD(timeElapsedSeconds, 3, 5);
        display_line[3][5] = ' ';
        display_line[3][9] = 's';
        //display_line[3][9] = timeElapsedMilliseconds + '0';
    }
}

void displayArrival(){
  
  //if(currCommand.comm == DISPLAY_NUMBER_COMMAND) {
        strcpy(display_line[0], "ARRIVED 0 ");
        display_line[0][9] = currentStation + '0';
        commandDisplayCounter = 0;
  //}
  
}

void displayIp(){
  
  if(commandsReceieved) {
        if(state != DONE)displayIP(1);
    } else {
        strcpy(display_line[0], " WAITING  ");
        strcpy(display_line[1], " FOR INPUT");

        if(state != DONE)displayIP(2);
    }
  
}

void displayStopwatch(){
    
}

void displayCommandsMenu() {
  switch(commandDisplayCounter++){
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
      commandDisplayCounter = 0;
      break;
    case DISPLAY_ARRIVAL_STATE:
      displayArrival();
    default: break;
  }
    display_changed = 1;
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
        calibrationMode = 0;
        LBDetect = RBDetect = LWDetect = RWDetect = 0;
        state = CALIBRATE;
    }

    strcpy(display_line[0], "          ");
    strcpy(display_line[1], "          ");
    strcpy(display_line[2], "          ");
    strcpy(display_line[3], "          ");
    display_changed = 1;
}

void MenuProcess(void) {
    if(transMenu) {
        transMenu = 0;
        transitionMenu(currMenu);
    }

    if(interractMenu) {
        interractMenu = 0;
        interractWithMenu();
    }

    switch(menuState) {
        //case START_MENU:
        //    //updateMenuPos(&start);
        //    //displayStartMenu();
        //    break;
        case MAIN_MENU:
            updateMenuPos(&mainMenu);
            strcpy(display_line[0], mainMenu.headers[mainMenu.current]);
            //displayMainMenu();
            display_changed = 1;
            break;

        case COMMANDS_MENU:
            displayCommandsMenu();
            break;

        /*case NETWORK_MENU:
            displayNetworkInfo();
            break;*/

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

