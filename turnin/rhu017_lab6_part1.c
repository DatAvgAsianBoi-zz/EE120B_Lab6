/*	Author: huryan18
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;
unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

enum States {start, light1, light2, light3} state;
unsigned char tmpB;

void TimerOn(){
      TCCR1B = 0x0B;    //bit3 = 0: clear timer on compare; 8MHz clock = 8000000/64 = 125,000 ticks/s
      OCR1A = 125;      //Timer interupt generated when TCNT1 == OCR1A; We want 1ms tick, so 125000 * .001 = 125
      TIMSK1 = 0x2;     //enables interupt for compare matched
      TCNT1 = 0;        //init avr counter
      _avr_timer_cntcurr = _avr_timer_M;
      SREG |= 0x80;
}
void TimerOff(){
      TCCR1B = 0x00;
}
void TimerISR(){
      TimerFlag = 1;
}
ISR(TIMER1_COMPA_vect){
      _avr_timer_cntcurr--;
      if(_avr_timer_cntcurr == 0){
            TimerISR();
            _avr_timer_cntcurr = _avr_timer_M;
      }
}
void TimerSet(unsigned long M){
      _avr_timer_M = M;
      _avr_timer_cntcurr = _avr_timer_M;
}

void tick(){
      switch(state){
            case start:
                  state = light1;
                  tmpB = 0xFF;
                  break;
            case light1:
                  state = light2;
                  break;
            case light2:
                  state = light3;
                  break;
            case light3:
                  state = light1;
                  break;
      }
      switch(state){
            case light1:
                  tmpB = 0x01;
                  break;
            case light2:
                  tmpB = 0x02;
                  break;
            case light3:
                  tmpB = 0x04;
                  break;
      }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRB = 0xFF;
    PORTB = 0x00;
    TimerSet(1000);
    TimerOn();
    tmpB = 0xFF;
    /* Insert your solution below */
    state = start;
    while (1) {
          tick();
          PORTB = tmpB;
          while(!TimerFlag);
          TimerFlag = 0;
    }
    return 1;
}
