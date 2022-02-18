#define TB0CCR0_INTERVAL        (1000)          // 8e6/2/8/(1/5e-3)
#define TB0CCR1_INTERVAL        (50000)        // 8e6/2/8/(1/800e-3)
#define TB0CCR2_INTERVAL        (25000)          // 8e6/2/8/(1/50e-3)
#define TIME_SEQUENCE_MAX       (250)
#define TIME_SEQUENCE_TIMER_COUNT     (8)
#define UPDATE_DISPLAY_TIMER_COUNT     (100)
void Init_Timers(void);
void Init_Timer_B0(void);