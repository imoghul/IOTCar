#define SETPOINT        (380)
struct PID{
  double kP,kD,kI;
  int error, lastError, lastIntegral;
};

typedef struct PID PIDController;

int GetOutput(PIDController* pidController, int setPoint, int current);