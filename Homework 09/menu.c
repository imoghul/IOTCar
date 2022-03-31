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

menu resistors= {
  .length = 10,
  .current = 0,
  .name = RESISTOR,
  .headers = {"   Black  ","   Brown  ","   Red    ","  Orange  ","  Yellow  ","   Green  ","   Blue   ","  Violet  ","   Gray   ","   White  "},
  .values = {"    1     ","    2     ","    3     ","    4     ","    5     ","    6     ","    7     ","    8     ","    9     ","    10    "},
  .transitions = {&resistors,&resistors,&resistors,&resistors,&resistors,&resistors,&resistors,&resistors,&resistors,&resistors}
};

menu shape= {
  .length = 10,
  .current = 0,
  .name = SHAPE,
  .headers = {"  Circle  ","  Square  "," Triangle ","  Octagon "," Pentagon ","  Hexagon ","   Cube   ","   Oval   ","  Sphere  "," Cylinder "},
  .values = {""},
  .transitions = {&shape,&shape,&shape,&shape,&shape,&shape,&shape,&shape,&shape,&shape}
};

menu song= {
  .length = 1,
  .current = 0,
  .name = SONG,
  .headers = {""},
  .values = {"We're the Red and Whie from State And we know we are the best. A hand behind our back, we can take on all the rest. Come over the hill, Carolina. Devils and Deacs stand in line. The Red and White from N.C. State. Go State!"},
  .transitions = {&song}
};

menu mainMenu  = {
  .length = 3,
  .current = 0,
  .name = MAIN,
  .headers = {"Resistors ","  Shapes  ","   Song   "},
  .values = {""},
  .transitions = {&resistors,&shape,&song}
};
menu start  = {
  .length = 1,
  .current = 0,
  .name = START_MENU,
  .headers = {""},
  .values = {""},
  .transitions = {&mainMenu}
};

menu* currMenu = &start;

void displayStartMenu(){
  lcd_BIG_mid();
  strcpy(display_line[0],"  Ibrahim ");
  strcpy(display_line[1],"Homework 9");
  strcpy(display_line[2],"  Moghul  ");
  display_changed = 1;
}  

void displayMainMenu(){
  lcd_4line();
  strcpy(display_line[0],mainMenu.headers[mainMenu.current]);
  strcpy(display_line[1], "          ");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = 1;
  //HEXtoBCD(mainMenu.current,3,0);
}
void displayResistorsMenu(){
  lcd_4line();
  strcpy(display_line[0],resistors.headers[resistors.current]);
  strcpy(display_line[1],resistors.values[resistors.current]);
  display_changed = 1;
  //HEXtoBCD(resistors.current,3,0);
}

void displayShapesMenu(){
  lcd_BIG_mid();
  strcpy(display_line[0],shape.current>=1?shape.headers[shape.current-1]:"          ");
  strcpy(display_line[1],shape.headers[shape.current]);
  strcpy(display_line[2],shape.current<shape.length-1?shape.headers[shape.current+1]:"          ");
  display_changed = 1;
  //HEXtoBCD(shape.current,3,0);
}

void displaySongMenu(){
  if(ADC_Thumb-lastThumb<SONG_SCROLL_THRESH){
    lcd_BIG_mid();
    song.current++;
    if(song.current<strlen(song.values[0])-10){
      strncpy(display_line[1],song.values[song.current],10);
    }
    display_changed = 1;
    lastThumb = ADC_Thumb;
  }
  //HEXtoBCD(shape.current,3,0);
}

void updateMenuPos(menu* m){
  unsigned int val = (ADC_Thumb*m->length)>>THUMB_RES;
  m->current  = val<m->length?val:m->length-1;
}

void trainsitionMenu(void){
  currMenu = currMenu->transitions[currMenu->current];
  menuState = currMenu->name;
  if(menuState == SONG) song.current = 0;
  strcpy(display_line[0], "          ");
  strcpy(display_line[1], "          ");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = 1;
}

void resetMenu(void){
  currMenu = &mainMenu;
  menuState = MAIN;
  strcpy(display_line[0], "          ");
  strcpy(display_line[1], "          ");
  strcpy(display_line[2], "          ");
  strcpy(display_line[3], "          ");
  display_changed = 1;
}

void MenuProcess(void){
  //updateMenuPos(&song);
  switch(menuState){
    case START_MENU:
      updateMenuPos(&start);
      displayStartMenu();
      break;
    case MAIN:
      updateMenuPos(&mainMenu);
      displayMainMenu();
      break;
    case RESISTOR:
      updateMenuPos(&resistors);
      displayResistorsMenu();
      break;
    case SHAPE:
      updateMenuPos(&shape);
      displayShapesMenu();
      break;
    case SONG:
      displaySongMenu();
      break;
    default: break;
  }
}


