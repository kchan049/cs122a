/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #2  Exercise #4 
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
unsigned long degrees = 0;
unsigned long numPhases;
unsigned char direction = 0;
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
			if (key != '\0' && !direction) {
				switch (key) {
					case '1':
					direction = 1;
					degrees = 90;
					break;
					case '2':
					direction = 1;
					degrees = 180;
					break;
					case '3':
					direction = 1;
					degrees = 360;
					break;
					case '4':
					direction = 2;
					degrees = 90;
					break;
					case '5':
					direction = 2;
					degrees = 180;
					break;
					case '6':
					direction = 2;
					degrees = 360;
					break;
				}
				numPhases = (degrees / 5.625) * 64;
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
