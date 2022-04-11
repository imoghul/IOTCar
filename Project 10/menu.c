#include "menu.h"
#include "msp430.h"
#include "adc.h"
#include "functions.h"
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

menu calib,start,mainMenu;

menu* currMenu = &start;



/*void displayStartMenu() {}

void displayMainMenu() {
    strcpy(display_line[0], mainMenu.headers[mainMenu.current]);
}

void displayCalibMenu() {
    display_changed = 1;
}*/


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
      case START_MENU:
          //updateMenuPos(&start);
          //displayStartMenu();
          break;
      case MAIN_MENU:
          strcpy(display_line[0], mainMenu.headers[mainMenu.current]);//displayMainMenu();
          break;
      case CALIB_MENU:
          display_changed = 1;//displayCalibMenu();
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
      .length = 1,
      .current = 0,
      .name = MAIN_MENU,
      .headers = {"CALIBRATE"},
      .values = {""},
      .transitions = {&calib}
  };
  
  start  = (menu){
      .length = 1,
      .current = 0,
      .name = START_MENU,
      .headers = {""},
      .values = {""},
      .transitions = {&mainMenu}
  };
}

