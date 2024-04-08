#include <msp430.h>


//declaring global variables
volatile unsigned int adcValue3 = 0; // To store ADC result from P1.3
volatile unsigned int adcValue4 = 0; // To store ADC result from P1.4
volatile unsigned int adcValue5 = 0; // To store ADC result from P1.5
volatile unsigned int adcValue8 = 0; // To store ADC result from P8.0
volatile unsigned int adcValue9 = 0; // To store ADC result from P8.1

//creating variable to track current channel
unsigned int channel = 0;

//creating variable to assign ADC input channel
volatile unsigned char currentChannel = ADCINCH_3; // Start with channel A3 (P1.3)


void configureADC(void)
{
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


#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
  if (currentChannel == ADCINCH_3)
  {
    channel = 3;
    adcValue3 = ADCMEM0;        //save result from A3
    currentChannel = ADCINCH_4; // Switch to next channel (A4)
  }

  else if (currentChannel == ADCINCH_4)
  {
    channel = 4;
    adcValue4 = ADCMEM0; // Save the result from A4
    currentChannel = ADCINCH_5; // Switch to next channel (A5)
  }


  else if (currentChannel == ADCINCH_5)
  {
    channel = 5;
    adcValue5 = ADCMEM0; // Save the result from A6
    currentChannel = ADCINCH_8; // Switch to next channel (A8)
  }


  else if (currentChannel == ADCINCH_8)
  {
    channel = 8;
    adcValue8 = ADCMEM0; // Save the result from A8
    currentChannel = ADCINCH_9; // Switch to next channel (A9)
  }

  else if (currentChannel == ADCINCH_9)
  {
    channel = 9;
    adcValue9 = ADCMEM0; // Save the result from A8
    currentChannel = ADCINCH_3; // Switch back to first channel (A3)
  }

  ADCCTL0 &= ~ADCENC; // Allow configuration changes by disabling the ADC
  ADCMCTL0 = (currentChannel | ADCSREF_0); // Update channel and reference

  // Re-enable the ADC and start the next conversion
  ADCCTL0 |= ADCENC | ADCSC;
}


int main(void)
{
  configureADC(); // Configure ADC and start conversion
  ADCCTL0 |= ADCENC | ADCSC; // Start the first conversion

  while (1)
  {

  }

}
