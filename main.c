#include "318code.h"

long joystickValue = 0;
long servoPosition =0;
void main(void)
{
    setupadc();            // Initialise ADC
    configureMotor();      // Initialise servo motor control
    PM5CTL0&=~LOCKLPM5;


    //setzeroposition();


    while (1)
    {
        // Read joystick input
        int servoVal = analogRead();

        // Map joystick input to servo position
        servoVal = map(servoVal, 0, 1023, -90, 300);
       // -90
        // Set the servo position
        TA0CCR1 = (servoVal * 1000 / 180) + 1000;

        // Wait for the servo to get there
        __delay_cycles(20);
    }
}





