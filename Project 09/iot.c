#include "iot.h"
#include "msp430.h"
#include <string.h>
#include "serial.h"

char iot_setup_state = BOOT_UP;
extern volatile char USB0_Char_Tx[];
extern unsigned volatile int pb0_buffered;
extern volatile char USB0_Char_Rx_Process[];
extern volatile char receievedFromPC;
char SSID[SSID_LEN+1];
char IP[IP_LEN+1];
extern volatile unsigned char display_changed;
extern char display_line[4][11];


int Init_IOT(void){
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
      if(UCA0IE & UCTXIE) break; // wait for the Tx to completely transmit
      if(pb0_buffered){ // wait for pb to finish buffering
        if(strcmp((char*)USB0_Char_Rx_Process,OK_RESPONSE) == 0) { //  check if the response was "OK"
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
          iot_setup_state = GET_SSID_Tx;
        }
        else iot_setup_state = CIPSERVER_Tx;
        clearProcessBuff_0();
      }
      break;
      
    case GET_SSID_Tx:
      strcpy((char*)USB0_Char_Tx,SSID_COMMAND);
      USCI_A0_transmit();
      iot_setup_state = GET_SSID_Rx;
      break;
    case GET_SSID_Rx:
      if(UCA0IE & UCTXIE) break;
      if(pb0_buffered){
        if(strncmp(SSID_RESPONSE,(char*)USB0_Char_Rx_Process,SSID_RESPONSE_LEN) == 0) {
          int i;
          for(i = 0;i<=SSID_LEN && USB0_Char_Rx_Process[i+SSID_RESPONSE_LEN+1]!='\"';++i) SSID[i] = USB0_Char_Rx_Process[i+SSID_RESPONSE_LEN+1];
          SSID[i+SSID_RESPONSE_LEN+2] = 0;
          SSID[SSID_LEN] = 0;
          strcpy(display_line[0],SSID);
          display_changed = 1;
          iot_setup_state = GET_IP_Tx;
        }
        else iot_setup_state = GET_SSID_Tx;
        clearProcessBuff_0();
      }
      break;
      
     case GET_IP_Tx:
      strcpy((char*)USB0_Char_Tx,IP_COMMAND);
      USCI_A0_transmit();
      iot_setup_state = GET_IP_Rx;
      break;
    case GET_IP_Rx:
      if(UCA0IE & UCTXIE) break;
      if(pb0_buffered){
        if(strncmp(IP_RESPONSE,(char*)USB0_Char_Rx_Process,IP_RESPONSE_LEN) == 0) {
          int i;
          char dotFound;
          int midIndex;
          for(i = 0;i<=IP_LEN && USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1]!='"';++i) {
            IP[i] = USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1];
            if(USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1]=='.'){
              if(!dotFound)dotFound = 1;
              else {
                dotFound = 0;
                midIndex = i;
              }
            }
          }
          IP[i+IP_RESPONSE_LEN+2] = 0;
          IP[IP_LEN] = 0;
          IP[midIndex] = 0;
          strcpy(display_line[1],"IP ADDRESS");
          centerStringToDisplay(2,IP);
          centerStringToDisplay(3,IP+midIndex+1);
          display_changed = 1;
          iot_setup_state = IOT_SETUP_FINISHED;
        }
        else iot_setup_state = GET_IP_Tx;
        clearProcessBuff_0();
      }
      break;
    
    default:
      return 1;
      break;
  }
  return 0;
}


void centerStringToDisplay(unsigned int line,char * s){
  strcpy(display_line[line]+((strlen(s)%2)?(strlen(s)>>1):((strlen(s)>>1) - 1)),s);
}