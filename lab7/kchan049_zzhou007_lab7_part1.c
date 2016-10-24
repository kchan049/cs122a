/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #7  Exercise #1
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

void A2D_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: Enables analog-to-digital conversion
	// ADSC: Starts analog-to-digital conversion
	// ADATE: Enables auto-triggering, allowing for constant
	//	    analog to digital conversions.
}

enum SM1_States { sm1_s1 } sm1_state;
	
void SM1_Init() {
	sm1_state = sm1_s1;
}

void SM1_Tick() {
	switch(sm1_state) { // Transitions
		case sm1_s1:
			sm1_state = sm1_s1;
			break;
		default:
			break;
	} // Transitions

	switch(sm1_state) { // State actions
		case sm1_s1:
			PORTB = (unsigned char)(ADC & 0xFF);
			PORTD = (unsigned char)((ADC >> 8) << 5);
			break;
		default:
			break;
	}
}

void SM1Task() {
	SM1_Init();
	for(;;)
	{
		SM1_Tick();
		vTaskDelay(1);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(SM1Task, (signed portCHAR *)"SM1Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	// initialize ports
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//Start Tasks
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

