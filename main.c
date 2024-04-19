#include <msp430.h>
#include <driverlib.h>
#include "liamsetup.h"
#include "hal_LCD.h"
#include <math.h>

void leaderboard(int x)
{
    showChar(((x / 1000) % 10) + 48, pos1);
    showChar(((x / 100) % 10) + 48, pos2);
    showChar(((x / 10) % 10) + 48, pos3);
    showChar(((x / 1) % 10) + 48, pos4);
}

long joystickValue = 0;
long servoPosition =0;


volatile unsigned int seconds = 120; // Initial count down time in seconds

#define DEBOUNCE 100000

//creating global interrupt flags
volatile bool SW1_interruptFlag_ = false;
volatile bool SW2_interruptFlag_ = false;

void configureButtonS1S2Interrupt(void)
{
    P1DIR &= ~(BIT3 | BIT4); // Clearing the bits sets them as input (using bitwise AND with the complement)

    // Enable pull-up resistors on P1.3 and P1.4 (PxOUT and PxREN registers)
    P1OUT |= (BIT3 | BIT4); // Setting the bits in PxOUT register enables pull-up mode when PxREN is set
    P1REN |= (BIT3 | BIT4); // Setting the bits in PxREN register enables the pull-up/pull-down resistors

    // Set interrupt edge for P1.3 and P1.4 to high-to-low transition (PxIES register)
    P1IES |= (BIT3 | BIT4); // Setting the bits configures the interrupt edge select to trigger on a high-to-low transition

    // Clear interrupt flags for P1.3 and P1.4 to ensure no pending interrupts (PxIFG register)
    P1IFG &= ~(BIT3 | BIT4); // Clearing the bits ensures there are no pending interrupts

    // Enable interrupts for P1.3 and P1.4 (PxIE register)
    P1IE |= (BIT3 | BIT4); // Setting the bits enables interrupts for P1.3 and P1.4
}



#pragma vector=PORT1_VECTOR

__interrupt void Port_1(void)
{

  switch(__even_in_range(P1IV, P1IV_P1IFG7))
  {

  case P1IV_P1IFG3:
    SW1_interruptFlag_ = 1;

    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN3);    //Technically a redundant statement as the even in range cleared this
    __delay_cycles(100000); //debounce the button press
    break;


    // Case for the interrupt flag from pin P1.4. It's similar to the previous case but for
    // pin P1.4, which is associated with button S2.
  case P1IV_P1IFG4: // Button S2 on P1.4 was pressed
    // Set the SW2_interruptFlag_ to 1, indicating that the interrupt occurred on P1.4.
    SW2_interruptFlag_ = 1;
    // Clear the interrupt flag for P1.4.
    GPIO_clearInterrupt(GPIO_PORT_P1, GPIO_PIN4);
    __delay_cycles(100000);//debounce the button press
    break;

  }
}

void updateDisplay(int x)
{
    // Convert seconds to minutes and seconds

    unsigned int minutes = x / 60;
    unsigned int secs = x % 60;

    // Display countdown time
    showChar((minutes / 10) + '0', pos1); // Display tens of minutes
    showChar((minutes % 10) + '0', pos2); // Display ones of minutes
    showChar(' ', pos3);
    showChar((secs / 10) + '0', pos4); // Display tens of seconds
    showChar((secs % 10) + '0', pos5); // Display ones of seconds
    showChar(' ', pos6); // Display ones of seconds

       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);
       __delay_cycles(DEBOUNCE);

}



void timeremaining()
{

    int pinState1 = 1; // Assuming switches are initially not pressed
    int pinState2 = 1;

    while (seconds > 0)  // Loop until time runs out
    {
       pinState1 = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN3); //SW1
       pinState2 = GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN4); //SW2

        updateDisplay(seconds); // Update display with current count down time

        // Check if either switch is pressed
        if (pinState1 == 0 || pinState2 == 0)
        {
            break; // Exit the loop if either switch is pressed
        }

        __delay_cycles(100000); // Delay for 1 second
        seconds--; // Decrement seconds




    }
}

volatile unsigned int finalscore;
volatile unsigned int ADC_Result;
volatile unsigned int ADC_Result1;
volatile unsigned int ADC_Result2;

int main(void)
{
     setupadc();            // Initialise ADC
     configureMotor();      // Initialise servo motor control
     PM5CTL0&=~LOCKLPM5;



    // Configure ADC10
       ADCCTL0 |= ADCSHT_2 | ADCON;                             // ADCON, S&H=16 ADC clks
       ADCCTL1 |= ADCSHP;                                       // ADCCLK = MODOSC; sampling timer
       ADCCTL2 |= ADCRES;                                       // 10-bit conversion results
       ADCMCTL0 |= ADCINCH_9;                                   // A1 ADC input select; Vref=AVCC
      // ADCIE |= ADCIE0;                                       // Enable ADC conv complete interrupt


     WDTCTL = WDTPW | WDTHOLD;   // Stop watch-dog timer
     // initialiseLedDial();
      PMM_unlockLPM5();
      configureButtonS1S2Interrupt();

      __enable_interrupt();

      Init_LCD();                                              // Initialize LCD
      //initialiseADC_Easy();                                    // Initialize ADC


     displayScrollText("MINI GRID GAME");
      while(seconds > 0)
      {
          // Read joystick input
          int servoVal = analogRead();

          // Map joystick input to servo position
         servoVal = map(servoVal, 0, 1023, -90, 300);

         // Set the servo position
         TA0CCR1 = (servoVal * 1000 / 180) + 1000;

         // Wait for the servo to get there
         __delay_cycles(200000);

        // Check interrupt flags to determine if button S1 or S2 was pressed
        // SW1 has replace the old pooling for the pinstate (current button state)
        if (SW1_interruptFlag_)    //SW1 pressed therefore display Player 1 stats
        {
            seconds--;
            displayScrollText("PLAYER 1 STATS");

            unsigned int timer = 300; // 3 seconds timer
             while(timer>0)                                      // try implement an escape
                 {

                 // Read joystick input
                   int servoVal = analogRead();

                   // Map joystick input to servo position
                  servoVal = map(servoVal, 0, 1023, -90, 300);

                  // Set the servo position
                  TA0CCR1 = (servoVal * 1000 / 180) + 1000;

                  // Wait for the servo to get there
                  __delay_cycles(20);
                   ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start


                     while( (ADCIFG & ADCIFG0) == 0 );

                     if (!SW1_interruptFlag_ || SW2_interruptFlag_ )  // Implementing an escape

                       break;

                     ADC_Result = ADCMEM0;


                     //finding each order
                     showChar(((ADC_Result / 1000) % 10) + 48,pos1);
                     showChar(((ADC_Result/ 100) % 10) + 48,pos2);
                     showChar(((ADC_Result/ 10) % 10) + 48,pos3);
                     showChar(((ADC_Result/ 1) % 10) + 48,pos4);
                     showChar('K',pos5);
                     showChar('W',pos6);
                     __delay_cycles(5000);
                     timer--;

                     ADC_Result1+=ADC_Result;
                 }

             seconds--;seconds--;seconds--;



             SW1_interruptFlag_ = 0;
        }

        if (SW2_interruptFlag_)
        {
            displayScrollText("PLAYER 2 STATS");
            unsigned int timer = 300; // 3 seconds timer

            while(timer>0)                                                       // try implement an escape
             {

                // Read joystick input
                  int servoVal = analogRead();

                  // Map joystick input to servo position
                  servoVal = map(servoVal, 0, 1023, -90, 300);

                 // Set the servo position
                 TA0CCR1 = (servoVal * 1000 / 180) + 1000;

                // Wait for the servo to get there
                __delay_cycles(20);

                 ADCCTL0 |= ADCENC | ADCSC;                           // Sampling and conversion start

                 while( (ADCIFG & ADCIFG0) == 0 );

                 if (!SW2_interruptFlag_ || SW1_interruptFlag_ )  // Implementing an escape
                   break;

                 ADC_Result = ADCMEM0;


                 //finding each order
                 showChar(((ADC_Result / 1000) % 10) + 48,pos1);
                 showChar(((ADC_Result / 100) % 10) + 48,pos2);
                 showChar(((ADC_Result / 10) % 10) + 48,pos3);
                 showChar(((ADC_Result / 1) % 10) + 48,pos4);
                 showChar('K',pos5);
                 showChar('W',pos6);
                 __delay_cycles(5000);
                 timer --;

                 ADC_Result2+=ADC_Result;

             }
            seconds--;seconds--;seconds--;


            SW2_interruptFlag_ = 0; // Clear the interrupt flag

        }

        seconds--;
        updateDisplay(seconds);

      }

      finalscore= ADC_Result1+ADC_Result2;

      displayScrollText("GAME OVER");

      displayScrollText("FINAL SCORE");

      leaderboard(finalscore);


}

/*
void leaderboard(int x)
{
    while(1)
    {
    showChar(((x / 1000) % 10) + 48,pos1);
    showChar(((x/ 100) % 10) + 48,pos2);
    showChar(((x/ 10) % 10) + 48,pos3);
    showChar(((x/ 1) % 10) + 48,pos4);
    showChar('K',pos5);
    showChar('W',pos6);
    }

}
*/
