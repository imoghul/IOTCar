#include "msp430.h"
#include "switches.h"
#include "ports.h"
#include "wheels.h"
volatile unsigned int sw1Okay, sw2Okay;
volatile unsigned int count_debounce_SW1, count_debounce_SW2;
volatile unsigned int sw1_pos, sw2_pos;
extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_seconds;
extern volatile char state;

void SwitchesProcess(void){
  Switch1Process();
  Switch2Process();
}
void Switch1Process(void){
  if(sw1Okay && sw1_pos){
    if(!(P4IN & SW1)){
      sw1_pos = PRESSED;
      sw1Okay = NOT_OKAY;
      count_debounce_SW1 = DEBOUNCE_RESTART;
      if(state == START){
        stopwatch_seconds = 0;
        cycle_count = 0;
        state = WAIT;
      }
    }
  }  
  if(count_debounce_SW1 <= DEBOUNCE_TIME){
    count_debounce_SW1++;
  }
  else{
    sw1Okay = OKAY;
    if(P4IN & SW1){
      sw1_pos = RELEASED;
    }
  }
}
void Switch2Process(void){
  if(sw2Okay && sw2_pos){
    if(!(P2IN & SW1)){
      sw2_pos = PRESSED;
      sw2Okay = NOT_OKAY;
      count_debounce_SW2 = DEBOUNCE_RESTART;
      //stopwatch_seconds = 0;
      //cycle_count = 0;
      //state = WAIT;
    }
  }  
  if(count_debounce_SW2 <= DEBOUNCE_TIME){
    count_debounce_SW2++;
  }
  else{
    sw2Okay = OKAY;
    if(P2IN & SW2){
      sw2_pos = RELEASED;
    }
  }
}