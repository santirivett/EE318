/**************************************************
 * ADC.c
 * Driver for ADC dial in middle of daughter board
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/

#include "liamsetup.h"

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

}








void SetServoPosition(int x)
{
    int duty_cycle = (x * 1000 / 180) + 1000;
    TA0CCR1=duty_cycle;
    __delay_cycles(200000000);
}

int analogRead()
{
    ADCCTL0 |= ADCENC | ADCSC; // Start conversion
    while (ADCCTL1 & ADCBUSY); // Wait for conversion to complete
    return ADCMEM0; // Return conversion result
}


// TimerA UpMode Configuration Parameter
Timer_A_initUpModeParam initUpParam_A1 =
{
    TIMER_A_CLOCKSOURCE_ACLK,               // ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          // ACLK/1 = 32768Hz
    0x2000,                                 // Timer period
    TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
    TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE ,   // Disable CCR0 interrupt
    TIMER_A_DO_CLEAR                        // Clear value
};

Timer_A_initCompareModeParam initCompParam =
{
    TIMER_A_CAPTURECOMPARE_REGISTER_1,        // Compare register 1
    TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE, // Disable Compare interrupt
    TIMER_A_OUTPUTMODE_RESET_SET,             // Timer output mode 7
    0x1000                                    // Compare value
};

/**
Initialise Using library
Using Library functions
*/
void initialiseADC_Easy()
{

     GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P8, GPIO_PIN1, GPIO_PRIMARY_MODULE_FUNCTION);

      //Initialize the ADC Module
    /*
     * Base Address for the ADC Module
     * Use Timer trigger 1 as sample/hold signal to start conversion
     * USE MODOSC 5MHZ Digital Oscillator as clock source
     * Use default clock divider of 1
     */
    ADC_init(ADC_BASE,
        ADC_SAMPLEHOLDSOURCE_2,
        ADC_CLOCKSOURCE_ADCOSC,
        ADC_CLOCKDIVIDER_1);

    ADC_enable(ADC_BASE);

    ADC_clearInterrupt(ADC_BASE,
            ADC_COMPLETED_INTERRUPT);


    ADC_enableInterrupt(ADC_BASE,
            ADC_COMPLETED_INTERRUPT);

    //Configure Memory Buffer
    /*
     * Base Address for the ADC Module
     * Use input A9 POT
     * Use positive reference of AVcc
     * Use negative reference of AVss
     */
    ADC_configureMemory(ADC_BASE,
        ADC_INPUT_A9,
        ADC_VREFPOS_AVCC,
        ADC_VREFNEG_AVSS);

    //Start a single measurement
    ADC_startConversion(ADC_BASE,
                        ADC_REPEATED_SINGLECHANNEL);


    // TimerA1.1 (125ms ON-period) - ADC conversion trigger signal
    Timer_A_initUpMode(TIMER_A1_BASE, &initUpParam_A1);

    //Initialize compare mode to generate PWM1
    Timer_A_initCompareMode(TIMER_A1_BASE, &initCompParam);

    // Start timer A1 in up mode
    Timer_A_startCounter(TIMER_A1_BASE,
        TIMER_A_UP_MODE
        );

    // Delay for reference settling
    __delay_cycles(300000);
}

/**
Initialise ADC
Directly using registers
*/
void initialiseADC_Advanced()
{


  SYSCFG2 |= ADCPCTL9;              // Configure ADC A9 pin

  // Configure ADC10
  ADCCTL0 |= ADCSHT_2 | ADCON;      // ADCON, S&H=16 ADC clks
  ADCCTL1 |= ADCSHP;                // ADCCLK = MODOSC; sampling timer
  ADCCTL2 |= ADCRES;                // 10-bit conversion results
  ADCMCTL0 |= ADCINCH_9;            // A9 ADC input select; Vref=AVCC

  ADCIFG &= ~0x01;  //Clear interrupt flag

  ADCIE |= ADCIE0;                  // Enable ADC conv complete interrupt
}






