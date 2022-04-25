// general
#define WHEEL_TICK              (20)
#define RIGHT_FORWARD_SPEED     (TB3CCR1)
#define LEFT_FORWARD_SPEED      (TB3CCR2)
#define RIGHT_REVERSE_SPEED     (TB3CCR3)
#define LEFT_REVERSE_SPEED      (TB3CCR4)
#define WHEEL_OFF               (0)
#define WHEEL_PERIOD            (20000)
#define RIGHT_MAX               (20000)
#define LEFT_MAX                (20000)
#define RIGHT_MIN               (8500)
#define LEFT_MIN                (8500)
// straight
#define STRAIGHT_RIGHT          (18000)
#define STRAIGHT_LEFT           (20000)
// circle
#define LCIRC_RIGHT             (3000)
#define LCIRC_LEFT              (20000)
#define RCIRC_RIGHT             (20000)
#define RCIRC_LEFT              (3000)
#define MAX_RCIRCLE_TICK        (1285)//(75)
#define MAX_LCIRCLE_TICK        (1180)//(72)
// turn
#define TURN90                  (460)
// detectors
#define LEFT_BLACK_DETECT       (LBDetect)
#define RIGHT_BLACK_DETECT      (RBDetect)
#define LEFT_GRAY_DETECT        (LBDetect-5)
#define RIGHT_GRAY_DETECT       (RBDetect-5)
#define LEFT_WHITE_DETECT       (9)//(LWDetect+2)
#define RIGHT_WHITE_DETECT      (9)//(RWDetect+2)
#define MS_PER_TICK             (4)
#define LOCK_TIME               (150)

int RunRightMotor( int val);
int RunLeftMotor( int val);
int Update_Ticks(int);
//int Drive_Path_Definite(int speedR, int speedL, unsigned int ticksDuration) ;
//int Drive_Path_Indefinite(int speedR, int speedL);
int Drive_Path(int speedR, int speedL, unsigned int ticksDuration);
int delay(int seconds, int cycles);
void ShutoffMotors(void);
void ShutoffRight(void);
void ShutoffLeft(void);
void MotorSafety(void);
int LockMotors(int, int);
int LockMotorsTime(int, int, int);

