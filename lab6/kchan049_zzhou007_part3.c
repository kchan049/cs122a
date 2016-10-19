/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #6  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/portpins.h>
#include <avr/pgmspace.h>

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"


unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

int off_row = 0;
int off_col = 0;

enum Tick_States { tick_s1, tick_s2 } tick_state;
enum Tock_States { tock_s1 } tock_state;
unsigned row = 0xFE;
int r = 0;
unsigned char ARRAY[5] = {
	0x00, 0x3C, 0x24, 0x3C, 0x00
};
void Tick_Init(){ tick_state = tick_s1; }
void Tock_Init(){ tock_state = tock_s1; }

void Tick_Tick() {
	switch(tick_state) { 
		case tick_s1:
		if (~PINA & 0x01 || ~PINA || ~PINB & 0x01 || ~PINB & 0x02) {
			tick_state = tick_s2;
			if (~PINA & 0x01) {
				if (off_row < 1)
				++off_row;
			}
			else if (~PINA & 0x02) {
				if (off_row > -1)
				--off_row;
			}
			else if (~PINB & 0x01) {
				if (off_col < 2)
				++off_col;
			}
			else if (~PINB & 0x02) {
				if (off_col > -2)
				--off_col;
			}
		}
		else if (!(~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02)) {
			tick_state = tick_s1;
		}
		break;
		case tick_s2:
		if (!(~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02)) {
			tick_state = tick_s1;
		}
		else if (~PINA & 0x01 || ~PINA & 0x02 || ~PINB & 0x01 || ~PINB & 0x02) {
			tick_state = tick_s2;
		}
		break;
		default:
		break;
	}

	switch(tick_state) {
		case tick_s1:
		break;
		case tick_s2:
		break;
		default: 
		break;
	}
}
void Tock_Tick() {
	switch(tock_state) { // Transitions
		case -1:
		tock_state = tock_s1;
		break;
		case tock_s1:
		if (1) {
			tock_state = tock_s1;
		}
		break;
		default:
		break;
	} // Transitions

	switch(tock_state) { // State actions
		case tock_s1:
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
}

void TickTask() {
	Tick_Init();
	for(;;)
	{
		Tick_Tick();
		vTaskDelay(1);
	}
}
void TockTask() {
	Tock_Init();
	for(;;)
	{
		Tock_Tick();
		vTaskDelay(2);
	}
	
}
void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TickTask, (signed portCHAR *)"TickTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse2(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TockTask, (signed portCHAR *)"TockTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	// initialize ports
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0x00; PORTB = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//Start Tasks
	StartSecPulse1(1);
	StartSecPulse2(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

