#include "318code.h"

volatile unsigned int adcValue = 0;

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void setupadc()
{
    //Step 1: Pin  Select P8.1 for ADC function

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

     P1SEL0 |= BIT1;

   //Step 2: Configure CLK source

    ADCCTL0 |= ADCSHT_2 | ADCON;                // Sample and hold time, ADC on, 16 ADCCLK cycles
    ADCCTL1 |= ADCSSEL_2|ADCSHP| ADCCONSEQ_0;  // Clock source: SMCLK, Clock divider: 1, Pulse-mode
    ADCCTL2 |= ADCRES;                         // Resolution: 10-bit
    ADCMCTL0 |= ADCINCH_9;                     // Select A9 for ADC input channel

    // Enable ADC interrupt

    //ADCIE |= ADCIE0;                         // Enable ADC interrupt for channel 0
  // __enable_interrupt();     // Enable global interrupts
}



// ADC ISR
#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch (__even_in_range(ADCIV, ADCIV_ADCIFG))
    {
        case ADCIV_ADCIFG:                    // ADC conversion completed
            adcValue = ADCMEM0;               // Store ADC value
            break;
        default:
            break;
    }
}


void configureMotor ()
{
     WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

     GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);        // PWM pin on MSP430
     P1SEL0 |= BIT7;

     TA0CCR0=20000;     //PWM 20ms
     TA0CCR1=1500;      //duty cycle of 1.5ms
     TA0CCTL1 |= OUTMOD_7;
     TA0CTL = TASSEL_2 + MC_1;
    // __delay_cycles(2000000); // Delay to reach position


//Code below. 180 rotation

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
    TA0CCR1=2500;
    TA0CCTL1 |= OUTMOD_7;
    TA0CTL = TASSEL_2 + MC_1;

}




void SetServoPosition(int x)
{
    int duty_cycle = (x * 1000 / 180) + 1000;
    TA0CCR1=duty_cycle;
}

int analogRead()
{
    ADCCTL0 |= ADCENC | ADCSC; // Start conversion
    while (ADCCTL1 & ADCBUSY); // Wait for conversion to complete
    return ADCMEM0; // Return conversion result
}




