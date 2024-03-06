
#include <msp430.h>
#include <driverlib.h>

void configureMotor();

void setzeroposition();

void setupadc();

void SetServoPosition(int x);

long map(long x, long in_min, long in_max, long out_min, long out_max);

void joystickRead(unsigned int *xAxis);

int analogRead();
//
