#include "serial.h"
#include <string.h>
#include "msp430.h"

// global variables
volatile unsigned int usb0_rx_wr,usb0_rx_rd,usb1_rx_wr,usb1_rx_rd ;
volatile char USB0_Char_Rx_Ring[SMALL_RING_SIZE],USB0_Char_Rx_Process[LARGE_RING_SIZE];
volatile char USB1_Char_Rx_Ring[SMALL_RING_SIZE],USB1_Char_Rx_Process[LARGE_RING_SIZE];
volatile char USB0_Char_Tx[LARGE_RING_SIZE],USB1_Char_Tx[LARGE_RING_SIZE];
unsigned volatile int pb0_index,pb1_index;
unsigned volatile int tx0_index,tx1_index;
unsigned volatile int pb0_buffered,pb1_buffered;
extern volatile unsigned char display_changed;
extern char display_line[4][11];

//----------------------------------------------------------------------------
void Init_Serial_UCA(void) {
    int i;

    for(i = 0; i < SMALL_RING_SIZE; i++) {
        USB0_Char_Rx_Ring[i] = 0x00;
        USB1_Char_Rx_Ring[i] = 0x00;
    }

    usb0_rx_wr = BEGINNING;
    usb0_rx_rd = BEGINNING;
    usb1_rx_wr = BEGINNING;
    usb1_rx_rd = BEGINNING;

    for(i = 0; i < LARGE_RING_SIZE; i++) {
        USB0_Char_Tx[i] = 0x00;
        USB1_Char_Tx[i] = 0x00;
    }

    //usb0_tx_ring_wr = BEGINNING;
    //usb0_tx_ring_rd = BEGINNING;
    //usb1_tx_ring_wr = BEGINNING;
    //usb1_tx_ring_rd = BEGINNING;
    
    // Configure UART 0
    UCA0CTLW0 = 0;
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 4;
    UCA0MCTLW = 0x5551;
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
    // Configure UART 1
    UCA1CTLW0 = 0;
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;
    UCA1BRW = 4;
    UCA1MCTLW = 0x5551;
    UCA1CTLW0 &= ~UCSWRST;
    UCA1IE |= UCRXIE;
}
//------------------------------------------------------------------------------
#pragma vector=EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
    unsigned int temp;

    switch(__even_in_range(UCA0IV, 0x08)) {
        case 0:
            break;

        case 2: // RXIFG
            temp = usb0_rx_wr++;
            USB0_Char_Rx_Ring[temp] = UCA0RXBUF;
            
            if (usb0_rx_wr >= (sizeof(USB0_Char_Rx_Ring))) {
                usb0_rx_wr = BEGINNING;
            }

            break;

        case 4: // TXIFG
          UCA0TXBUF = USB0_Char_Tx[tx0_index];
          USB0_Char_Tx[tx0_index++] = 0;
          if(USB0_Char_Tx[tx0_index] == 0) {
            UCA0IE &= ~UCTXIE;
            clearProcessBuff0();
          }
          break;

        default:
            break;
    }
}



void clearProcessBuff0(){
  for(int i = 0;i<sizeof(USB0_Char_Rx_Process);++i)USB0_Char_Rx_Process[i]=0;
  pb0_index=0;
  pb0_buffered=0;
}

void out_character(char character) {
    //------------------------------------------------------------------------------
    // The while loop will stall as long as the Flag is not set [port is busy]
    while (!(UCA0IFG & UCTXIFG)); // USCI_A0 TX buffer ready?

    UCA0TXBUF = character;
    //------------------------------------------------------------------------------
}

void USCI_A0_transmit(void){
  tx0_index=0;
  UCA0IE |= UCTXIE;
}
void USCI_A1_transmit(void){
  tx1_index=0;
  UCA1IE |= UCTXIE;
}

void loadRingtoPB_0(void){
  if(usb0_rx_wr != usb0_rx_rd){
    USB0_Char_Rx_Process[pb0_index] = USB0_Char_Rx_Ring[usb0_rx_rd];
    if(usb0_rx_rd++ >= SMALL_RING_SIZE-1) usb0_rx_rd = BEGINNING;
    if(pb0_index++ >= LARGE_RING_SIZE-1) pb0_index=BEGINNING;
  }
  if(pb0_index>=2 && USB0_Char_Rx_Process[pb0_index-1]=='\n' && USB0_Char_Rx_Process[pb0_index-2]=='\r') {
    pb0_buffered = 1;
    pb0_index = BEGINNING;
  }
}

void copyPBtoTx_0(void){
  if(!pb0_buffered)return;
  for(int i = 0;i<sizeof(USB0_Char_Rx_Process);++i) USB0_Char_Tx[i] = USB0_Char_Rx_Process[i];
  strcpy(display_line[3],(char*)USB0_Char_Tx);
  for(int i = 0;i<10;++i)
    if(display_line[3][i] == '\r' || display_line[3][i]=='\n') display_line[3][i]=0;
  clearProcessBuff0();
}



//int i = 0;
            //for(;i<10 && USB1_Char_Rx[i]!=0;++i)
            //  display_line[1][i] = USB1_Char_Rx[i];
            //for(;i<10;++i) display_line[1][i]=' ';
            //display_line[1][10] = 0;
        