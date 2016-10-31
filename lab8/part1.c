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

unsigned char phase[8] = {1,3,2,6,4,12,8,9};
unsigned char counter = 0;
enum SM1_States { on } sm1_state;

void SM1_Tick() {
	switch(sm1_state) { // Transitions

		case on:
			sm1_state = on;
			break;
		default:
			break;
	}

	switch(sm1_state) { // State actions
		case on:
			if (~PINB & 0x01) {
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
	DDRB = 0x00; PORTB = 0xFF;
	//Start Tasks
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
