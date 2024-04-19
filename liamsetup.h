/**************************************************
 * ADC.h
 * Drives ADC
 *
 * Copyright 2015 University of Strathclyde
 *
 *
 **************************************************/
#include <msp430.h>
#include <driverlib.h>

extern void initialiseADC_Easy();  //Initialise ADC, setting GPIO parameters
extern void initialiseADC_Advanced();
void configureMotor();

void setzeroposition();

void setupadc();

void SetServoPosition(int x);

long map(long x, long in_min, long in_max, long out_min, long out_max);

void joystickRead(unsigned int *xAxis);

int analogRead();



//extern void setLedDial(unsigned char value); //Set dial value
//extern void refreshLedDial(); //Refresh the display
