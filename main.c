#include "318code.h"

#define ADC_THRESHOLD        1000    // Threshold value to detect joystick movement
#define JOYSTICK_CENTER      512     // Center position of the joystick
#define SERVO_MIN            0       // Minimum servo position
#define SERVO_MAX            180     // Maximum servo position
#define SERVO_CENTER         90      // Center position for the servo

long joystickValue = 0;
long servoPosition =0;
void main(void)
{
    setupadc();            // Initialise ADC
    configureMotor();      // Initialise servo motor control
    PM5CTL0&=~LOCKLPM5;


    while (1)
    {
        // Read joystick input
        int servoVal = analogRead();

        // Map joystick input to servo position // adjust values???
        servoVal = map(servoVal, 0, 1023, 90, 180);

        // Set the servo position
        TA0CCR1 = (servoVal * 1000 / 180) + 1000;

        // Wait for the servo to get there
        __delay_cycles(20);
    }
}




/*
 * while(1)
    {
        // Read joystick input
        ADCCTL0 |= ADCENC | ADCSC;  //  ADC conversion
        while((ADCIFG & ADCIFG0)==0);    // Wait for conversion to complete

        joystickValue = ADCMEM0;

        // Map joystick input to servo position (0-180 degrees)
        servoPosition = map(joystickValue, 0, 1023, SERVO_MIN, SERVO_MAX);

        // Set servo position by utilising PWM duty cycles
        TA0CCR1 = (servoPosition * 1000 / 180) + 1000;

        __delay_cycles(20);
    }
 */

