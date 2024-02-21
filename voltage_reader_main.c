#include <msp430.h>
#include <driverlib.h>
#include "Uos_ADC.h"
#include "hal_LCD.h"
#include <math.h>

//trying to relay voltage measurement onto LCD

volatile unsigned int ADC_Result;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                                // Stop WDT

    Init_LCD();                                              //using predifined functions from library
    initialiseADC_Easy();

    // Configure GPIO
    P1DIR |= BIT0;                                           // Set P1.0/LED to output direction
    P1OUT &= ~BIT0;                                          // P1.0 LED off

    P4DIR |= BIT0;
    P4OUT &= ~BIT0;

    // Configure ADC A9 pin
    P1SEL0 |= BIT1;
    //P1SEL1 |= BIT1;

    //configure the A9 pin
    //SYSCFG2 |= ADCPCTL9;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Configure ADC10
    ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
    ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
    ADCCTL2 |= ADCRES;                                       // 10-bit conversion results
    ADCMCTL0 |= ADCINCH_9;                                   // A1 ADC input select; Vref=AVCC
   // ADCIE |= ADCIE0;                                       // Enable ADC conv complete interrupt


    displayScrollText("PLAYER 1");

    LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);

    while(1)
    {
        ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start

        while( (ADCIFG & ADCIFG0) == 0 );

        ADC_Result = ADCMEM0;


        //finding each order
        showChar(((ADC_Result / 1000) % 10) + 48,pos1);
        showChar(((ADC_Result / 100) % 10) + 48,pos2);
        showChar(((ADC_Result / 10) % 10) + 48,pos3);
        showChar(((ADC_Result / 1) % 10) + 48,pos4);
        showChar('K',pos5);
        showChar('W',pos6);
        //__delay_cycles(5000);
    }
}
