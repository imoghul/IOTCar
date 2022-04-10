#include "menu.h"
#include "msp430.h"
#include "adc.h"
#include "functions.h"
#include <string.h>
extern volatile unsigned int ADC_Thumb;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char menuState = START_MENU;
unsigned int lastThumb;


menu start  = {
    .length = 1,
    .current = 0,
    .name = START_MENU,
    .headers = {""},
    .values = {""},
    .transitions = {&start}
};


menu* currMenu = &start;



void displayStartMenu() {
    /*lcd_BIG_mid();
    strcpy(display_line[0], "  Ibrahim ");
    strcpy(display_line[1], "Homework 9");
    strcpy(display_line[2], "  Moghul  ");
    display_changed = 1;*/
}


void updateMenuPos(menu* m) {
    unsigned int val = (ADC_Thumb * m->length) >> THUMB_RES;
    m->current  = val < m->length ? val : m->length - 1;
}

void trainsitionMenu(void) {
    currMenu = currMenu->transitions[currMenu->current];
    menuState = currMenu->name;

    strcpy(display_line[0], "          ");
    strcpy(display_line[1], "          ");
    strcpy(display_line[2], "          ");
    strcpy(display_line[3], "          ");
    display_changed = 1;
}

void MenuProcess(void) {
    switch(menuState) {
        case START_MENU:
            updateMenuPos(&start);
            displayStartMenu();
            break;
        default:
            break;
    }
}


