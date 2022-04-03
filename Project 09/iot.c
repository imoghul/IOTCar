#include "iot.h"
#include "msp430.h"
#include <string.h>
#include "serial.h"

char iot_setup_state = BOOT_UP;
extern volatile char USB0_Char_Tx[];
extern unsigned volatile int pb0_buffered;
extern volatile char USB0_Char_Rx_Process[];
extern volatile char receievedFromPC;


void Init_IOT(void){
  //if(!receievedFromPC) return;
  
  switch(iot_setup_state){
    case BOOT_UP:
      if(pb0_buffered){
        if(strcmp((char*)USB0_Char_Rx_Process,BOOT_RESPONSE) == 0){
          iot_setup_state=CIPMUX_Tx;
        }
      clearProcessBuff_0();
      }
      break;
    case CIPMUX_Tx:
      strcpy((char*)USB0_Char_Tx,ALLOW_MULTIPLE_CONNECTIONS);
      USCI_A0_transmit();
      iot_setup_state = CIPMUX_Rx;
      break;
    case CIPMUX_Rx:
      if(UCA0IE & UCTXIE) break;
      if(pb0_buffered){
        if(strcmp((char*)USB0_Char_Rx_Process,OK_RESPONSE) == 0) {
          iot_setup_state = CIPSERVER_Tx;
        }
        else iot_setup_state = CIPMUX_Tx;
        clearProcessBuff_0();
      }
      break;
    case CIPSERVER_Tx:
      strcpy((char*)USB0_Char_Tx,START_SERVER);
      USCI_A0_transmit();
      iot_setup_state = CIPSERVER_Rx;
      break;
    case CIPSERVER_Rx:
      if(UCA0IE & UCTXIE) break;
      if(pb0_buffered){
        if(strcmp((char*)USB0_Char_Rx_Process,OK_RESPONSE) == 0) {
          iot_setup_state = IOT_SETUP_FINISHED;
        }
        else iot_setup_state = CIPSERVER_Tx;
        clearProcessBuff_0();
      }
      break;
    default:
      break;
  }
  
}