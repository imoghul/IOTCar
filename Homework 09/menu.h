#define MENU_LEN                (10)
#define MAIN                    ('M')
#define RESISTOR                ('R')
#define SHAPE                   ('S')
#define START_MENU              ('T')
#define SONG                    ('s')
//#define SONG_SCROLL_THRESH      (2)
typedef struct menuStruct{
  char length;
  char current;
  char name;
  char * headers[MENU_LEN];
  char * values[MENU_LEN];
  struct menuStruct * transitions[MENU_LEN];
} menu;

void displayMenu(menu m,unsigned int headerLine,unsigned int valLine);
void MenuProcess(void);
void updateMenuPos(menu* m);
void trainsitionMenu(void);
void resetMenu(void);
void displayResistorsMenu();
void displayShapesMenu();
void displayStartMenu();
void displaySongMenu();