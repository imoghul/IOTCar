#include "msp430.h"
#include "ports.h"
#include "adc.h"

void Init_ADC(void){
//------------------------------------------------------------------------------
// V_DETECT_L
// V_DETECT_R
// V_THUMB
//------------------------------------------------------------------------------
// ADCCTL0 Register
(0x04) // Pin 2 A2
(0x08) // Pin 3 A3
(0x20) // Pin 5 A5
ADCCTL0 = 0;
  ADCCTL0 |= ADCSHT_2;
  ADCCTL0 |= ADCMSC;
  ADCCTL0 |= ADCON;
// ADCCTL1 Register
ADCCTL2 = 0;
ADCCTL1 |= ADCSHS_0; ADCCTL1 |= ADCSHP; ADCCTL1 &= ~ADCISSH; ADCCTL1 |= ADCDIV_0; ADCCTL1 |= ADCSSEL_0; ADCCTL1 |= ADCCONSEQ_0;
// Reset
// 16 ADC clocks
// MSC
// ADC ON
// Reset
// 00b = ADCSC bit
// ADC sample-and-hold SAMPCON signal from sampling timer.
// ADC invert signal sample-and-hold.
// ADC clock divider - 000b = Divide by 1
// ADC clock MODCLK
// ADC conversion sequence 00b = Single-channel single-conversion
// ADCCTL1 & ADCBUSY  identifies a conversion is in process
// ADCCTL2 Register
  ADCCTL2 = 0;
  ADCCTL2 |= ADCPDIV0;
  ADCCTL2 |= ADCRES_2;
  ADCCTL2 &= ~ADCDF;
  ADCCTL2 &= ~ADCSR;
// ADCMCTL0 Register
  ADCMCTL0 |= ADCSREF_0;
  ADCMCTL0 |= ADCINCH_5;
  ADCIE |= ADCIE0;
  ADCCTL0 |= ADCENC;
  ADCCTL0 |= ADCSC;
// Reset
// ADC pre-divider 00b = Pre-divide by 1
// ADC resolution 10b = 12 bit (14 clock cycle conversion time) // ADC data read-back format 0b = Binary unsigned.
// ADC sampling rate 0b = ADC buffer supports up to 200 ksps
// VREF - 000b = {VR+ = AVCC and VR– = AVSS }
// V_THUMB (0x20) Pin 5 A5
// Enable ADC conv complete interrupt
// ADC enable conversion.
// ADC start conversion.  
}

#pragma vector=ADC_VECTOR 
__interrupt void ADC_ISR(void){
switch(__even_in_range(ADCIV,ADCIV_ADCIFG)){ 
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
	switch (ADC_Channel4+){
		case 0x00:
			ADCMCTL0 &= ~ADCINCH_2; 
			ADCMCTL0 = ADCINCH_3; 
			ADC_Left_Detect = ADCMEM0; 
			ADC_Left_Detect = ADC_Left_Detect >> 2; 
			HEXtoBCD (ADC_Left Detect); 
			adc_line4(0);
			break; 
		case 0x01:
			break; 
		case 0x0?:
			ADC_Channel=0;
			 break; 
		default: break;
	}
	ADCCTL0 |= ADCENC; 
	ADCCTL0 |= ADCSC;
	default: break;
} 
default: break;
}

void Init_REF(void){
  PMMCTL0_H = PMMPW_H;
  PMMCTL2 = INTREFEN;
  PMMCTL2 |= REFVSEL_2;
  while(!(PMMCTL2 & REFGENRDY));
}

void Init_DAC(void){
  DAC_data = 1000;
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
