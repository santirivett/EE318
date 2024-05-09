//note: the following file was expanded from the code given in the EE312 lab notes


/**************************************************
 * ADC.c
 * Driver for ADC dial in middle of daughter board
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/

#include "liamsetup.h"
#include "hal_LCD.h"
#include <time.h>
#include <stdlib.h>

//volatile unsigned int timer = 100; // time left for game


volatile unsigned int adcValue = 0;


//taken from semester notes, maps ADC value to a servo motor position
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//ADC initialisation - taken from semester notes
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

//function to set up the motor
void configureMotor ()
{
     WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

     //initialisation the PWM P1.7 pin
     GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN7);        // PWM pin on MSP430
     P1SEL0 |= BIT7;

     TA0CCR0=20000;     //PWM 20ms
     TA0CCR1=1500;      //duty cycle of 1.5ms
     TA0CCTL1 |= OUTMOD_7;  //timer from the user guide
     TA0CTL = TASSEL_2 + MC_1;

}

//takes ADC value to convert it into a servo motor position
void SetServoPosition(int x)
{
    //formula derived from trial and error
    int duty_cycle = (x * 1000 / 180) + 1000;
    TA0CCR1=duty_cycle;
    __delay_cycles(200000000);
}

//converts analogue signal
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


//alternate way to set up ADC
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

void displayLCD(int score) {

    showChar(((score / 1000) % 10) + 48,pos1);
    showChar(((score / 100) % 10) + 48,pos2);
    showChar(((score / 10) % 10) + 48,pos3);
    showChar(((score / 1) % 10) + 48,pos4);
    showChar('K',pos5);
    showChar('W',pos6);

}

//function to reset LED - part of LED arc sub component
void reset_LED() {
    P1DIR &= ~BIT1;
    P1OUT &= ~BIT1;

    P1DIR &= ~BIT0;
    P1OUT &= ~BIT0;

    P2DIR &= ~BIT7;
    P2OUT &= ~BIT7;

    P5DIR &= ~BIT1;
    P5OUT &= ~BIT1;

    P2DIR &= ~BIT5;
    P2OUT &= ~BIT5;
}

//rnadom number generator to make LEDs light in a random way
int ran_num_gen() {
    srand(time(NULL));   // Initialization, should only be called once.
    int r = 1 + (rand() % 5);      // Returns a random integer between 6 and 1

    return r;
}

//chooses which LED lights depending on random numbers generated
void LED_sel(int r) {

    //initialising pins
    reset_LED();

    //resetting pins - ensuring that they are off before lighting an LED (we only want 1 on at once)

    if (r == 1) {
        P1DIR |= BIT1;              //setting p1.4
        P1OUT |= BIT1;              //setting p1.4
    }

    if (r == 2) {
        P1DIR |= BIT0;              //setting p1.4
        P1OUT |= BIT0;              //setting p1.4
    }

    if (r == 3) {
        P2DIR |= BIT7;              //setting p1.5
        P2OUT |= BIT7;              //setting p1.5
    }

    if (r == 4) {
        P5DIR |= BIT1;              //setting p1.6
        P5OUT |= BIT1;              //setting p1.6
    }

    if (r == 5) {
        P2DIR |= BIT5;              //setting p5.0
        P2OUT |= BIT5;              //setting p5.0
    }
}


//code that executes the LED arc process
void LED_ARC()
{


    while(1)
    {

            int x = ran_num_gen();
            LED_sel(x);

           //add delay cycles - this controls how long the LED will light for
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);
            __delay_cycles(500);



        }



}








