#define BEGINNING       (0)
#define SMALL_RING_SIZE (16)
#define LARGE_RING_SIZE (25)

void Init_Serial_UCA(void);
void out_character(char character);

void USCI_A0_transmit(void);
void USCI_A1_transmit(void);
void clearProcessBuff0();
void copyPBtoTx_0(void);
void loadRingtoPB_0(void);
