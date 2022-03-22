#include "msp430.h"
#include "ports.h"
#include "adc.h"
#include "macros.h"
#include "wheels.h"
#include "functions.h"
#include "detectors.h"
#include "sm.h"
#include <string.h>

volatile unsigned int ADC_Channel;
volatile unsigned int ADC_Left_Detect, ADC_Right_Detect, ADC_Thumb, ADC_Vbat, ADC_Vdac, ADC_V3v3;
volatile unsigned int DAC_data;
char adc_char[5];
extern char display_line[4][11];
extern volatile unsigned char display_changed;
extern volatile unsigned int checkAdc;
extern volatile char state;
volatile unsigned int adcUpdated;

void Init_ADC(void) {
    //------------------------------------------------------------------------------
    // V_DETECT_L
    // V_DETECT_R
    // V_THUMB
    //------------------------------------------------------------------------------
    // ADCCTL0 Register
    ADCCTL0 = 0;
    ADCCTL0 |= ADCSHT_2;
    ADCCTL0 |= ADCMSC;
    ADCCTL0 |= ADCON;
    // ADCCTL1 Register
    ADCCTL1 = 0;
    ADCCTL1 |= ADCSHS_0;
    ADCCTL1 |= ADCSHP;
    ADCCTL1 &= ~ADCISSH;
    ADCCTL1 |= ADCDIV_0;
    ADCCTL1 |= ADCSSEL_0;
    ADCCTL1 |= ADCCONSEQ_0;
    // ADCCTL2 Register
    ADCCTL2 = 0;
    ADCCTL2 |= ADCPDIV0;
    ADCCTL2 |= ADCRES_1;
    ADCCTL2 &= ~ADCDF;
    ADCCTL2 &= ~ADCSR;
    // ADCMCTL0 Register
    ADCMCTL0 |= ADCSREF_0;
    ADCMCTL0 |= ADCINCH_5;

    ADCIE |= ADCIE0;
    ADCCTL0 |= ADCENC;
    ADCCTL0 |= ADCSC;
}

void Init_REF(void) {
    PMMCTL0_H = PMMPW_H;
    PMMCTL2 = INTREFEN;
    PMMCTL2 |= REFVSEL_2;

    while(!(PMMCTL2 & REFGENRDY));
}

void Init_DAC(void) {
    DAC_data = 10000;
    SAC3DAT = DAC_data;
    SAC3DAC = DACSREF_1;
    SAC3DAC |= DACLSEL_0;
    //  SAC3DAC |= DACIE;
    SAC3DAC |= DACEN;
    SAC3OA = NMUXEN;
    SAC3OA |= PMUXEN;
    SAC3OA |= PSEL_1;
    SAC3OA |= NSEL_1;
    SAC3OA |= OAPM;
    SAC3PGA = MSEL_1;
    SAC3OA |= SACEN;
    SAC3OA |= OAEN;
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void) {
    switch(__even_in_range(ADCIV, ADCIV_ADCIFG)) {
        case ADCIV_NONE:
            break;

        case ADCIV_ADCOVIFG:
            break;

        case ADCIV_ADCTOVIFG:
            break;

        case ADCIV_ADCHIIFG:
            break;

        case ADCIV_ADCLOIFG:
            break;

        case ADCIV_ADCINIFG:
            break;

        case ADCIV_ADCIFG:
            ADCCTL0 &= ~ADCENC;

            switch (ADC_Channel++) {
                case 0x00:
                    ADCMCTL0 &= ~ADCINCH_5;
                    ADCMCTL0 = ADCINCH_9;
                    ADC_Thumb = ADCMEM0;
                    ADC_Thumb = ADC_Thumb >> 2;
                    ADCCTL0 |= ADCSC;
                    //HEXtoBCD(ADC_Thumb,2,0);
                    break;

                case 0x01:
                    ADCMCTL0 &= ~ADCINCH_9;
                    ADCMCTL0 = ADCINCH_10;
                    ADC_Vbat = ADCMEM0;
                    ADC_Vbat = ADC_Vbat >> 2;
                    ADCCTL0 |= ADCSC;
                    //HEXtoBCD(ADC_Vbat,1);
                    break;

                case 0x02:
                    ADCMCTL0 &= ~ADCINCH_10;
                    ADCMCTL0 = ADCINCH_11;
                    ADC_Vdac = ADCMEM0;
                    ADC_Vdac = ADC_Vdac >> 2;
                    //HEXtoBCD(ADC_Vdac,2);
                    ADCCTL0 |= ADCSC;
                    break;

                case 0x03:
                    ADCMCTL0 &= ~ADCINCH_11;
                    ADCMCTL0 = ADCINCH_2;
                    ADC_V3v3 = ADCMEM0;
                    ADC_V3v3 = ADC_V3v3 >> 2;
                    //HEXtoBCD(ADC_V3v3,3);
                    ADCCTL0 |= ADCSC;
                    break;

                case 0x04:
                    ADCMCTL0 &= ~ADCINCH_2;
                    ADCMCTL0 = ADCINCH_3;
                    ADC_Left_Detect = ADCMEM0;
                    ADC_Left_Detect = ADC_Left_Detect >> 1;
                    ADCCTL0 |= ADCSC;
                    break;

                case 0x05:
                    ADCMCTL0 &= ~ADCINCH_3;
                    ADCMCTL0 = ADCINCH_5;
                    ADC_Right_Detect = ADCMEM0;
                    ADC_Right_Detect = ADC_Right_Detect >> 1;
                    break;

                case 0x06:
                    adcUpdated = 1;
                    ADC_Channel = 0;
                    break;

                default:
                    break;
            }

            ADCCTL0 |= ADCENC;

        default:
            break;
    }
}


void HEXtoBCD(int hex_value, int line, int start) {
    int value = 0;
    adc_char[0] = '0';

    while(hex_value > 999) {
        hex_value -= 1000;
        value += 1;
    }

    adc_char[0] = 0x30 + value;
    display_line[line][start] = 0x30 + value;
    value = 0;

    while(hex_value > 99) {
        hex_value -= 100;
        value += 1;
        adc_char[1] = 0x30 + value;
        display_line[line][start + 1] = 0x30 + value;
    }

    adc_char[1] = 0x30 + value;
    display_line[line][start + 1] = 0x30 + value;
    value = 0;

    while(hex_value > 9) {
        hex_value -= 10;
        value += 1;
    }

    adc_char[2] = 0x30 + value;
    display_line[line][start + 2] = 0x30 + value;
    adc_char[3] = 0x30 + hex_value;
    display_line[line][start + 3] = 0x30 + hex_value;
    adc_char[4] = 0;
    display_changed = 1;
}

