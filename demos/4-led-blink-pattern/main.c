//Alternate LEDs from Off, Green, and Red
#include <msp430.h>
#include "libTimer.h"
#include "led.h"

int main(void) {
  P1DIR |= LEDS;
  P1OUT &= ~LED_GREEN;
  P1OUT |= LED_RED;

  configureClocks();		/* setup master oscillator, CPU & peripheral clocks */
  enableWDTInterrupts();	/* enable periodic interrupt */
  
  or_sr(0x18);			/* CPU off, GIE on */
}

// global state vars that control blinking
int blinkLimit = 8;  // duty cycle = 1/blinkLimit
int blinkCount = 0;  // cycles 0...blinkLimit-1
int secondCount = 0; // state var representing repeating time 0…1s
int blinkLimitRed = 0;  //blink limit for red LED
int blinkCountRed = 0;  // count for red LED


void
__interrupt_vec(WDT_VECTOR) WDT()	/* 250 interrupts/sec */
{
  // handle blinking 
  blinkCount ++;
  if (blinkCount >= blinkLimit) { // on for 1 interrupt period
    blinkCount = 0;
    P1OUT |= LED_GREEN;
  } else		          // off for blinkLimit - 1 interrupt periods
    P1OUT &= ~LED_GREEN;

  blinkCountRed ++;
  if(blinkCountRed >= blinkLimitRed){
    blinkCountRed = 0;
    P1OUT |= LED_RED;
  } else
    P1OUT &= ~LED_RED;

  // measure a second
  secondCount ++;
  if (secondCount >= 250) {  // once each second
    secondCount = 0;
    
    blinkLimitRed = blinkLimitRed + 2;
    if(blinkLimitRed >= 8)
      blinkLimitRed = 0;
    
    blinkLimit = blinkLimit - 2;	     // reduce duty cycle
    if (blinkLimit <= 0)     // but don't let duty cycle go below 1/7.
      blinkLimit = 8;
  }
} 

