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

short l_LessBlack,l_LessGray,l_LessWhite;
short r_LessBlack,r_LessGray,r_LessWhite;
short l_GreaterBlack,l_GreaterGray,l_GreaterWhite;
short r_GreaterBlack,r_GreaterGray,r_GreaterWhite;
short lessWhiteOr,lessWhiteAnd,greaterWhiteOr,greaterWhiteAnd;
short lessWhiteOr,lessWhiteAnd,greaterWhiteOr,greaterWhiteAnd;
short lessGrayOr,lessGrayAnd,greaterGrayOr,greaterGrayAnd;
short lessGrayOr,lessGrayAnd,greaterGrayOr,greaterGrayAnd;
short lessBlackOr,lessBlackAnd,greaterBlackOr,greaterBlackAnd;
short lessBlackOr,lessBlackAnd,greaterBlackOr,greaterBlackAnd;

void updateDetectors(void){
  l_LessBlack = ADC_Left_Detect<LEFT_WHITE_DETECT;
  r_LessBlack = ADC_Right_Detect<RIGHT_WHITE_DETECT;
}

void calibrate(void) {
    if(calibrationMode == 0) {
        unsigned int left = ADC_Left_Detect, right = ADC_Right_Detect;

        if (left > LBDetect) LBDetect = left;

        if (right > RBDetect) RBDetect = right;
    } else if(calibrationMode == 1) {
        unsigned int left = ADC_Left_Detect, right = ADC_Right_Detect;

        if (left > LWDetect) LWDetect = left;

        if (right > RWDetect) RWDetect = right;
    }

    HEXtoBCD(LBDetect, 2, 6);
    HEXtoBCD(RBDetect, 2, 0);
    HEXtoBCD(LWDetect, 1, 6);
    HEXtoBCD(RWDetect, 1, 0);
}