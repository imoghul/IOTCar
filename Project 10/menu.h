#define MENU_LEN                (10)
#define START_MENU              ('T')
#define CALIB_MENU              ('C')
#define MAIN_MENU               ('M')
#define COMMANDS_MENU           ('c')
#define NETWORK_MENU            ('N')
#define DISPLAY_ARRIVAL_STATE   (500)
// an attempt to organize all the data that a menu necessitates
typedef struct menuStruct {
    char length;
    char current;
    char name;
    char * headers[MENU_LEN];
    char * values[MENU_LEN];
    struct menuStruct * transitions[MENU_LEN];
} menu;


//===========================================================================
// Function name: MenuProcess
//
// Description: Runs the state machine for the menu system
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: menuState
//
// Author: Ibrahim Moghul
// Date: Mar 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void MenuProcess(void);
//===========================================================================
// Function name: updateMenuPos
//
// Description: updates the menu's index based on ADC value
//
// Passed : pointer to menu struct
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Mar 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void updateMenuPos(menu* m);
//===========================================================================
// Function name: trainsitionMenu
//
// Description: used to transition to next menu. This is triggered by button
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: currMenu, menuState, display_line, display_changed, lastThumb
//
// Author: Ibrahim Moghul
// Date: Mar 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void transitionMenu(menu * m);
//===========================================================================
// Function name: displayMainMenu
//
// Description: used to reset to display main menu
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Mar 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void displayMainMenu();

void Init_Menu(void);

void interractWithMenu(void);