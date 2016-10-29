#include <avr/io.h>
#include "C:\Users\student\Desktop\scheduler.h"

unsigned char phase[8] = {1,3,2,6,4,12,8,9};
unsigned char counter = 0;
enum BL_States { SM1_On };
int TickFct_BlinkLED(int state) {
	switch(state) { // Transitions
		case -1: // Initial transition
		state = SM1_On;
		break;
		case SM1_On:
			state = SM1_On;
		break;
		default:
		state = -1;
	}

	switch(state) { // State actions
		case SM1_On:
			if (~PINB & 0x03 == 3) {
				
			}
			else if (~PINB & 0x01) {
				if (counter < 7)
				++counter;
				else
				counter = 0;
			}
			else if (~PINB & 0x02) {
				if (counter > 0)
				--counter;
				else
				counter = 7;
			}
		
		PORTA = phase[counter];
		break;
		default:
		break;
	}
	
	return state;
}

int main(void) {
	
	// initialize ports
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0x00; PORTB = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	
	tasksNum = 1; // declare number of tasks
	task tsks[1]; // initialize the task array
	tasks = tsks; // set the task array
	unsigned char pp = 3;
	// define tasks
	unsigned char i=0; // task counter
	tasks[i].state = -1;
	tasks[i].period = pp;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_BlinkLED;
	++i;
	
	TimerSet(pp); // value set should be GCD of all tasks
	TimerOn();

	while(1) {} // task scheduler will be called by the hardware interrupt
	
}
