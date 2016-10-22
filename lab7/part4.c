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

void Set_A2D_Pin(unsigned char pinNum) {
	ADMUX = (pinNum <= 0x07) ? pinNum : ADMUX;
	// Allow channel to stabilize
	static unsigned char i = 0;
	for ( i=0; i<15; i++ ) { asm("nop"); }
}

enum SM2_States { SM2_s1 } SM2_State;
unsigned row = 0xFE;
int r = 0;
unsigned char ARRAY[5] = {
	0x01, 0x00, 0x00, 0x00, 0x00
};
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
		if (r == 4) {
			row = 0xFE;
			r = 0;
		}
		else {
			row = (row << 1) | 0x01;
			++r;
		}

		PORTC = ARRAY[r];
		PORTD = row;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM2_State = state;
	return state;
}

unsigned char p_x, p_y;
void left() {
	if (p_x < 7) {
		ARRAY[p_y] <<= 1;
		++p_x;
	}
}
void right() {
	if (p_x > 0) {
		ARRAY[p_y] >>= 1;
		--p_x;
	}
}
void swap(unsigned char a, unsigned char b) {
	unsigned char temp = ARRAY[a];
	ARRAY[a] = ARRAY[b];
	ARRAY[b] = temp;
}
void up() {
	if (p_y < 4)
	{
		swap(p_y,p_y+1);
		++p_y;
	}
}
void down() {
	if (p_y > 0)
	{
		swap(p_y,p_y-1);
		--p_y;
	}
}
enum SM1_States { SM1_s1 } SM1_State;

int TickFct_State_machine_1(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	static int timerROW = 0;
	static int timerCOL = 0;
	static int periodROW;
	static int periodCOL;
//	my_period = 100;
	switch(state) { // Transitions
		case -1:
		p_x = 0;

		p_y = 0;
		SM1_State = SM1_s1;
		break;
		case SM1_s1:
		if (1) {
			SM1_State = SM1_s1;
			timerROW+=100;
			timerCOL+=100;
		}
		break;
		default:
		SM1_State = SM1_s1;
	} // Transitions

	switch(SM1_State) { // State actions
		case SM1_s1:
		
		Set_A2D_Pin(0x00);
		if (ADC < 49 || ADC > 899)
			periodROW = 100;
		else if (ADC >= 49 && ADC <= 225 || ADC >= 658 && ADC <= 899)
			periodROW = 250;
		else if (ADC >= 226 && ADC <= 379 || ADC >= 408 && ADC <= 657)
			periodROW = 500;
		else
			periodROW = 1000;
		if (timerROW >= periodROW) {
			timerROW = 0;
			if (ADC < 386) {
				left();
			}
			else if (ADC > 447) {
				right();
			}
		}
		
		Set_A2D_Pin(0x01);
		if (ADC < 134 || ADC > 797)
			periodCOL = 100;
		else if (ADC >= 135 && ADC <= 220 || ADC >= 696 && ADC <= 796)
			periodCOL = 250;
		else if (ADC >= 221 && ADC <= 320 || ADC >= 595 && ADC <= 695)
			periodCOL = 500;
		else
			periodCOL = 1000;
		if (timerCOL >= periodCOL) {
			timerCOL = 0;
			if (ADC < 416) {
				up();
			}
			else if (ADC > 480) {
				down();
			}
		}
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	return SM1_State;
}

int main(void)
{
	// initialize ports
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	tasksNum = 2; // declare number of tasks
	task tsks[2]; // initialize the task array
	tasks = tsks; // set the task array
	
	A2D_init();
	
	// define tasks
	unsigned char i=0; // task counter
	tasks[i].state = -1;
	tasks[i].period = 2;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_State_machine_2;
	++i;
	tasks[i].state = -1;
	tasks[i].period = 100;
	tasks[i].elapsedTime = tasks[i].period;
	tasks[i].TickFct = &TickFct_State_machine_1;
	++i;
	
	TimerSet(2); // value set should be GCD of all tasks
	TimerOn();
    while(1)
    {
        //TODO:: Please write your application code 
    }
}
