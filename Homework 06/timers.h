#define TB0CCR0_INTERVAL        (2000)          // 4 ms
#define TB0CCR1_INTERVAL        (50000)         // 100 ms
#define TB0CCR2_INTERVAL        (50000)         // 100 ms
#define TIME_SEQUENCE_MAX       (250)
#define TIME_SEQUENCE_TIMER_COUNT     (1)
#define UPDATE_DISPLAY_TIMER_COUNT     (50)
#define BACKLITE_TIMER_COUNT            (12)
void Init_Timers(void);
void Init_Timer_B0(void);
void Init_Timer_B3(void);