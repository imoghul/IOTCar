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

void StateMachine(void);
void Straight(char);
void Turn(char);
void LineFollow();
void Exit(int);
int delay(int seconds, int cycles);