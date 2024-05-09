#include <msp430.h>
#include "hal_LCD.h"
#include "liamsetup.h"
#include "components.h"

//declaring global variables
volatile unsigned int adcValue3 = 0; // To store ADC result from P1.3
volatile unsigned int adcValue4 = 0; // To store ADC result from P1.4
volatile unsigned int adcValue5 = 0; // To store ADC result from P1.5
volatile unsigned int adcValue8 = 0; // To store ADC result from P8.0
volatile unsigned int adcValue9 = 0; // To store ADC result from P8.1

volatile unsigned int timer = 250; // To store ADC result from P8.1
volatile unsigned int final_score=0;


//creating variable to keep track of channel process
int count = 0;

//creating variable to track current channel
unsigned int channel = 0;

//creating variable to assign ADC input channel
volatile unsigned char currentChannel = ADCINCH_3; // Start with channel A3 (P1.3)


void configureADC(void)
{
  //begin setup
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
  P1SEL0 |= BIT4 | BIT5 | BIT6 | BIT7; // set ADC pins
  ADCCTL0 = ADCSHT_2 | ADCON; // ADC ON, sampling time
  ADCCTL1 = ADCSHP; // Use sampling timer
  ADCCTL2 = ADCRES; // enable 10-bit conversion results
  ADCIE = ADCIE0; // Enable ADC conversion complete interrupt
  ADCMCTL0 = ADCINCH_4 | ADCSREF_0; // set A4 (P1.4) as start, internal ref voltage 1.5V
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode

  __bis_SR_register(GIE); // Enable global interrupts
}

//ADC interrupt vector
#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    while (timer>0)
    {
  if (currentChannel == ADCINCH_3)
  {
    channel = 3;
    adcValue3 = ADCMEM0;        //save result from A3

    // Read joystick input
    int servoVal = analogRead();

    // Map joystick input to servo position
    servoVal = map(servoVal, 0, 1023, -90, 300);

    // Set the servo position
    TA0CCR1 = (servoVal * 1000 / 180) + 1000;

    // Wait for the servo to get there
    __delay_cycles(200000);

    if (count == 0) {
        currentChannel = ADCINCH_4; // Switch to next channel (A4)
    }

    if (count == 1) {
            currentChannel = ADCINCH_5; // Switch to next channel (A4)
        }

    if (count == 2) {
            currentChannel = ADCINCH_8; // Switch to next channel (A4)
        }

    if (count == 3) {
            currentChannel = ADCINCH_9; // Switch to next channel (A4)
        }



  }

  else if (currentChannel == ADCINCH_4)
  {
    channel = 4;
    adcValue4 = ADCMEM0; // Save the result from A4
    count++;
    currentChannel = ADCINCH_3; // Switch to next channel (A5)


  }

  else if (currentChannel == ADCINCH_5)
  {
    channel = 5;
    adcValue5 = ADCMEM0; // Save the result from A6
    count++;
    currentChannel = ADCINCH_3; // Switch to next channel (A8)
  }


  else if (currentChannel == ADCINCH_8)
  {
    channel = 8;
    adcValue8 = ADCMEM0; // Save the result from A8
    count++;
    currentChannel = ADCINCH_3; // Switch to next channel (A9)
  }

  else if (currentChannel == ADCINCH_9)
  {
    channel = 9;
    adcValue9 = ADCMEM0; // Save the result from A8
    count = 0;
    currentChannel = ADCINCH_3; // Switch back to first channel (A3)
  }

  ADCCTL0 &= ~ADCENC; // Allow configuration changes by disabling the ADC
  ADCMCTL0 = (currentChannel | ADCSREF_0); // Update channel and reference

  // Re-enable the ADC and start the next conversion
  ADCCTL0 |= ADCENC | ADCSC;

  int x = ran_num_gen();
  LED_sel(x);

 //add delay cycles - this controls how long the LED will light for
  __delay_cycles(20000);
  __delay_cycles(20000);

  showChar(((adcValue8 / 1000) % 10) + 48,pos1);
  showChar(((adcValue8 / 100) % 10) + 48,pos2);
  showChar(((adcValue8 / 10) % 10) + 48,pos3);
  showChar(((adcValue8 / 1) % 10) + 48,pos4);
  showChar('K',pos5);
  showChar('W',pos6);
  final_score+=adcValue8;
  timer--;
    }

    displayScrollText("GAME OVER");
    displayScrollText("FINAL SCORE");

    while (1) {
        showChar(((final_score / 1000) % 10) + 48,pos1);
        showChar(((final_score / 100) % 10) + 48,pos2);
        showChar(((final_score / 10) % 10) + 48,pos3);
        showChar(((final_score / 1) % 10) + 48,pos4);

    }



}


int main(void)
{
     WDTCTL = WDTPW | WDTHOLD;                                // Stop WDT

    //initialising components
    Init_LCD();                                              //using predifined functions from library
    //initialiseADC_Easy();
    configureADC();
    configureMotor();      // Initialise servo motor control


    // Configure GPIO
//    P1DIR |= BIT0;                                           // Set P1.0/LED to output direction
//    P1OUT &= ~BIT0;                                          // P1.0 LED off
//
//    P4DIR |= BIT0;
//    P4OUT &= ~BIT0;

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


    displayScrollText("MINI GRID GAME");

    LCD_E_selectDisplayMemory(LCD_E_BASE, LCD_E_DISPLAYSOURCE_MEMORY);

    while(1)
    {

        ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start






    }

}
