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
//variables
unsigned row = 0xFE;
int r = 0;
unsigned char ARRAY[5] = {
	0x01, 0x00, 0x00, 0x00, 0x00
};

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
enum SM1_States { sm1_s1 } sm1_state;
enum SM2_States { sm2_s1 } sm2_state;

void SM1_Init() {
	sm1_state = sm1_s1;
}
void SM2_Init() {
	sm2_state = sm2_s1;
}


void SM1_Tick() {
	static int timerROW = 0;
	static int timerCOL = 0;
	static int periodROW;
	static int periodCOL;
	switch(sm1_state) { // Transitions
		case sm1_s1:
			sm1_state = sm1_s1;
			timerROW+=100;
			timerCOL+=100;
			break;
		default:
			break;
	} // Transitions

	switch(sm1_state) { // State actions
		case sm1_s1:
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
		default:
			break;
	} // State actions
}

void SM2_Tick() {
	switch(sm2_state) { // Transitions
		case sm2_s1:
			sm2_state = sm2_s1;
			break;
		default:
			break;
	} // Transitions

	switch(sm2_state) { // State actions
		case sm2_s1:
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
		default: 
			break;
	} // State actions
}

void SM1Task() {
	SM1_Init();
	for(;;)
	{
		SM1_Tick();
		vTaskDelay(100);
	}
}
void SM2Task() {
	SM2_Init();
	for(;;)
	{
		SM2_Tick();
		vTaskDelay(2);
	}
}
void StartSecPulse1(unsigned portBASE_TYPE Priority) {
	xTaskCreate(SM1Task, (signed portCHAR *)"SM1Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse2(unsigned portBASE_TYPE Priority) {
	xTaskCreate(SM2Task, (signed portCHAR *)"SM2Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void) {
	//init
	A2D_init();
	// initialize ports
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	//Start Tasks
	StartSecPulse1(1);
	StartSecPulse2(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
