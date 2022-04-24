#include <string.h>
#include "utils.h"
#include "macros.h"
#include "adc.h"
extern volatile unsigned char display_changed;
extern char display_line[4][11];

//===========================================================================
// Function name: centerStringToDisplay
//
// Description: This function centers a string to a desired line on the 
// display
//
// Passed : line, s
// Locals: len, pos
// Returned: no values returned
// Globals: display_line
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void centerStringToDisplay(unsigned int line, char * s) {
    int len = strlen(s);
    int pos = ((LINE_LEN - len) >> 1);
    strcpy(display_line[line] + pos, s);
    display_line[line][pos + len] = ' ';
    display_line[line][LINE_LEN] = '\0';
}

//===========================================================================
// Function name: subStringPos
//
// Description: This function a sub string in a string and returns a pointer
// to the beginning of it
//
// Passed : str, subString
// Locals: lenSub, len
// Returned: pointer to beginning of sub string
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

char* subStringPos(const char* str, char * subString) {

    int i, d;

    int lenSub = strlen(subString);
    int len = strlen(str) - lenSub;

    for (i = 0; i < len; i++) {
        int exists = true;

        for (d = 0; d < lenSub; d++) {
            if (str[i + d] != subString[d]) {
                exists = false;
                break;
            }
        }

        if (exists) {
            return (char*)(str + i);
        }
    }

    return false;
}

//===========================================================================
// Function name: stoi
//
// Description: This function converts a string to an integer
//
// Passed : str, len
// Locals: num
// Returned: the integer representation of the string
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int stoi(char* str, int len) {
    int num = 0;

    for(int i = 0; i < len/* && str[i] >= '0' && str[i] <= '9'*/; ++i)
        num = num * BASE_10 + (int)(str[i] - '0');

    return num;
}

//===========================================================================
// Function name: charInString
//
// Description: This function returns the first instance of a character in
// a string
//
// Passed : str, c
// Locals: no variables declared
// Returned: pointer to first instance of character in string
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

char* charInString(const char* str, char c) {
    for(int i = 0; i < strlen(str) + 1; i++)
        if(str[i] == c) return (char*)(str + i);

    return false;
}

//===========================================================================
// Function name: absVal
//
// Description: This function returns the absolute value of an integer
//
// Passed : n
// Locals: no variables declared
// Returned: |n|
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

unsigned int absVal(int n) {
    const int ret[2] = {n, -n};
    return (unsigned int)(ret [n < 0]);
}

//===========================================================================
// Function name: HEXtoBCD
//
// Description: This function puts an int to a location on the LCD in base 10
//
// Passed : hex_value, line, start
// Locals: value, bases
// Returned: no values returned
// Globals: display_line
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void HEXtoBCD(int hex_value, int line, int start) {

    int value = BEGINNING;

    int i;
    int bases[] = HEX_BCD_BASES;

    for(i = 0; i < HEX_BCD_SIZE; i++) {
        int base = bases[i];

        while(hex_value > (base - 1)) {
            hex_value -= base;
            value += 1;
        }

        display_line[line][start + i] = value + '0';

    }

    display_line[line][start + HEX_BCD_SIZE] = hex_value + '0'; // set the last character to the ones place
}

//===========================================================================
// Function name: additionSafe
//
// Description: This function adds 2 numbers, handles overflow, and
// constrains the ouptut to a desired range
//
// Passed : val, max, min, increment
// Locals: res
// Returned: the safe addition of the two integers
// Globals: no global values
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

int additionSafe(int val, int max, int min, int increment) {
    long res = val + increment;

    if(res > (long)max) res = (long)max;

    if(res < (long)min) res = (long)min;

    return (int)res;
}

// int multSafe(int a, int b) {
//     if(a == 0 || b == 0) return 0;

//     int res = a * b;

//     if(a == res / b)return res;

//     return (INT_MAX) * (a < 0 ? -1 : 1) * (b < 0 ? -1 : 1);
// }