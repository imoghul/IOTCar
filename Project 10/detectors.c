#include "msp430.h"
#include "ports.h"
#include "adc.h"
#include "wheels.h"
#include <string.h>
#include "sm.h"
#include "detectors.h"
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char movingDirection;
int rightVals[VALUES_TO_HOLD];
extern volatile unsigned int ADC_Left_Detect, ADC_Right_Detect;
int lastLeft;
int lastRight;
int leftVals[VALUES_TO_HOLD];
extern volatile unsigned int adcUpdated;
extern volatile unsigned int calibrationMode;
unsigned int LBDetect, LWDetect, RBDetect, RWDetect;

short l_LessBlack, l_LessGray, l_LessWhite;
short r_LessBlack, r_LessGray, r_LessWhite;
short l_GreaterBlack, l_GreaterGray, l_GreaterWhite;
short r_GreaterBlack, r_GreaterGray, r_GreaterWhite;
short lessWhiteOr, lessWhiteAnd, greaterWhiteOr, greaterWhiteAnd;
short lessWhiteOr, lessWhiteAnd, greaterWhiteOr, greaterWhiteAnd;
short lessGrayOr, lessGrayAnd, greaterGrayOr, greaterGrayAnd;
short lessGrayOr, lessGrayAnd, greaterGrayOr, greaterGrayAnd;
short lessBlackOr, lessBlackAnd, greaterBlackOr, greaterBlackAnd;
short lessBlackOr, lessBlackAnd, greaterBlackOr, greaterBlackAnd;

void updateDetectors(void) {
    /*l_LessBlack = ADC_Left_Detect < LEFT_BLACK_DETECT;
    r_LessBlack = ADC_Right_Detect < RIGHT_BLACK_DETECT;
    l_LessGray = ADC_Left_Detect < LEFT_GRAY_DETECT;
    r_LessGray = ADC_Right_Detect < RIGHT_GRAY_DETECT;*/
    l_LessWhite = ADC_Left_Detect < LEFT_WHITE_DETECT;
    r_LessWhite = ADC_Right_Detect < RIGHT_WHITE_DETECT;
    //
    /*l_GreaterBlack = ADC_Left_Detect > LEFT_BLACK_DETECT;
    r_GreaterBlack = ADC_Right_Detect > RIGHT_BLACK_DETECT;
    l_GreaterGray = ADC_Left_Detect > LEFT_GRAY_DETECT;
    r_GreaterGray = ADC_Right_Detect > RIGHT_GRAY_DETECT;*/
    l_GreaterWhite = !l_LessWhite;//ADC_Left_Detect > LEFT_WHITE_DETECT;
    r_GreaterWhite = !r_LessWhite;//ADC_Right_Detect > RIGHT_WHITE_DETECT;
    //
    lessWhiteOr = l_LessWhite || r_LessWhite;
    lessWhiteAnd = l_LessWhite && r_LessWhite;
    greaterWhiteAnd = l_GreaterWhite && r_GreaterWhite;
    greaterWhiteOr = l_GreaterWhite || r_GreaterWhite;
}

void calibrate(void) {
    unsigned int left = ADC_Left_Detect, right = ADC_Right_Detect;
    int * leftDetect = calibrationMode ? &LWDetect : &LBDetect;
    int * rightDetect = calibrationMode ? &RWDetect : &RBDetect;

    if (left > *leftDetect) *leftDetect = left;

    if (right > *rightDetect) *rightDetect = right;

    HEXtoBCD((int)LBDetect, 3, 6);
    HEXtoBCD((int)RBDetect, 3, 0);
    HEXtoBCD((int)LWDetect, 2, 6);
    HEXtoBCD((int)RWDetect, 2, 0);
    HEXtoBCD((int)left, 0, 6);
    HEXtoBCD((int)right, 0, 0);
    display_changed = 1;
}