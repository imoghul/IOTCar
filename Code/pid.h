#define SET_POINT        (20)
struct PID {
    int kP, kD;//, kI;
    int error, lastError;//, lastIntegral;
};

typedef struct PID PIDController;

int GetOutput(PIDController* pidController, int setPoint, int current);
void ClearController(PIDController* pidController);

int additionSafe( int pin,  int max,  int min, int increment);
int multSafe(int a, int b);
