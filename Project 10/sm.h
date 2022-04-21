// states
#define CALIBRATE       ('C')
#define START           ('S')
#define WAIT            ('W')
#define END             ('E')
#define ARM             ('A')
#define TURN            ('T')
#define STRAIGHT        ('s')
#define LINEFOLLOW      ('L')
#define DRIVE           ('D')
#define EXIT            ('e')
#define DONE            ('d')

#define LF_TURN_DECREMENT       (2500)
#define LEG1                    (2500)
#define LEG2                    (5000)
#define LEG3                    (1800)
#define PRELIMINARY_TURN        (100)
#define EXITING_TIME            (4000)
#define CIRCLING_TIME           (120)
#define TIME_TO_CIRCLE          (10)

void StateMachine(void);
void Straight(char);
void Turn(char);
void LineFollow(char);
void Exit(int);
int delay(int seconds, int cycles);
int Drive(int r, int l, unsigned int time);