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
extern volatile char transMenu,interractMenu;
unsigned int lastThumb;
extern command currCommand;
extern char commandsReceieved;
extern volatile unsigned int stopwatchUpdated;
extern volatile int timeElapsedSeconds,timeElapsedMilliseconds;

menu calib,start,mainMenu,commandsOutput;//,networkInfo;

menu* currMenu = &start;



/*void displayStartMenu() {}

void displayMainMenu() {
    strcpy(display_line[0], mainMenu.headers[mainMenu.current]);
}

void displayCalibMenu() {
    display_changed = 1;
}*/

void displayCommandsMenu() {
  //strcpy(display_line[3],"          ");
  display_line[3][0] = currCommand.comm;
  //HEXtoBCD(currCommand.duration, 1, 4);
  if(currCommand.comm == DISPLAY_NUMBER_COMMAND){
    strcpy(display_line[0],"ARRIVED 0 ");
    display_line[0][9] = currCommand.duration+'0';
  }
  displayIP();
  if(currCommand.comm == 0 && currCommand.duration == 0) display_line[3][0] = display_line[3][1] = display_line[3][2] = display_line[3][3] =' ';
  if(!commandsReceieved) {
    strcpy(display_line[3],"WAITING...");
  }
  else display_line[3][1] = display_line[3][2] = display_line[3][3] =' ';
  if(stopwatchUpdated){
    stopwatchUpdated = 0;
    HEXtoBCD(timeElapsedSeconds,3,4);
    display_line[3][4] = ' ';
    display_line[3][8] = '.';
    display_line[3][9] = timeElapsedMilliseconds + '0';
  }
  display_changed = 1;
}


void updateMenuPos(menu* m) {
    unsigned int val = (ADC_Thumb * m->length) >> THUMB_RES;
    m->current  = val < m->length ? val : m->length - 1;
}

void interractWithMenu(void){
  switch(menuState){
    case CALIB_MENU:
      calibrationMode++;
      break;
  }
}

void transitionMenu(menu* m) {
    // transitioning out code
    if(menuState == CALIB_MENU){
      state = START;
    }
  
    currMenu = m->transitions[m->current];
    menuState = currMenu->name;
    
    // transitioning in code
    if(menuState == CALIB_MENU){
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
  if(transMenu){
    transMenu = 0;
    transitionMenu(currMenu);
  }
  if(interractMenu) {
    interractMenu = 0;
    interractWithMenu();
  }
  switch(menuState) {
      /*case START_MENU:
          //updateMenuPos(&start);
          //displayStartMenu();
          break;*/
      case MAIN_MENU:
          updateMenuPos(&mainMenu);
          strcpy(display_line[0], mainMenu.headers[mainMenu.current]);//displayMainMenu();
          display_changed = 1;
          break;
      case CALIB_MENU:
          display_changed = 1;//displayCalibMenu();
          break;
      case COMMANDS_MENU:
          displayCommandsMenu();
          break;
      /*case NETWORK_MENU:
          displayNetworkInfo();*/
          break;
      default:
          break;
  }
}

void Init_Menu(void){
  calib = (menu){
      .length = 1,
      .current = 0,
      .name = CALIB_MENU,
      .headers = {""},
      .values = {""},
      .transitions = {&mainMenu}
  };
  
  mainMenu  = (menu){
      .length = 3,
      .current = 0,
      .name = MAIN_MENU,
      .headers = {"CALIBRATE "," COMMANDS "},//," NETWORK  "},
      .values = {""},
      .transitions = {&calib,&commandsOutput}//,&networkInfo}
  };
  
  start  = (menu){
      .length = 1,
      .current = 0,
      .name = START_MENU,
      .headers = {""},
      .values = {""},
      .transitions = {&mainMenu}
  };
  commandsOutput = (menu){
      .length = 1,
      .current = 0,
      .name = COMMANDS_MENU,
      .headers = {""},
      .values = {""},
      .transitions = {&mainMenu}
  };
  /*networkInfo = (menu){
      .length = 1,
      .current = 0,
      .name = NETWORK_MENU,
      .headers = {""},
      .values = {""},
      .transitions = {&mainMenu}
  };*/
}

