//==============================================================================
// File Name : ports.c
//
// Description: This file contains the Initialization for all port pins
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: 7.21.1
//==============================================================================
#include "ports.h"
#include "msp430.h"

//===========================================================================
// Function name: Init_Ports
//
// Description: Initialize all ports
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Ports(void) {
    Init_Port1();
    Init_Port2();
    Init_Port3(USE_GPIO);
    Init_Port4();
    Init_Port5();
    Init_Port6();
}

//===========================================================================
// Function name: Init_Port1
//
// Description: Initialize all pins on port 1
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port1() {
    P1OUT = 0x00;
    P1DIR = 0x00;

    P1SEL0 &= ~RED_LED;
    P1SEL1 &= ~RED_LED;
    P1DIR |= RED_LED;
    P1OUT &= ~RED_LED;

    //P1SEL0 |= A1_SEEED;
    //P1SEL1 |= A1_SEEED;
    P1SELC |= A1_SEEED;

    //P1SEL0 |= V_DETECT_L;
    //P1SEL1 |= V_DETECT_L;
    P1SELC |= V_DETECT_L;

    //P1SEL0 |= V_DETECT_R;
    //P1SEL1 |= V_DETECT_R;
    P1SELC |= V_DETECT_R;

    P1SEL0 &= ~SMCLK_2476;
    P1SEL1 &= ~SMCLK_2476;
    P1DIR &= ~SMCLK_2476;

    P1SEL0 |= V_THUMB;
    P1SEL1 |= V_THUMB;

    P1SEL0 &= ~UCA0RXD;
    P1SEL0 |= UCA0RXD;

    P1SEL0 &= ~UCA0TXD;
    P1SEL0 |= UCA0TXD;
}

//===========================================================================
// Function name: Init_Port2
//
// Description: Initialize all pins on port 2
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port2() {
    P2OUT = 0x00;
    P2DIR = 0x00;

    P2SEL0 &= ~RESET_LCD;
    P2SEL1 &= ~RESET_LCD;
    P2OUT &= ~RESET_LCD;
    P2DIR |= RESET_LCD;

    P2SEL0 &= ~L_REVERSE_2476;
    P2SEL1 &= ~L_REVERSE_2476;
    P2DIR &= ~L_REVERSE_2476;

    P2SEL0 &= ~P2_2;
    P2SEL1 &= ~P2_2;
    P2DIR &= ~P2_2;

    P2SEL0 &= ~SW2;
    P2SEL1 &= ~SW2;
    P2OUT |= SW2;
    P2DIR &= ~SW2;
    P2REN |= SW2;
    P2IES |= SW2;
    P2IFG &= ~SW2;
    P2IE |= SW2;

    P2SEL0 &= ~IOT_RUN_CPU;
    P2SEL1 &= ~IOT_RUN_CPU;
    P2OUT &= ~IOT_RUN_CPU;
    P2DIR |= IOT_RUN_CPU;

    P2SEL0 &= ~DAC_ENB;
    P2SEL1 &= -DAC_ENB;
    P2OUT |= DAC_ENB;
    P2DIR |= DAC_ENB;

    P2SEL0 &= ~LFXOUT;
    P2SEL1 |= LFXOUT;

    P2SEL0 &= ~LFXIN;
    P2SEL1 |= LFXIN;
}

//===========================================================================
// Function name: Init_Port3
//
// Description: Initialize all pins on port 3
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port3(char smclk) {
    P3OUT = 0x00;
    P3DIR = 0x00;

    P3SEL0 &= ~TEST_PROBE;
    P3SEL1 &= ~TEST_PROBE;
    P3DIR &= ~TEST_PROBE;

    P3SEL0 &= ~DAC_CNTL1;
    P3SEL1 &= ~DAC_CNTL1;
    P3DIR &= ~DAC_CNTL1;

    P3SEL0 |= OA2N;
    P3SEL1 |= OA2N;

    P3SEL0 &= ~LCD_BACKLITE;
    P3SEL1 &= ~LCD_BACKLITE;
    P3DIR |= LCD_BACKLITE;
    P3OUT |= LCD_BACKLITE;

    switch(smclk) {
        case(USE_SMCLK):
            P3SEL0 |= SMCLK_2355;
            P3SEL1 &= ~SMCLK_2355;
            P3DIR |= SMCLK_2355;
            break;

        case(USE_GPIO):
            P3SEL0 &= ~SMCLK_2355;
            P3SEL1 &= ~SMCLK_2355;
            P3DIR &= ~SMCLK_2355;
            break;

        default:
            break;
    }

    P3SEL0 &= ~DAC_CNTL;
    P3SEL1 &= ~DAC_CNTL;
    P3DIR &= ~DAC_CNTL;

    P3SEL0 &= ~IOT_LINK_CPU;
    P3SEL1 &= ~IOT_LINK_CPU;
    P3DIR &= ~IOT_LINK_CPU;
    //P3OUT &= ~IOT_LINK_CPU;

    P3SEL0 &= ~IOT_EN_CPU;
    P3SEL1 &= ~IOT_EN_CPU;
    P3DIR &= ~IOT_EN_CPU;
    //P3OUT &= ~IOT_EN_CPU;
}

//===========================================================================
// Function name: Init_Port4
//
// Description: Initialize all pins on port 4
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port4() {
    P4OUT = 0x00;
    P4DIR = 0x00;

    P4SEL0 &= ~_4_0;
    P4SEL1 &= ~_4_0;
    P4DIR &= ~_4_0;

    P4SEL0 &= ~SW1;
    P4SEL1 &= ~SW1;
    P4OUT |= SW1;
    P4DIR &= ~SW1;
    P4REN |= SW1;
    P4IES |= SW1;
    P4IFG &= ~SW1;
    P4IE |= SW1;

    P4SEL0 |= UCA1TXD;
    P4SEL1 &= ~UCA1TXD;

    P4SEL0 |= UCA1RXD;
    P4SEL1 &= UCA1RXD;

    P4SEL0 &= ~UCB1_CS_LCD;
    P4SEL1 &= ~UCB1_CS_LCD;
    P4OUT |= UCB1_CS_LCD;
    P4DIR |= UCB1_CS_LCD;

    P4SEL0 |= UCB1CLK;
    P4SEL1 &= ~UCB1CLK;

    P4SEL0 |= UCB1SIMO;
    P4SEL1 &= ~UCB1SIMO;

    P4SEL0 |= UCB1SOMI;
    P4SEL1 &= UCB1SOMI;
}


//===========================================================================
// Function name: Init_Port5
//
// Description: Initialize all pins on port 5
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port5() {
    P5OUT = 0x00;
    P5DIR = 0x00;

    P5SEL0 &= ~CHECK_BAT;
    P5SEL1 &= ~CHECK_BAT;
    P5DIR &= ~CHECK_BAT;

    //P5SEL0 |= V_BAT;
    //P5SEL1 |= V_BAT;
    P5SELC |= V_BAT;
    //P5DIR &= ~V_BAT;

    //P5SEL0 |= V_DAC;
    //P5SEL1 |= V_DAC;
    P5SELC |= V_DAC;
    //P5DIR &= ~V_DAC;

    //P5SEL0 |= V_3_3;
    //P5SEL1 |= V_3_3;
    P5SELC |= V_3_3;
    //P5DIR &= ~V_3_3;

    P5SEL0 &= ~IOT_BOOT_CPU;
    P5SEL1 &= ~IOT_BOOT_CPU;
    P5DIR &= ~IOT_BOOT_CPU;
}

//===========================================================================
// Function name: Init_Port6
//
// Description: Initialize all pins on port 6
//
// Passed : no variables passed
// Locals: no variables declared
// Returned: no values returned
// Globals: no global variables used
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: (7.21.1)
//===========================================================================
void Init_Port6() {
    P6OUT = 0x00;
    P6DIR = 0x00;

    P6SEL0 |= R_FORWARD;
    P6SEL1 &= ~R_FORWARD;
    P6DIR |= R_FORWARD;
    //P6OUT &= ~R_FORWARD;

    P6SEL0 |= L_FORWARD;
    P6SEL1 &= ~L_FORWARD;
    P6DIR |= L_FORWARD;
    //P6OUT &= ~L_FORWARD;

    P6SEL0 |= R_REVERSE;
    P6SEL1 &= ~R_REVERSE;
    P6DIR |= R_REVERSE;
    //P6OUT &= ~R_REVERSE;

    P6SEL0 |= L_REVERSE_2355;
    P6SEL1 &= ~L_REVERSE_2355;
    P6DIR |= L_REVERSE_2355;
    //P6OUT &= ~L_REVERSE_2355;

    P6SEL0 &= ~IR_LED;
    P6SEL1 &= ~IR_LED;
    P6DIR |= IR_LED;
    P6OUT |= IR_LED;

    P6SEL0 &= ~P6_5;
    P6SEL1 &= ~P6_5;
    P6DIR &= ~P6_5;

    P6SEL0 &= ~GRN_LED;
    P6SEL1 &= ~GRN_LED;
    P6DIR |= GRN_LED;
    P6OUT &= ~GRN_LED;
}