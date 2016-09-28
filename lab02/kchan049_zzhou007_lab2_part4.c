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
enum LEDState {shiftleft, shiftright} led_state;
enum BUTTONState {pressed, notpressed} button_state;

unsigned char GetBit(unsigned char x, unsigned char k)
{
	return ((x & (0x01 << k)) != 0);
}

void LEDS_Init(){
	PORTD = 0x01;
	led_state = shiftleft;
}

unsigned char shift;
void BUTTON_Init(){
	shift = 0;
	button_state = notpressed;
}

void BUTTON_Tick() {
	//Transitions
	switch (button_state){
		case pressed:
		if (GetBit(PINA, 0)){
			button_state = notpressed;
		}
		break;
		case notpressed:
		if (!GetBit(PINA, 0)){
			button_state = pressed;
			shift = 1;
		}
		break;
	}
}
void LEDS_Tick(){
	//Actions
	switch(led_state){
		case shiftleft:
		PORTD = PORTD << 1;
		break;
		case shiftright:
		PORTD = PORTD >> 1;
		break;
	}
	switch(led_state){
		case shiftleft:
			if(PORTD == 0x80 || shift) {
				if (shift)
					shift = 0;
				led_state = shiftright;
			}
			else
				led_state = shiftleft;
			break;
			
		case shiftright:
			if(PORTD == 0x01 || shift) {
				if (shift)
				shift = 0;
				led_state = shiftleft;
			}
			else
				led_state = shiftright;
			break;
	}
}

void LedSecTask()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick();
		vTaskDelay(100);
	}
	
}

void ButtonSecTask()
{
	BUTTON_Init();
	for(;;)
	{
		BUTTON_Tick();
		vTaskDelay(50);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartSecPulse2(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(ButtonSecTask, (signed portCHAR *)"ButtonSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;
	//Start Tasks
	StartSecPulse1(1);
	StartSecPulse2(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

