// general
#define WHEEL_TICK              (20)
#define RIGHT_FORWARD_SPEED     (TB3CCR1)
#define LEFT_FORWARD_SPEED      (TB3CCR2)
#define RIGHT_REVERSE_SPEED     (TB3CCR3)
#define LEFT_REVERSE_SPEED      (TB3CCR4)
#define WHEEL_OFF               (0)
#define WHEEL_PERIOD            (20000)
#define RIGHT_MAX               (10000)
#define LEFT_MAX                (10000)
#define RIGHT_MIN               (8500)
#define LEFT_MIN                (8500)
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
#define LEFT_BLACK_DETECT       (LBDetect>>1)
#define RIGHT_BLACK_DETECT      (RBDetect>>1)
#define LEFT_GRAY_DETECT        (LBDetect*3/10)
#define RIGHT_GRAY_DETECT       (RBDetect*3/10)
#define LEFT_WHITE_DETECT       (LWDetect+20)
#define RIGHT_WHITE_DETECT      (RWDetect+20)
#define MS_PER_TICK             (4)
#define LOCK_TIME               (25)

int RunRightMotor( int val);
int RunLeftMotor( int val);
int Update_Ticks(int);
int Drive_Path_Definite(int speedR, int speedL, unsigned int ticksDuration) ;
int Drive_Path_Indefinite(int speedR, int speedL);
int delay(int seconds, int cycles);
void ShutoffMotors(void);
void ShutoffRight(void);
void ShutoffLeft(void);
void MotorSafety(void);
int LockMotors(int, int);
int LockMotorsTime(int, int, int);

