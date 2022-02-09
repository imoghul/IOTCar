// general
#define WHEEL_TICK (20)
// straight
#define STRAIGHT_RIGHT (21)
#define STRAIGHT_LEFT (18)
// circle
#define LCIRC_RIGHT (5)
#define LCIRC_LEFT (21)
#define RCIRC_RIGHT (21)
#define RCIRC_LEFT (5)
#define MAX_RCIRCLE_TICK (6000)  // without bearing (5500)
#define MAX_LCIRCLE_TICK (5300)  // without bearing (5500)
// triangle
#define TRIANGLE_LEG    (1000)
#define TRIANGLE_TURN_TICK      (1250)
#define TRIANGLE_LEFT_TICK      (0)
#define TRIANGLE_RIGHT_TICK     (RCIRC_RIGHT)


// states
#define START           ('S')
#define WAIT            ('W')
#define END             ('E')
#define ARM             ('A')
#define CIRCLE          ('C')
#define FIGURE8         ('F')
#define TRIANGLE        ('T')

void RunMotor(int, volatile unsigned int*, int, int);
void Drive_Straight(int);
void Left_Circle(int);
void Right_Circle(int);
int Update_Ticks(volatile unsigned int*, int, char);
int Drive_Path(int right_ticks, int left_ticks, int max_ticks, char endState);
void delay(int seconds,int cycles);
void StateMachine(void);
void Figure8(void);
void Circle(void);
void Triangle(void);