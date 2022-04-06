//==============================================================================
// File Name : ports.h
//
// Description: This file contains macros and function declarations for ports.c
//
// Author: Ibrahim Moghul
// Date: Feb 2022
// Compiler: Built with IAR Embedded Workbench Version: 7.21.1
//==============================================================================

#define USE_GPIO        (0)
#define USE_SMCLK       (1)

// Port 1 Pins
#define RED_LED         (0x01) // 0 RED LED 0
#define A1_SEEED        (0x02) // 1 A1_SEEED
#define V_DETECT_L      (0x04) // 2 V_DETECT_L
#define V_DETECT_R      (0x08) // 3 V_DETECT_R
#define SMCLK_2476      (0x10) // 4 SMCLK_2476
#define V_THUMB         (0x20) // 5 V_THUMB
#define UCA0RXD         (0x40) // 6 Back Channel UCA0RXD
#define UCA0TXD         (0x80) // 7 Back Channel UCA0TXD
// Port 2 Pins
#define RESET_LCD       (0x01) // 0 RESET LCD
#define L_REVERSE_2476  (0x02) // 1 L_REVERSE FOR MSP430FR2476
#define P2_2            (0x04) // 2
#define SW2             (0x08) // 3 SW2
#define IOT_RUN_CPU     (0x10) // 4 IOT_RUN_CPU
#define DAC_ENB         (0x20) // 5 DAC_ENB
#define LFXOUT          (0x40) // 6 XOUTR
#define LFXIN           (0x80) // 7 XINR
// Port 3 Pins
#define TEST_PROBE      (0x01) // 0 TEST PROBE
#define DAC_CNTL1       (0x02) // 1 DAC_CNTL1
#define OA2N            (0x04) // 2 Photodiode Circuit
#define LCD_BACKLITE    (0x08) // 3
#define SMCLK_2355      (0x10) // 4
#define DAC_CNTL        (0x20) // 5
#define IOT_LINK_CPU    (0x40) // 6
#define IOT_EN_CPU      (0x80) // 7
// Port 4 Pins
#define _4_0            (0x01) // 0
#define SW1             (0x02) // 1 SW1
#define UCA1RXD         (0x04) // 2 Back Channel UCA1RXD (0x08) // 3 Back Channel UCA1TXD (0x10) // 4 Chip Select
#define UCA1TXD         (0x08) // 3 Back Channel UCA1TXD
#define UCB1_CS_LCD     (0x10) // 4 Chip Select
#define UCB1CLK         (0x20) // 5 SPI mode -
#define UCB1SIMO        (0x40) // 6 UCB1SIMO
#define UCB1SOMI        (0x80) // 7 UCB1SOMI
// Port 5 Pins
#define CHECK_BAT       (0x01)
#define V_BAT           (0x02)
#define V_DAC           (0x04)
#define V_3_3           (0x08)
#define IOT_BOOT_CPU    (0x10)
// Port 6 Pins
#define R_FORWARD       (0x01) // 0
#define L_FORWARD       (0x02) // 1 
#define R_REVERSE       (0x04) // 2 
#define L_REVERSE_2355  (0x08) // 3 
#define IR_LED          (0x10) // 4 
#define P6_5            (0x20) // 5 
#define GRN_LED         (0x40) // 6 


void Init_Ports(void);
void Init_Port1(void);
void Init_Port2(void);
void Init_Port3(char smclk);
void Init_Port4(void);
void Init_Port5(void);
void Init_Port6(void);