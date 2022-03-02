// general
#define WHEEL_TICK              (20)
#define RIGHT_FORWARD_SPEED     (TB3CCR1)
#define LEFT_FORWARD_SPEED      (TB3CCR2)
#define RIGHT_REVERSE_SPEED     (TB3CCR3)
#define LEFT_REVERSE_SPEED      (TB3CCR4)
#define WHEEL_OFF               (0)
#define WHEEL_PERIOD            (20000)
#define RIGHT_MAX               (STRAIGHT_RIGHT)
#define LEFT_MAX                (STRAIGHT_LEFT)
#define RIGHT_MIN               (5000)
#define LEFT_MIN                (4000)
// straight
#define STRAIGHT_RIGHT          (20000)
#define STRAIGHT_LEFT           (16000)
// circle
#define LCIRC_RIGHT             (3000) 
#define LCIRC_LEFT              (20000)
#define RCIRC_RIGHT             (20000)
#define RCIRC_LEFT              (3000) 
#define MAX_RCIRCLE_TICK        (1285)//(75)
#define MAX_LCIRCLE_TICK        (1180)//(72)
// triangle
#define TRIANGLE_LEG            (120)//(4)
#define TRIANGLE_TURN_TICK      (350)//(15)
#define TRIANGLE_LEFT_TICK      (0)
#define TRIANGLE_RIGHT_TICK     (RCIRC_RIGHT)
// detectors
#define LEFT_LINE_DETECT        (150)
#define RIGHT_LINE_DETECT       (150)
// states
#define START           ('S')
#define WAIT            ('W')
#define END             ('E')
#define ARM             ('A')
#define TURN            ('T')        
#define STRAIGHT        ('s')
#define LINEFOLLOW      ('L')

int RunRightMotor(unsigned int val, int);
int RunLeftMotor(unsigned int val,int);
int Update_Ticks(int);
int Drive_Path(unsigned int,unsigned int,int , int,unsigned int);
int delay(int seconds,int cycles);
void StateMachine(void);
void ShutoffMotors(void);
void ShutoffRight(void);
void ShutoffLeft(void);
void MotorSafety(void);
int LockMotors(int,int,int);