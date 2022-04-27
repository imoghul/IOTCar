#define MOVING_RIGHT    ('R')
#define MOVING_LEFT     ('L')
#define MOVING_STRAIGHT ('S')
#define NOT_MOVING      ('N')
#define VALUES_TO_HOLD  (4)
#define INCREASING      ('I')
#define DECREASING      ('D')
#define NEUTRAL         ('n')

#define EMITTER_ON             {P6OUT |= IR_LED;}
#define EMITTER_OFF            {P6OUT &= ~IR_LED;}

void calibrate(void);
void updateDetectors(void);