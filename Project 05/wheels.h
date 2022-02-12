// general
#define WHEEL_TICK              (20)
// straight
#define STRAIGHT_RIGHT          (20)
#define STRAIGHT_LEFT           (16)
// circle
#define LCIRC_RIGHT             (3)
#define LCIRC_LEFT              (20)
#define RCIRC_RIGHT             (20)
#define RCIRC_LEFT              (3)
#define MAX_RCIRCLE_TICK        (75)  // without time_change (6000)
#define MAX_LCIRCLE_TICK        (72)  // without time_change (5300)
// triangle
#define TRIANGLE_LEG            (20)
#define TRIANGLE_TURN_TICK      (13)
#define TRIANGLE_LEFT_TICK      (0)
#define TRIANGLE_RIGHT_TICK     (RCIRC_RIGHT)
// forward
#define ONESEC_STRAIGHT         (10)
#define TWOSEC_STRAIGHT         (20)
// spin
#define SPIN_CK                 (1)
#define SPIN_CCK                (-1)
#define SPINR_TICKS             (30)
#define SPINL_TICKS             (30)
// states
#define START                   ('S')
#define WAIT                    ('W')
#define ARM                     ('A')
#define END                     ('E')
#define FORWARD1                ('F')
#define FORWARD2                ('f')
#define REVERSE                 ('R')
#define SPINCK                  ('P')
#define SPINCCK                 ('s')

void RunMotor(int, int, volatile unsigned int*, int, int);
int Drive_Straight(int, int);
int Update_Ticks(int);
int Drive_Path(int right_ticks, int left_ticks, int max_ticks, int polarityr, int polarityl/*, char endState*/);
void delay(int seconds,int cycles);
void StateMachine(void);
void ShutoffMotors(void);
void MotorSafety(void);
void Forward(int, int, const char *);
void Spin(int,int, const char * disp);