#include <avr/io.h>
#include <stdlib.h>
#include "C:\Users\student\Desktop\scheduler.h"
#include "C:\Users\student\Desktop\keypad.h"

unsigned char phase[8] = {1,3,2,6,4,12,8,9};
unsigned char counter = 0;
unsigned long numPhases;
unsigned char direction = 0;
unsigned char digit_lock = 0;
enum BL_States { SM1_On };
void forward() {
	if (counter < 7)
	++counter;
	else
	counter = 0;
}
void reverse() {
	if (counter > 0)
	--counter;
	else
	counter = 7;
}
int TickFct_BlinkLED(int state) {
	static unsigned long degrees = 90;
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
	char key;
	switch(state) { // State actions
		case SM1_On:
		key = GetKeypadKey();
		if (digit_lock)
			--digit_lock;
		if (key != '\0' && !direction && !digit_lock) {
			digit_lock = 100;
			if (key >= '0' && key <= '9') {
				degrees = degrees * 10 + (unsigned long)(key - '0');
			}
			else if (key == '#') {
				numPhases = (degrees / 5.625) * 64 + 1;
				direction = 2;
				degrees = 0;
			}
		}
		if (direction) {
			if (direction == 1) {
				forward();
			}
			else {
				reverse();
			}
			if(--numPhases == 0) {
				direction = 0;
			}
			PORTA = phase[counter];
		}
		break;
		default:
		break;
	}
	
	return state;
}

int main(void) {
	
	// initialize ports
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xF0; PORTC = 0x0F; // keypad
	
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
