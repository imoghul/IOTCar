#define TB0CCR0_INTERVAL                (2000)          // 4 ms
#define TB0CCR1_INTERVAL                (50000)         // 100 ms
#define TB0CCR2_INTERVAL                (50000)         // 100 ms

#define TB1CCR0_INTERVAL                (50000)         // 200 ms
#define TB1CCR1_INTERVAL                (25000)         // 100 ms
#define TB1CCR2_INTERVAL                (25000)         // 100 ms

#define TIME_SEQUENCE_MAX               (250)
#define CHECK_ADC_TIMER_COUNT           (14)
#define TIMER_COUNT_MAX                 (701) // multiply the other ones and add 1
#define PING_COUNT_MAX                  (10)

void Init_Timers(void);
void Init_Timer_B0(void);
void Init_Timer_B1(void);
void Init_Timer_B3(void);