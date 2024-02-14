#include "318code.h"


#define ADC_THRESHOLD        1000    // Threshold value to detect joystick movement
volatile unsigned int adcValue = 0;

// ADC interrupt service routine

   #pragma vector=ADC_VECTOR
   __interrupt void ADC_ISR(void)
   {
       switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
       {
           // ADC conversion complete
           case ADCIV_ADCIFG:
           adcValue = ADCMEM0;                      // Store ADC value
           break;
       }
   }



void main(void)
{
   //setzeroposition();    // start at 0 degrees
     setupadc();            // Initilaise ADC
    configureMotor();

    while(1)
        {
            ADCCTL0 |= ADCENC + ADCSC;             // Sampling and conversion start
            while(ADCCTL1 & ADCBUSY);           // Wait until conversion is done

            unsigned int adcValue = ADCMEM0;       // Read ADC value

            if (adcValue < ADC_THRESHOLD)           // Joystick not moved
            {
                SetServoPosition(500);  // Keep servo at left position
            }
            else if (adcValue > 1023 - ADC_THRESHOLD)
            {
                SetServoPosition(2500); // Keep servo at right position
            }
            else
            {
                // Joystick should do nothing but wait till adcValue changes
            }
        }


}







   /* while(1)
    {
        // aim to continuously monitor and update servo position with joystick

        if(adc_value<(ADC_THRESHOLD -JOYSTICK_DEADZONE))
        {
            //LEFT
            configureMotor(SERVO_MAX_VALUE);
            //TA0CCR1=SERVO_MAX_VALUE;
            joystick_active = 1; // Joystick is active
        }

        else if(adc_value>(ADC_THRESHOLD + JOYSTICK_DEADZONE))
        {
            //RIGHT
            configureMotor(SERVO_MIN_VALUE);

           // TA0CCR1= SERVO_MIN_VALUE;
            joystick_active = 1; // Joystick is active

        }
        else
        {
            if((joystick_active)==0)
            {
                //go to zero position

                 setzeroposition();
                 joystick_active=0;
            }
        }
    }*/
//}






