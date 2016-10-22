#include <avr/io.h>
#include <stdlib.h>
#include "C:\Users\student\Desktop\scheduler.h"

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

enum SM2_States { SM2_s1 } SM2_State;
int TickFct_State_machine_2(int state) {
	switch(state) { // Transitions
		case -1:
		state = SM2_s1;
		break;
		case SM2_s1:
		if (1) {
			state = SM2_s1;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM2_s1:
			PORTB = (unsigned char)(ADC & 0xFF);
			PORTD = (unsigned char)((ADC >> 8) << 5);
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM2_State = state;
	return state;
}

int main(void)
{
	// initialize ports
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	tasksNum = 1; // declare number of tasks
	task tsks[1]; // initialize the task array
	tasks = tsks; // set the task array
	
	A2D_init();
	
	// define tasks
	unsigned char i=0; // task counter
	tasks[i].state = -1;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_State_machine_2;
	++i;
	
	TimerSet(2); // value set should be GCD of all tasks
	TimerOn();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
