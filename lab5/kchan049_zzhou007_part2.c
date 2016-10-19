/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #5  Exercise #2 
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

enum TickState {tickwait, tickup, tickdown, tickpower, tickoff, tickon} tick_state;
enum Pat1State {pat1wait, pat1start, pat1next} pat1_state;
enum Pat2State {pat2wait, pat2start, pat2next} pat2_state;
enum Pat4State {pat4wait, pat4start, pat4next} pat4_state;
		
void Tick_Init(){ tick_state = tickwait; }
void Pat1_Init(){ pat1_state = pat1wait; }
void Pat2_Init(){ pat2_state = pat2wait; }
void Pat4_Init(){ pat4_state = pat4wait; }
	
unsigned char go = 0;
void Tick_Tick() {
	//buttonnex
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
	}
	//action
	switch(tick_state) {
		case tickwait:
			go = count + 1;
			break;
		case tickup:
			go = count + 1;
			break;
		case tickdown:
			go = count + 1;
			break;
		case tickpower:
			go = 0;
			transmit_data(0x00);
			break;
		case tickoff:
			transmit_data(0x00);
			break;
	}
}

void Pat1_Tick() {
	//trans
	switch (pat1_state){
		case pat1wait:
		if (go == 1)
		pat1_state = pat1start;
		else
		pat1_state = pat1wait;
		break;
		case pat1start:
		if (go == 1)
		pat1_state = pat1next;
		else
		pat1_state = pat1wait;
		break;
		case pat1next:
		if (go == 1)
		pat1_state = pat1start;
		else
		pat1_state = pat1wait;
		break;
		
	}
	//action
	switch (pat1_state){
		case pat1wait:
		break;
		case pat1start:
		transmit_data(0x0F);
		break;
		case pat1next:
		transmit_data(0xF0);
		break;
		
		
	}
}

void Pat2_Tick() {
	//trans
	switch (pat2_state){
		case pat2wait:
		if (go == 2)
		pat2_state = pat2start;
		else
		pat2_state = pat2wait;
		break;
		case pat2start:
		if (go == 2)
		pat2_state = pat2next;
		else
		pat2_state = pat2wait;
		break;
		case pat2next:
		if (go == 2)
		pat2_state = pat2start;
		else
		pat2_state = pat2wait;
		break;
		
	}
	//action
	switch (pat2_state){
		case pat2wait:
		break;
		case pat2start:
		transmit_data(0xAA);
		break;
		case pat2next:
		transmit_data(0x55);
		break;
		
		
	}
}

void Pat4_Tick() {
	//trans
	switch (pat4_state){
		case pat4wait:
		if (go == 3)
		pat4_state = pat4start;
		else
		pat4_state = pat4wait;
		break;
		case pat4start:
		if (go == 3)
		pat4_state = pat4next;
		else
		pat4_state = pat4wait;
		break;
		case pat4next:
		if (go == 3)
		pat4_state = pat4start;
		else
		pat4_state = pat4wait;
		break;
	}
	//action
	switch (pat4_state){
		case pat4wait:
		break;
		case pat4start:
		transmit_data(0xFF);
		break;
		case pat4next:
		transmit_data(0x00);
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
void Pat1Task() {
	Pat1_Init();
	for(;;)
	{
		Pat1_Tick();
		vTaskDelay(100);
	}
}
void Pat2Task() {
	Pat2_Init();
	for(;;)
	{
		Pat2_Tick();
		vTaskDelay(100);
	}
}
void Pat4Task() {
	Pat4_Init();
	for(;;)
	{
		Pat4_Tick();
		vTaskDelay(100);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(TickTask, (signed portCHAR *)"TickTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse2(unsigned portBASE_TYPE Priority) {
	xTaskCreate(Pat1Task, (signed portCHAR *)"Pat1Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse3(unsigned portBASE_TYPE Priority) {
	xTaskCreate(Pat2Task, (signed portCHAR *)"Pat2Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse4(unsigned portBASE_TYPE Priority) {
	xTaskCreate(Pat4Task, (signed portCHAR *)"Pat4Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x00; PORTD = 0xFF;
	//Start Tasks
	StartSecPulse1(1);
	StartSecPulse2(1);
	StartSecPulse3(1);
	StartSecPulse4(1);
	
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

