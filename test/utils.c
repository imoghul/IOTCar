#include <string.h>
#include "utils.h"
#include "macros.h"
extern volatile unsigned char display_changed;
extern char display_line[4][11];

void centerStringToDisplay(unsigned int line, char * s) {
    strcpy(display_line[line] + ((10 - strlen(s)) >> 1), s);
}

char* subStringPos(const char* str,char * subString) {

    int i = 0;
    int d = 0;

    int lenSub = strlen(subString);
    int len = strlen(str) - lenSub;
    
    for (i = 0; i < len;i++) {
        int exists = 1;

        for (d = 0; d < lenSub; d++) {
            if (str[i + d] != subString[d]) {
                exists = 0;
                break;
            }
        }

        if (exists) return (char*)(str + i);
    }
    return 0;
}

int stoi(char* str,int len) {
    int num = 0;
    for(int i = 0; i < len && str[i] >= '0' && str[i] <= '9'; ++i)
        num = num * 10 + (int)(str[i] - '0');
    return num;
}

char* charInString(const char* str, char c) {
    for(int i = 0; i < strlen(str)+1; i++)
        if(str[i] == c) return (char*)(str + i);

    return 0;
}

unsigned int absVal(int n) {
    const int ret[2] = {n, -n};
    return (unsigned int)(ret [n < 0]);
}

void HEXtoBCD(int hex_value, int line, int start) {
    int value = 0;

    while(hex_value > 999) {
        hex_value -= 1000;
        value += 1;
    }

    display_line[line][start] = 0x30 + value;
    value = 0;

    while(hex_value > 99) {
        hex_value -= 100;
        value += 1;
    }

    display_line[line][start + 1] = 0x30 + value;
    value = 0;

    while(hex_value > 9) {
        hex_value -= 10;
        value += 1;
    }

    display_line[line][start + 2] = 0x30 + value;
    display_line[line][start + 3] = 0x30 + hex_value;
}


int additionSafe(int val, int max, int min, int increment) {
    long res = val + increment;

    if(res > (long)max) res = (long)max;

    if(res < (long)min) res = (long)min;

    return (int)res;
}

int multSafe(int a, int b) {
    if(a == 0 || b == 0) return 0;

    int res = a * b;

    if(a == res / b)return res;
    return (INT_MAX) * (a < 0 ? -1 : 1) * (b < 0 ? -1 : 1);
}