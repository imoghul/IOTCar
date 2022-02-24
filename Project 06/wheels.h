// general
#define WHEEL_TICK              (20)
#define RIGHT_FORWARD_SPEED     (TB3CCR1)
#define LEFT_FORWARD_SPEED      (TB3CCR2)
#define RIGHT_REVERSE_SPEED     (TB3CCR3)
#define LEFT_REVERSE_SPEED      (TB3CCR4)
#define WHEEL_OFF               (0)
#define WHEEL_PERIOD            (20000)
// straight
#define STRAIGHT_RIGHT          (20000)
#define STRAIGHT_LEFT           (16000)
// circle
#define LCIRC_RIGHT             (3000) // 3
#define LCIRC_LEFT              (20000)
#define RCIRC_RIGHT             (20000)
#define RCIRC_LEFT              (3000) // 3
#define MAX_RCIRCLE_TICK        (1285)//(75)
#define MAX_LCIRCLE_TICK        (1180)//(72)
// triangle
#define TRIANGLE_LEG            (120)//(4)
#define TRIANGLE_TURN_TICK      (350)//(15)
#define TRIANGLE_LEFT_TICK      (0)
#define TRIANGLE_RIGHT_TICK     (RCIRC_RIGHT)
// detectors
#define LEFT_LINE_DETECT        (200)
#define RIGHT_LINE_DETECT       (200)
// states
#define START           ('S')
#define WAIT            ('W')
#define END             ('E')
#define ARM             ('A')
#define RCIRC           ('R')
#define LCIRC           ('L')
#define STRAIGHT        ('s')

int RunRightMotor(int val);
int RunLeftMotor(int val);
int Update_Ticks(int);
int Drive_Path(int , int, int);
void delay(int seconds,int cycles);
void StateMachine(void);
void ShutoffMotors(void);
void ShutoffRight(void);
void ShutoffLeft(void);
void MotorSafety(void);