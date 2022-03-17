// states
#define CALIBRATE       ('C')
#define START           ('S')
#define WAIT            ('W')
#define END             ('E')
#define ARM             ('A')
#define TURN            ('T')
#define STRAIGHT        ('s')
#define LINEFOLLOW      ('L')
#define EXIT            ('e')

void StateMachine(void);
void Straight();
void Turn();
void LineFollow();
void Exit();
int delay(int seconds, int cycles);