#define MOVING_RIGHT    ('R')
#define MOVING_LEFT     ('L')
#define MOVING_STRAIGHT ('S')        
#define NOT_MOVING      ('N')
#define VALUES_TO_HOLD  (4)
#define INCREASING      ('I')
#define DECREASING      ('D')
#define NEUTRAL         ('n')
void EmitterOn(void);
void EmitterOff(void);
void DetectMovement(void);
void push( int list[], int val);
void clearList(int list[]);
int rollingSum(int * list);
char getDirection(int* list);
int validList(int* list);
unsigned int abs(int);