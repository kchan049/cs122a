/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #8  Exercise #3 
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
#include "keypad.h"

unsigned char phase[8] = {1,3,2,6,4,12,8,9};
unsigned char counter = 0;
unsigned long numPhases;
unsigned char direction = 0;
unsigned char digit_lock = 0;
enum SM1_States { on } sm1_state;

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

void SM1_Tick() {
	static unsigned long degrees = 90;
	switch(sm1_state) { // Transitions
		case on:
			sm1_state = on;
			break;
		default:
			break;
	}
	char key;
	switch(sm1_state) { // State actions
		case on:
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
}

void SM1_Init() {
	sm1_state = on;
}


void SM1Task() {
	SM1_Init();
	for(;;)
	{
		SM1_Tick();
		vTaskDelay(3);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(SM1Task, (signed portCHAR *)"SM1Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	//init
	// initialize ports
	DDRA = 0xFF; PORTA = 0x00;
	DDRD = 0xF0; PORTD = 0x0F;
	//Start Tasks
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
