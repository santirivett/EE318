#include "318code.h"


void setupadc()
{
    //Step 1: Pin  Select P8.1 for ADC function

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);
    P8SEL0 |= BIT1;

   //Step 2: Configure CLK source

    ADCCTL0 |= ADCSHT_2 | ADCON;               // Sample and hold time, ADC on
    ADCCTL1 |= ADCSSEL_2 | ADCDIV_0 | ADCSHP; // Clock source: SMCLK, Clock divider: 1, Pulse-mode
    ADCCTL2 |= ADCRES;                        // Resolution: 10-bit
    ADCMCTL0 |= ADCINCH_9;                    // Select A0 for ADC input channel

    // Enable ADC interrupt

    ADCIE |= ADCIE0;                         // Enable ADC interrupt for channel 0
   __enable_interrupt();                     // Enable global interrupts

}


void configureMotor ()
{
     WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

     GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);        // PWM pin on MSP430
     P1SEL0 |= BIT7;

     TA0CCR0=20000;
     TA0CCR1=1500;
     TA0CCTL1 |= OUTMOD_7;
     TA0CTL = TASSEL_2 + MC_1;
    // __delay_cycles(2000000); // Delay to reach position




     /*while(1)
         {
                 TA0CCR0=20000;  //PWM (20ms)
                  TA0CCR1=2500;     //duty cycle. Values differs to get different position //2500
                  TA0CCTL1 |= OUTMOD_7;
                  TA0CTL = TASSEL_2 + MC_1;


                  __delay_cycles(2000000); // Delay to reach position

                   TA0CCR1=500;                                                            //500

                   __delay_cycles(2000000);


         }
         */
}


void setzeroposition()
{
    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);        // PWM pin on MSP430
    P1DIR|=BIT7;
    P1SEL0 |= BIT7;


    TA0CCR0=20000;  //PWM (20ms)
    TA0CCR1=1000;
    TA0CCTL1 |= OUTMOD_7;
    TA0CTL = TASSEL_2 + MC_1;

}

SetServoPosition(int x)
{
    TA0CCR1=x;
}









