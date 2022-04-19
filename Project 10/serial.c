//------------------------------------------------------------------------------
//
//  Description: This file contains the Serial Routine - Serial Processes
//
//
//  Ibrahim Moghul
//  Mar 2022
//  Built with IAR Embedded Workbench Version: (7.21.1)
//------------------------------------------------------------------------------
#include "serial.h"
#include "macros.h"
#include <string.h>
#include "msp430.h"

// global variables
volatile unsigned int usb0_rx_wr, usb1_rx_wr;
unsigned int usb0_rx_rd, usb1_rx_rd;
volatile char USB0_Char_Rx_Ring[SMALL_RING_SIZE], USB0_Char_Rx_Process[LARGE_RING_SIZE];
volatile char USB1_Char_Rx_Ring[SMALL_RING_SIZE], USB1_Char_Rx_Process[LARGE_RING_SIZE];
volatile char USB0_Char_Tx[LARGE_RING_SIZE], USB1_Char_Tx[LARGE_RING_SIZE];
unsigned volatile int pb0_index, pb1_index;
unsigned volatile int tx0_index, tx1_index;
unsigned volatile int pb0_buffered, pb1_buffered;
extern volatile unsigned char display_changed;
extern char display_line[4][11];
unsigned volatile int serialState;
volatile char receievedFromPC = OFF;

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
            USB0_Char_Tx[tx0_index++] = 0;

            if(USB0_Char_Tx[tx0_index] == 0) {
                UCA0IE &= ~UCTXIE;
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
            USB1_Char_Tx[tx1_index++] = 0;

            if(USB1_Char_Tx[tx1_index] == 0) {
                UCA1IE &= ~UCTXIE;
            }

            break;

        default:
            break;
    }
}

void clearProcessBuff(volatile char* pb, volatile unsigned int* pb_index, volatile unsigned int* pb_buffered) {
    for(int i = 0; i < LARGE_RING_SIZE; ++i)pb[i] = 0;

    *pb_index = 0;
    *pb_buffered = 0;
}
void clearProcessBuff_0(void) {
    clearProcessBuff(USB0_Char_Rx_Process, &pb0_index, &pb0_buffered);
}
void clearProcessBuff_1(void) {
    clearProcessBuff(USB1_Char_Rx_Process, &pb1_index, &pb1_buffered);
}

/*void out_character(char character) {
    //------------------------------------------------------------------------------
    // The while loop will stall as long as the Flag is not set [port is busy]
    while (!(UCA0IFG & UCTXIFG)); // USCI_A0 TX buffer ready?

    UCA0TXBUF = character;
    //------------------------------------------------------------------------------
}*/

void USCI_A0_transmit(void) {
    tx0_index = 0;
    UCA0IE |= UCTXIE;
}

void USCI_A1_transmit(void) {
    tx1_index = 0;
    UCA1IE |= UCTXIE;
}

void loadRingtoPB(volatile unsigned int* rx_wr, unsigned int* rx_rd, volatile char* Rx_Process, volatile char* Rx_Ring, volatile unsigned int* pb_index, volatile unsigned int* pb_buffered) {
    if(*pb_buffered) return;

    if(*rx_wr != *rx_rd) {
        Rx_Process[pb0_index] = Rx_Ring[*rx_rd];

        if(++(*rx_rd) > SMALL_RING_SIZE - 1) *rx_rd = BEGINNING;

        if(++(*pb_index) > LARGE_RING_SIZE - 1) *pb_index = BEGINNING;
    }

    if(*pb_index >= 2 && Rx_Process[(*pb_index) - 1] == '\n' && Rx_Process[(*pb_index) - 2] == '\r') {
        *pb_buffered = 1;
        *pb_index = BEGINNING;
    }
}

void loadRingtoPB_0(void) {
    loadRingtoPB(&usb0_rx_wr, &usb0_rx_rd, USB0_Char_Rx_Process, USB0_Char_Rx_Ring, &pb0_index, &pb0_buffered);
}
void loadRingtoPB_1(void) {
    loadRingtoPB(&usb1_rx_wr, &usb1_rx_rd, USB1_Char_Rx_Process, USB1_Char_Rx_Ring, &pb1_index, &pb1_buffered);
}


void SerialProcess(void) {
    /*if(!pb0_buffered)*/loadRingtoPB_0();

    /*if(!pb1_buffered)*/loadRingtoPB_1();

}