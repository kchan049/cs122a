#include <avr/io.h>
#include <stdlib.h>
#include "C:\Users\student\Desktop\scheduler.h"

int off_row = 0;
int off_col = 0;

enum SM1_States { SM1_s1, SM1_s2 } SM1_State;
int TickFct_State_machine_1(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	switch(state) { // Transitions
		case -1:
		state = SM1_s1;
		break;
		case SM1_s1:
		if (~PINA & 0x01 || ~PINA || ~PINB & 0x01 || ~PINB & 0x02) {
			state = SM1_s2;
			if (~PINA & 0x01) {
				if (off_row < 0)
					++off_row;
			}
			else if (~PINA & 0x02) {
				if (off_row > -4)
					--off_row;
			}
			else if (~PINB & 0x01) {
				if (off_col < 7)
					++off_col;
			}
			else if (~PINB & 0x02) {
				if (off_col > 0)
					--off_col;
			}
		}
		else if (!(~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02)) {
			state = SM1_s1;
		}
		break;
		case SM1_s2:
		if (!(~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02)) {
			state = SM1_s1;
		}
		else if (~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02) {
			state = SM1_s2;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SM1_s1:
		break;
		case SM1_s2:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM1_State = state;
	return state;
}

enum SM2_States { SM2_s1 } SM2_State;
unsigned row = 0xFE;
int r = 0;
unsigned char ARRAY[5] = {
	0xFF
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
		if (r + off_row > 4 || r + off_row < 0)
			PORTC = 0;
		else
		{
			unsigned char pattern = ARRAY[r + off_row];
			if (off_col < 0) {
				for (int i = 0; i < abs(off_col); ++i)
					pattern <<= 1;
			}
			else {
				for (int i = 0; i < off_col; ++i)
					pattern >>= 1;
			}
			PORTC = pattern;
		}
		PORTD = row;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SM2_State = state;
	return state;
}
