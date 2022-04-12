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

#define LF_TURN_DECREMENT       (4000)

void StateMachine(void);
void Straight();
void Turn();
void LineFollow();
void Exit();
int delay(int seconds, int cycles);