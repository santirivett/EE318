#include <msp430.h>



// WORKING LDR CODE!!!

// Global pointers for ADC values

volatile unsigned int* ptrLDR1 = 0;
volatile unsigned int* ptrLDR2 = 0;
volatile unsigned int* ptrLDR3 = 0;
volatile unsigned int* ptrLDR4 = 0;
volatile unsigned int adcValue1 = 0; // To store ADC result from P1.4
volatile unsigned int adcValue2 = 0; // To store ADC result from P1.5
volatile unsigned int adcValue3 = 0; // To store ADC result from P1.6
volatile unsigned int adcValue4 = 0; // To store ADC result from P1.7
volatile unsigned int LDR1 = 0;
volatile unsigned int LDR2 = 0;
volatile unsigned int LDR3 = 0;
volatile unsigned int LDR4 = 0;

unsigned int channel = 0;
unsigned int threshold = 750;
unsigned int moveCheck = 5;

volatile unsigned char currentChannel = ADCINCH_4; // Start with channel A6 (P1.6)


void configureADC(void)

{
  WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer
  P1SEL0 |= BIT4 | BIT5 | BIT6 | BIT7; // Configure P1.4 - P1.7 for ADC function
  ADCCTL0 = ADCSHT_2 | ADCON; // ADC ON, sampling time
  ADCCTL1 = ADCSHP; // Use sampling timer
  ADCCTL2 = ADCRES; // 10-bit conversion results
  ADCIE = ADCIE0; // Enable ADC conv complete interrupt
  ADCMCTL0 = ADCINCH_4 | ADCSREF_0; // Start with channel A4 (P1.4), internal ref voltage 1.5V
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode

  __bis_SR_register(GIE); // Enable global interrupts
}


#pragma vector = ADC_VECTOR
__interrupt void ADC_ISR(void)
{
  if (currentChannel == ADCINCH_4)
  {
    channel = 4;
    //adcValue1 = ADCMEM0; // Save the result from A4
    adcValue1 = ADCMEM0;
    currentChannel = ADCINCH_5; // Switch to next channel (A5)
  }

  else if (currentChannel == ADCINCH_5)
  {
    channel = 5;
    adcValue2 = ADCMEM0; // Save the result from A5
    currentChannel = ADCINCH_6; // Switch to next channel (A6)
  }


  else if (currentChannel == ADCINCH_6)

  {
    channel = 6;
    adcValue3 = ADCMEM0; // Save the result from A6
    currentChannel = ADCINCH_7; // Switch to next channel (A7)
  }


  else if (currentChannel == ADCINCH_7)
  {
    channel = 7;
    adcValue4 = ADCMEM0; // Save the result from A5
    currentChannel = ADCINCH_4; // Switch back to first channel (A4)
  }


  ADCCTL0 &= ~ADCENC; // Disable ADC to allow configuration changes
  ADCMCTL0 = (currentChannel | ADCSREF_0); // Update channel and reference


  // Re-enable the ADC and start the next conversion
  ADCCTL0 |= ADCENC | ADCSC;
  //ADCMCTL0 = (ADCMCTL0 & ~ADCINCH_7) | currentChannel; // Set next channel

}



int main(void)
{
  configureADC(); // Configure ADC and start conversion
  ADCCTL0 |= ADCENC | ADCSC; // Start the first conversion


  while (1)
  {
    ptrLDR1 = &LDR1;
    LDR1 = adcValue1;
    LDR2 = adcValue2;
    LDR3 = adcValue3;
    LDR4 = adcValue4;
  }

}
