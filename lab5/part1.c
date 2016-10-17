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

unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}

// Ensure DDRC is setup as output
void transmit_data(unsigned char data) {
	unsigned char i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		PORTC = 0x08;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
	PORTC |= 0x02;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

enum TickState {tickwait, tickinc, tickdec, tickclear} tick_state;

void Tick_Init(){
	tick_state = tickwait;
}

void Tick_Tick() {
	//button
	char incbutton = ~GetBit(PIND, 0) & 0x01;
	char decbutton = ~GetBit(PIND, 1) & 0x01;
	//counter
	static char count = 3;
	//action
	switch(tick_state) {
		case tickwait:
			if (incbutton && !decbutton) {
				if (count <= 0xFE) {
					count++;
				}
				tick_state = tickinc;
			} else if (!incbutton && decbutton) {
				if (count >= 0x01) {
					count--;
				}
				tick_state = tickdec;
			} else if (incbutton && decbutton) {
				tick_state = tickclear;
			}
			break;
		case tickinc:
			if (incbutton && decbutton) {
				tick_state = tickclear;
			} else if (!incbutton && !decbutton) {
				tick_state = tickwait;
			}
			break;
		case tickdec:
			if (incbutton && decbutton) {
				tick_state = tickclear;
			} else if (!incbutton && !decbutton) {
				tick_state = tickwait;
			}
			break;
		case tickclear:
			if (!incbutton && !decbutton) {
				tick_state = tickwait;
			}
			break;
	}
	//transition
	switch(tick_state) {
		case tickwait:
			transmit_data(count);
			break;
		case tickinc:
			transmit_data(count);
			break;
		case tickdec:
			transmit_data(count);
			break;
		case tickclear:
			count = 0;
			transmit_data(count);
			break;
	}
}

void TickTask() {
	Tick_Init();
	for(;;)
	{
		Tick_Tick();
		vTaskDelay(50);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TickTask, (signed portCHAR *)"TickTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x00; PORTD = 0xFF;
	//Start Tasks
	StartSecPulse1(1);

	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

