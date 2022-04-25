#include "serial.h"
#include "macros.h"
#include <string.h>
#include "msp430.h"

// global variables
extern volatile unsigned int usb0_rx_wr, usb1_rx_wr;
extern unsigned int usb0_rx_rd, usb1_rx_rd;
extern volatile char USB0_Char_Rx_Ring[SMALL_RING_SIZE], USB0_Char_Rx_Process[LARGE_RING_SIZE];
extern volatile char USB1_Char_Rx_Ring[SMALL_RING_SIZE], USB1_Char_Rx_Process[LARGE_RING_SIZE];
extern volatile char USB0_Char_Tx[LARGE_RING_SIZE], USB1_Char_Tx[LARGE_RING_SIZE];
extern unsigned volatile int pb0_index, pb1_index;
extern unsigned volatile int tx0_index, tx1_index;
extern unsigned volatile int pb0_buffered, pb1_buffered;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
extern unsigned volatile int serialState;
extern volatile char receievedFromPC;
//===========================================================================
// Function name: eUSCI_A0_ISR
//
// Description: This is the isr for USC0, it puts all retreivals into the
// ring buffer, and outputs what's in the tx buffer
//
// Passed : no variables passed
// Locals: temp
// Returned: no values returned
// Globals: USB0_Char_Rx,Ring,usb0_rx_wr,recievedFromPC,tx0_index
// USB0_Char_Tx
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
#pragma vector=EUSCI_A0_VECTOR
__interrupt void eUSCI_A0_ISR(void) {
    unsigned int temp;

    switch(__even_in_range(UCA0IV, 0x08)) {
        case 0:
            break;

        case 2: // RXIFG
            temp = usb0_rx_wr++;
            USB0_Char_Rx_Ring[temp] = UCA0RXBUF;

            if (usb0_rx_wr >= (SMALL_RING_SIZE)) {
                usb0_rx_wr = BEGINNING;
            }

            if(receievedFromPC) UCA1TXBUF = USB0_Char_Rx_Ring[temp];

            break;

        case 4: // TXIFG
            //if(receievedFromPC==OFF) {
            //  UCA0IE &= ~UCTXIE;
            //  return;
            //}
            UCA0TXBUF = USB0_Char_Tx[tx0_index];
            USB0_Char_Tx[tx0_index++] = '\0';

            if(USB0_Char_Tx[tx0_index] == false) {
                UCA0IE &= ~UCTXIE;
            }

            break;

        default:
            break;
    }
}
//===========================================================================
// Function name: eUSCI_A1_ISR
//
// Description: This is the isr for USC1, it puts all retreivals into the
// ring buffer, and outputs what's in the tx buffer
//
// Passed : no variables passed
// Locals: temp
// Returned: no values returned
// Globals: USB1_Char_Rx,Ring,usb1_rx_wr,recievedFromPC,tx1_index
// USB1_Char_Tx
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
#pragma vector=EUSCI_A1_VECTOR
__interrupt void eUSCI_A1_ISR(void) {
    unsigned int temp;

    switch(__even_in_range(UCA1IV, 0x08)) {
        case 0:
            break;

        case 2: // RXIFG
            if (!receievedFromPC)// && recieved=='\n')
                receievedFromPC = ON;

            temp = usb1_rx_wr++;
            USB1_Char_Rx_Ring[temp] = UCA1RXBUF;

            if(receievedFromPC)UCA0TXBUF = USB1_Char_Rx_Ring[temp];

            if (usb1_rx_wr >= (SMALL_RING_SIZE)) {
                usb1_rx_wr = BEGINNING;
            }

            break;

        case 4: // TXIFG
            if(receievedFromPC == OFF) {
                UCA1IE &= ~UCTXIE;
                return;
            }

            UCA1TXBUF = USB1_Char_Tx[tx1_index];
            USB1_Char_Tx[tx1_index++] = '\0';

            if(USB1_Char_Tx[tx1_index] == false) {
                UCA1IE &= ~UCTXIE;
            }

            break;

        default:
            break;
    }
}