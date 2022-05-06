#include <msp430.h>
#include "timerLib/libTimer.h"
#include "buzzer.h"

#define LED_RED BIT6               // P1.6
#define LED_GREEN BIT0             // P1.0
#define LEDS (LED_RED | LED_GREEN)

#define SW1 BIT3/* switch1 is p1.3 */
#define SWITCHES SW1/* only 1 switch on this board */

int count = 0;
int state = 1;
int period = 0;
int cycle = 500;
int buzzerOn = 0;

void switch_init() {
  P1REN |= SWITCHES;/* enables resistors for switches */
  P1IE |= SWITCHES;/* enable interrupts from switches */
  P1OUT |= SWITCHES;/* pull-ups for switches */
  P1DIR &= ~SWITCHES;/* set switches' bits for input */
}

void led_init() {
  P1DIR |= LEDS;
  P1OUT &= ~LEDS;/* leds initially off */
}

void wdt_init() {
  configureClocks();/* setup master oscillator, CPU & peripheral clocks */
  enableWDTInterrupts();/* enable periodic interrupt */
}

void main(void){
  switch_init();
  led_init();
  wdt_init();
  buzzer_init();
  or_sr(0x18);  // CPU off, GIE on
}

static int buttonDown;

void

switch_interrupt_handler(){
  char p1val = P1IN;/* switch is in P1 */
  /* update switch interrupt sense to detect changes from current buttons */
  P1IES |= (p1val & SWITCHES);/* if switch up, sense down */
  P1IES &= (p1val | ~SWITCHES);/* if switch down, sense up */
  if (p1val & SW1) {
    buttonDown = 0;
    buzzer_set_period(1000);
  }else{
    buttonDown =1;
    buzzer_set_period(500);
  }
}

/* Switch on P1 (S2) */

void

__interrupt_vec(PORT1_VECTOR) Port_1(){
  if (P1IFG & SWITCHES) {      /* did a button cause this interrupt? */
    P1IFG &= ~SWITCHES;      /* clear pending sw interrupts */
    switch_interrupt_handler();/* single handler for all switches */
  }
}
greenOn(){
  P1OUT |= LED_GREEN;
  return 0;
}
greenOff(){
  P1OUT &= ~LED_GREEN;
    return 0;
}
redOn(){
  P1OUT |= LED_RED;
  return 0;
}
redOff(){
  P1OUT &= ~LED_RED;
  return 0;
}

void

__interrupt_vec(WDT_VECTOR) WDT()/* 250 interrupts/sec */{  
  count++;
  if(buzzerOn){
    buzzer_set_period(500);
  }
  if(count > cycle){
    count=0;
    state++;
  }
  if(state == 4){
      state = 0;
  }

  switch (state) {
  case 0: // both leds off
    P1OUT &= ~LEDS;
    //buzzer_set_period(1000);
    break;
  case 1: //green led on only
    P1DIR |= LED_GREEN;
    redOff();
    greenOn();
    //buzzer_set_period(700);
    break;
   case 2: // red led on only
    P1DIR |= LED_RED;
    greenOff();
    redOn();
    //buzzer_set_period(400);
    break;
   case 3: //both leds alternate and buzz
     P1DIR |= LEDS;
     // buzzer_set_period(200);
     period++;
     if(period < 50){
       greenOn();
       redOff();
     }
     else{
       redOn();
       greenOff();
     }
     if(period >100)period =0;
    break;
  default:
    count++;//if (!buttonDown) /* don't blink off if button is down */
  }
} 
