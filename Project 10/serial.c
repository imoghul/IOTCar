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
volatile char receievedFromPC;

//===========================================================================
// Function name: Init_Serial_UCA
//
// Description: This function initializes the serial ports of the FRAM board
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: usb0_rx_wr, usb0_rx_rd, usb1_rx_wr, usb1_rx_rd, USB0_Char_Rx_Ring
// , USB1_Char_Rx_ring, USB0_Char_Tx, USB1_Char_Tx
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
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
    // UCA0BRW = 4;
    // UCA0MCTLW = 0x5551;
    SET_UCA_115200;
    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
    
    // Configure UART 1
    /*UCA1CTLW0 = 0;
    UCA1CTLW0 |= UCSWRST;
    UCA1CTLW0 |= UCSSEL__SMCLK;
    // UCA1BRW = 4;
    // UCA1MCTLW = 0x5551;
    UCA1CTLW0 &= ~UCSWRST;
    UCA1IE |= UCRXIE;*/
    UCA1CTLW0 = UCA0CTLW0;
    UCA1IE |= UCRXIE;
}


//===========================================================================
// Function name: clearProcessBuff
//
// Description: generic function to clear a process buffer
//
// Passed : pb, pb_Index, pb_buffered
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void clearProcessBuff(volatile char* pb, volatile unsigned int* pb_index, volatile unsigned int* pb_buffered) {
    for(int i = 0; i < LARGE_RING_SIZE; ++i)pb[i] = '\0';

    *pb_index = BEGINNING;
    *pb_buffered = false;
}

//===========================================================================
// Function name: clearProcessBuff_0
//
// Description: This function is used to clear the process buffer for USC0
//
// Passed : pb, pb_Index, pb_buffered
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void clearProcessBuff_0(void) {
    clearProcessBuff(USB0_Char_Rx_Process, &pb0_index, &pb0_buffered);
}

//===========================================================================
// Function name: clearProcessBuff_1
//
// Description: This function is used to clear the process buffer for USC1
//
// Passed : pb, pb_Index, pb_buffered
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
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

//===========================================================================
// Function name: USCI_A0_transmit
//
// Description: This function is used to transmit from USC0
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: tx0_index
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void USCI_A0_transmit(void) {
    tx0_index = BEGINNING;
    UCA0IE |= UCTXIE;
}

//===========================================================================
// Function name: USCI_A1_transmit
//
// Description: This function is used to transmit from USC1
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: tx1_index
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void USCI_A1_transmit(void) {
    tx1_index = BEGINNING;
    UCA1IE |= UCTXIE;
}

//===========================================================================
// Function name: loadRingtoPB
//
// Description: generic function to load from ring to process buffer
//
// Passed : rx_wr,rx_rd,Rx_Process,Rx_Ring,pb_index,pb_buffered
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void loadRingtoPB(volatile unsigned int* rx_wr, unsigned int* rx_rd, volatile char* Rx_Process, volatile char* Rx_Ring, volatile unsigned int* pb_index, volatile unsigned int* pb_buffered) {
    if(*pb_buffered) return;

    if(*rx_wr != *rx_rd) {
        Rx_Process[*pb_index] = Rx_Ring[*rx_rd];

        if(++(*rx_rd) > SMALL_RING_SIZE - 1) *rx_rd = BEGINNING;

        if(++(*pb_index) > LARGE_RING_SIZE - 1) *pb_index = BEGINNING;
    }

    if(/**pb_index >= 2 && */Rx_Process[(*pb_index) - 1] == '\n' && Rx_Process[(*pb_index) - 2] == '\r') {
        *pb_buffered = true;
        *pb_index = BEGINNING;
    }
}

//===========================================================================
// Function name: loadRingtoPB_0
//
// Description: This function is used to load from ring to process for USC0
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: usb0_rx_rw,usb0_rx_rd,USB0_Char_Rx_Process, USB0_Rx_Ring, 
// pb0_index, pb0_buffered
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void loadRingtoPB_0(void) {
    loadRingtoPB(&usb0_rx_wr, &usb0_rx_rd, USB0_Char_Rx_Process, USB0_Char_Rx_Ring, &pb0_index, &pb0_buffered);
}

//===========================================================================
// Function name: loadRingtoPB_1
//
// Description: This function is used to load from ring to process for USC1
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: usb1_rx_rw,usb1_rx_rd,USB1_Char_Rx_Process, USB1_Rx_Ring, 
// pb1_index, pb1_buffered
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================

void loadRingtoPB_1(void) {
    loadRingtoPB(&usb1_rx_wr, &usb1_rx_rd, USB1_Char_Rx_Process, USB1_Char_Rx_Ring, &pb1_index, &pb1_buffered);
}

//===========================================================================
// Function name: SerialProcess
//
// Description: This function runs all the processes necessarity for 
// serial communications
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no globals used
//
// Author: Ibrahim Moghul
// Date: Apr 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================


void SerialProcess(void) {
    /*if(!pb0_buffered)*/loadRingtoPB_0();

    /*if(!pb1_buffered)*/loadRingtoPB_1();

}