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

#define LF_TURN_DECREMENT       (3000)
#define LEG1                    (2000)
#define LEG2                    (2000)        
#define PRELIMINARY_TURN        (100)
#define CIRCLING_TIME           (70)
#define TIME_TO_CIRCLE          (3)

void StateMachine(void);
void Straight(char);
void Turn(char);
void LineFollow(char);
void Exit(int);
int delay(int seconds, int cycles);