/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #5  Exercise #3 
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
void transmit_data(unsigned char data, unsigned char number) {
	unsigned char i;
	for (i = 0; i < 8 ; ++i) {
		// Sets SRCLR to 1 allowing data to be set
		// Also clears SRCLK in preparation of sending data
		if (number == 0)
			PORTC = 0x08;
		else
			PORTC = 0x20;
		// set SER = next bit of data to be sent.
		PORTC |= ((data >> i) & 0x01);
		// set SRCLK = 1. Rising edge shifts next bit of data into the shift register
		PORTC |= 0x04;
	}
	// set RCLK = 1. Rising edge copies data from the “Shift” register to the “Storage” register
	if (number == 0)
		PORTC |= 0x02;
	else
		PORTC |= 0x10;
	// clears all lines in preparation of a new transmission
	PORTC = 0x00;
}

char pat1 = 0xFF;
char pat2 = 0xAA;
char pat3 = 0xF0;
char pat(char prev) {
	return ~prev;
}

char playpat(char go, char unit) {
	if (go == 1) {
		transmit_data(pat(pat1), unit);
	}
	else if (go == 2) {
		transmit_data(pat(pat2), unit);
	}
	else if (go == 3) {
		transmit_data(pat(pat3), unit);
	}
	else {
		transmit_data(0, unit);
	}
}

void flip() {
	pat1 = ~pat1;
	pat2 = ~pat2;
	pat3 = ~pat3;
}

enum TickState {tickwait, tickup, tickdown, tickpower, tickoff, tickon} tick_state;
enum Tick1State {tick1wait, tick1up, tick1down, tick1power, tick1off, tick1on} tick1_state;
enum FlipState {flipwait, flipon} flip_state;

void Tick_Init(){ tick_state = tickwait; }
void Tick1_Init(){ tick1_state = tick1wait; }
void Flip_Init(){ flip_state = flipwait; }

void Flip_Tick() {
	switch(flip_state) {
		case flipwait:
			flip_state = flipon;
			break;
		case flipon:
			flip();
			flip_state = flipon;
			break;
	}
}

void Tick_Tick() {
	//button
	char upbutton = ~GetBit(PIND, 0) & 0x01;
	char downbutton = ~GetBit(PIND, 1) & 0x01;
	//counter
	static char count = 0;
	//transaction
	switch(tick_state) {
		case tickwait:
			if (upbutton && !downbutton) {
				if (count < 2) {
					count++;
				} else if (count == 2) {
					count = 0;
				}
				tick_state = tickup;
			} else if (!upbutton && downbutton) {
				if (count > 0) {
					count--;
				} else if (count == 0) {
					count = 2;
				}
				tick_state = tickdown;
			} else if (upbutton && downbutton) {
				tick_state = tickpower;
			}
			break;
		case tickup:
			if (upbutton && downbutton) {
				tick_state = tickpower;
			} else if (!upbutton && !downbutton) {
				tick_state = tickwait;
			}
			break;
		case tickdown:
			if (upbutton && downbutton) {
				tick_state = tickpower;
			} else if (!upbutton && !downbutton) {
				tick_state = tickwait;
			}
			break;
		case tickpower:
			if (!upbutton && !downbutton) {
				tick_state = tickoff;
			}
			break;
		case tickoff:
			if (upbutton && downbutton) {
				tick_state = tickon;
			}
			break;
		case tickon:
			if (!upbutton && !downbutton) {
				tick_state = tickwait;
			}			
			break;
	}
	//action
	switch(tick_state) {
		case tickwait:
			playpat(count + 1, 0);
			break;
		case tickup:
			playpat(count + 1, 0);
			break;
		case tickdown:
			playpat(count + 1, 0);
			break;
		case tickpower:
			playpat(0, 0);
			break;
		case tickoff:
			playpat(0, 0);
			break;
	}
}

void Tick_Tick1() {
	//button
	char upbutton = ~GetBit(PIND, 2) & 0x01;
	char downbutton = ~GetBit(PIND, 3) & 0x01;
	//counter
	static char count = 0;
	//transaction
	switch(tick1_state) {
		case tick1wait:
		if (upbutton && !downbutton) {
			if (count < 2) {
				count++;
				} else if (count == 2) {
				count = 0;
			}
			tick1_state = tick1up;
			} else if (!upbutton && downbutton) {
			if (count > 0) {
				count--;
				} else if (count == 0) {
				count = 2;
			}
			tick1_state = tick1down;
			} else if (upbutton && downbutton) {
			tick1_state = tick1power;
		}
		break;
		case tick1up:
		if (upbutton && downbutton) {
			tick1_state = tick1power;
			} else if (!upbutton && !downbutton) {
			tick1_state = tick1wait;
		}
		break;
		case tick1down:
		if (upbutton && downbutton) {
			tick1_state = tick1power;
			} else if (!upbutton && !downbutton) {
			tick1_state = tick1wait;
		}
		break;
		case tick1power:
		if (!upbutton && !downbutton) {
			tick1_state = tick1off;
		}
		break;
		case tick1off:
		if (upbutton && downbutton) {
			tick1_state = tick1on;
		}
		break;
		case tick1on:
		if (!upbutton && !downbutton) {
			tick1_state = tick1wait;
		}
		break;
	}
	//action
	switch(tick1_state) {
		case tick1wait:
		playpat(count + 1, 1);
		break;
		case tick1up:
		playpat(count + 1, 1);
		break;
		case tick1down:
		playpat(count + 1, 1);
		break;
		case tick1power:
		playpat(0, 1);
		break;
		case tick1off:
		playpat(0, 1);
		break;
	}
}



void TickTask() {
	Tick_Init();
	for(;;)
	{
		Tick_Tick();
		vTaskDelay(1);
	}
}

void TickTask1() {
	Tick1_Init();
	for(;;)
	{
		Tick_Tick1();
		vTaskDelay(1);
	}
}
void FlipTask() {
	Flip_Init();
	for(;;)
	{
		Flip_Tick();
		vTaskDelay(500);
	}
}
void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TickTask, (signed portCHAR *)"TickTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartSecPulse2(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TickTask1, (signed portCHAR *)"TickTask1", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartSecPulse3(unsigned portBASE_TYPE Priority) {
	xTaskCreate(FlipTask, (signed portCHAR *)"FlipTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
int main(void) {
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x00; PORTD = 0xFF;
	//Start Tasks
	StartSecPulse1(1);
	StartSecPulse2(1);
	StartSecPulse3(1);

	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

