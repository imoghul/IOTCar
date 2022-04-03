#define BEGINNING       (0)
#define SMALL_RING_SIZE (16)
#define LARGE_RING_SIZE (25)

void Init_Serial_UCA(void);
void out_character(char character);

void USCI_A0_transmit(void);
void USCI_A1_transmit(void);
void clearProcessBuff(volatile char* pb,volatile unsigned int* pb_index,volatile unsigned int* pb_buffered);
void loadRingtoPB(volatile unsigned int* rx_wr,unsigned int* rx_rd,volatile char* Rx_Process,volatile char* Rx_Ring,volatile unsigned int* pb_index,volatile unsigned int* pb_buffered);
void copyPBtoTx_0(void);
void loadRingtoPB_0(void);
void loadRingtoPB_1(void);
void clearProcessBuff_0(void);
void clearProcessBuff_1(void);

void SerialProcess(void);