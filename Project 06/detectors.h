#define MOVING_RIGHT    ('R')
#define MOVING_LEFT     ('L')
#define MOVING_STRAIGHT ('S')
#define NOT_MOVING      ('N')
#define MEMORY_LEN      (100)
void EmitterOn(void);
void EmitterOff(void);
void DetectMovement(void);
void push( int list[], int val);
void clearList(int list[]);
int average(int * list);
int abs(int);