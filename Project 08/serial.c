#include "serial.h"
#include <string.h>
#include "msp430.h"

// global variables
volatile unsigned int usb0_rx_ring_wr,usb0_rx_ring_rd,usb1_rx_ring_wr,usb1_rx_ring_rd ;
volatile char USB0_Char_Rx[SMALL_RING_SIZE],USB1_Char_Rx[SMALL_RING_SIZE];
volatile char USB0_Char_Tx[LARGE_RING_SIZE],USB1_Char_Tx[LARGE_RING_SIZE];
volatile char String1[] = "STRINGNUM1";
volatile char String2[] = "STRINGNUM2";
volatile char String3[] = "STRINGNUM3";
volatile char String4[] = "STRINGNUM4";
volatile char String5[] = "STRINGNUM5";
volatile char String6[] = "STRINGNUM6";
extern unsigned volatile UCA0_index,UCA1_index;
extern volatile unsigned char display_changed;
extern char display_line[4][11];

//----------------------------------------------------------------------------
void Init_Serial_UCA(void) {
    int i;

    for(i = 0; i < SMALL_RING_SIZE; i++) {
        USB0_Char_Rx[i] = 0x00;
        USB1_Char_Rx[i] = 0x00;
    }

    usb0_rx_ring_wr = BEGINNING;
    usb0_rx_ring_rd = BEGINNING;
    usb1_rx_ring_wr = BEGINNING;
    usb1_rx_ring_rd = BEGINNING;

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
            temp = usb0_rx_ring_wr++;
            USB0_Char_Rx[temp] = UCA0RXBUF;
            
            if (usb0_rx_ring_wr >= (sizeof(USB0_Char_Rx)) || (temp>=1 && USB0_Char_Rx[temp]=='\n' && USB0_Char_Rx[temp-1]=='\r')) {
                usb0_rx_ring_wr = BEGINNING;
                UCA0_index = 0;
            }
            
            strcpy(display_line[3], "          ");
            int i = 0;
            for(;i<10 && USB0_Char_Rx[i]!=0;++i)
              display_line[3][i] = USB0_Char_Rx[i];
            for(;i<10;++i) display_line[0][i]=' ';
            display_line[3][10] = 0;

            break;

        case 4: // TXIFG
          switch(UCA0_index++){
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
              UCA0TXBUF = USB0_Char_Tx[UCA0_index];
              break;
            case 9:
              UCA0TXBUF = 0x0D;
              break;
            case 10:
              UCA0TXBUF = 0x0A;
              break;
            default:
              UCA0IE &= ~UCTXIE;
              break;
            
          }
          break;

        default:
            break;
    }
}

#pragma vector=EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
    unsigned int temp;

    switch(__even_in_range(UCA1IV, 0x08)) {
        case 0:
            break;

        case 2: // RXIFG
            temp = usb1_rx_ring_wr++;
            USB1_Char_Rx[temp] = UCA1RXBUF;
            
            if (usb1_rx_ring_wr >= (sizeof(USB1_Char_Rx)) || (temp>=1 && USB1_Char_Rx[temp]=='\n' && USB1_Char_Rx[temp-1]=='\r')) {
                usb1_rx_ring_wr = BEGINNING;
                UCA1_index = 0;
            }
            
            /*int i = 0;
            for(;i<10 && USB1_Char_Rx[i]!=0;++i)
              display_line[1][i] = USB1_Char_Rx[i];
            for(;i<10;++i) display_line[1][i]=' ';
            display_line[1][10] = 0;*/
        

            break;

        case 4: // TXIFG
          switch(UCA1_index++){
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
              UCA1TXBUF = USB1_Char_Tx[UCA1_index];
              break;
            case 9:
              UCA1TXBUF = 0x0D;
              break;
            case 10:
              UCA1TXBUF = 0x0A;
              break;
            default:
              UCA1IE &= ~UCTXIE;
              break;
            
          }
          break;

        default:
            break;
    }
}

void out_character(char character) {
    //------------------------------------------------------------------------------
    // The while loop will stall as long as the Flag is not set [port is busy]
    while (!(UCA0IFG & UCTXIFG)); // USCI_A0 TX buffer ready?

    UCA0TXBUF = character;
    //------------------------------------------------------------------------------
}
