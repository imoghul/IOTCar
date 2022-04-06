#include "iot.h"
#include <string.h>
#include "msp430.h"
#include <string.h>
#include "adc.h"
#include "serial.h"
#include "ports.h"
#include "sm.h"
#include <stdlib.h>

char iot_setup_state = BOOT_UP;
extern volatile char USB0_Char_Tx[];
extern unsigned volatile int pb0_buffered;
extern volatile char USB0_Char_Rx_Process[];
extern volatile char receievedFromPC;
char SSID[SSID_LEN+1];
char IP[IP_LEN+1];
extern volatile unsigned char display_changed;
extern char display_line[4][11];
char dotFound;
int midIndex;
char midFound;
command CommandBuffer[COMMAND_BUFFER_LEN];
char cb_index;

extern volatile unsigned int cycle_count;
extern volatile unsigned int stopwatch_milliseconds;
extern volatile unsigned int stopwatch_seconds;

extern volatile char state;
extern volatile int stateCounter;
extern volatile char nextState;

extern int polarityRight, polarityLeft;
extern unsigned int driveTime;

command emptyCommand = {0,0};


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
        iot_setup_state = CIPSERVER_Tx;
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
        iot_setup_state = GET_SSID_Tx;
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
          centerStringToDisplay(0,SSID);
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

          for(i = 0;i<=IP_LEN && USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1]!='"';++i) {
            IP[i] = USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1];
            if(USB0_Char_Rx_Process[i+IP_RESPONSE_LEN+1]=='.'){
              if(!dotFound)dotFound = 1;
              else {
                dotFound = 0;
                if(!midFound){
                  midIndex = i;
                  midFound = 1;
                }
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
  strcpy(display_line[line]+((10-strlen(s))>>1),s);
}

void IOTBufferCommands(void){
  if(pb0_buffered) {
    char * pos = strstr((char*)USB0_Char_Rx_Process,CARET_SECURITY_CODE);
    while(pos){
      pos+=CARET_SECURITY_CODE_LEN; // now should be on where the command actually is
      char comm = *pos;
      pos++;
      char * end_caret = strchr(pos,'^');
      char * end_null = strchr(pos,0);
      char * end = end_caret?end_caret:end_null;
      int time = stoi(pos);//strtol(pos,&end,10);
      command c = {
        .comm = comm,
        .duration = time
      };
      pushCB(c);
      pos = strstr(pos,CARET_SECURITY_CODE);
    }
    clearProcessBuff_0();
  }
  
}

int stoi(char* str){
  
  int num = 0;
  int n = strlen(str);
  for(int i =0;i<n && str[i]>=48 && str[i]<=57;++i)
    num = num*10+(int)(str[i]-48);
  return num;
}

command popCB(void){
  command ret = CommandBuffer[0];
  for(int i = 0;i<COMMAND_BUFFER_LEN-1;++i) CommandBuffer[i] = CommandBuffer[i+1];
  CommandBuffer[COMMAND_BUFFER_LEN-1] = emptyCommand;
  return ret;
}
void pushCB(command c){
  int i;
  for(i = 0;i<COMMAND_BUFFER_LEN;++i)
    if(CommandBuffer[i].comm==0 && CommandBuffer[i].duration==0) break;
  if(i==COMMAND_BUFFER_LEN) {
    return;
  }
  CommandBuffer[i] = c;
}

void ProcessCommands(void){
  if(state == START && (CommandBuffer[0].comm!=0 && CommandBuffer[0].duration!=0)){
    command c = popCB();
    stopwatch_seconds = 0;
    cycle_count = 0;
    state = DRIVE;
    display_line[1][2] = c.comm;
    HEXtoBCD(c.duration,1,4);
    driveTime = (int)(c.duration*(c.comm==RIGHT_COMMAND||c.comm==LEFT_COMMAND?TURN_CONSTANT:1));
    switch(c.comm){
      case (FORWARD_COMMAND):
        polarityRight = 1;
        polarityLeft = 1;
        break;
      case (REVERSE_COMMAND):
        polarityRight = -1;
        polarityLeft = -1;
        break;
      case (RIGHT_COMMAND):
        polarityRight = 1;
        polarityLeft = -1;
        break;
      case (LEFT_COMMAND):
        polarityRight = -1;
        polarityLeft = 1;
        break;
      case (LINEFOLLOW_COMMAND):
        state = STRAIGHT;
        break;
      default:
        state = START;
        break;
    }
  }
}