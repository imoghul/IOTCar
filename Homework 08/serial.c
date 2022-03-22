#include "serial.h"
//----------------------------------------------------------------------------
void Init_Serial_UCA0(void) {
    int i;

    for(i = 0; i < SMALL_RING_SIZE; i++) {
        USB_Char_Rx[i] = 0x00;
    }

    usb_rx_ring_wr = BEGINNING;
    usb_rx_ring_rd = BEGINNING;

    for(i = 0; i < LARGE_RING_SIZE; i++) {
        USB_Char_Tx[i] = 0x00;
    }

    usb_tx_ring_wr = BEGINNING;
    usb_tx_ring_rd = BEGINNING;
    // Configure UART 0
    UCA0CTLW0 = 0;
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BRW = 52;
    UCA0MCTLW = 0x4911 ;
    UCA0CTLW0 &= ~ UCSWRST;
    UCA0IE |= UCRXIE;
}


// global variables
volatile unsigned int usb_rx_ring_wr;
volatile char USB_Char_Rx[SMALL_RING_SIZE] ;
//------------------------------------------------------------------------------
#pragma vector=EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
    unsigned int temp;

    switch(__even_in_range(UCA0IV, 0x08)) {
        case 0:
            break;

        case 2:
            temp = usb_rx_ring_wr++;
            USB_Char_Rx[temp] = UCA0RXBUF;

            if (usb_rx_ring_wr >= (sizeof(USB_Char_Rx))) {
                usb_rx_ring_wr = BEGINNING;
            }

            break;

        case 4:
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
