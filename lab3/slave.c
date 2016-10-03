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
#include "usart_ATmega1284.h"
enum LEDState {initled, offled, onled} led_state;
enum RECState {initrec, recrec} rec_state;

//inits
void LEDS_Init(){
	led_state = initled;
}

void REC_Init(){
	rec_state = initrec;
}

char ledstatus = 0;
void LEDS_Tick(){
	//Transitions
	switch(led_state){
		case initled:
			led_state = offled;
			break;
		case offled:
			if (ledstatus)
				led_state = onled;
			break;
		case onled:
			if (!ledstatus)
				led_state = offled;
			break;
		default:
			break;
	}

	//Actions
	switch(led_state){
		case initled:
			PORTA = 0x00;
			break;
		case offled:
			PORTA = 0x00;
			break;
		case onled:
			PORTA = 0xFF;
			break;
		default:
			break;
	}
}

void REC_Tick() {
	//transition
	switch(rec_state){
		case initrec:
			rec_state = recrec;
			break;
		case recrec:
			rec_state = recrec;
			break;
		default:
			break;
	}
	//action
	switch(rec_state){
		case initrec:
			break;
		case recrec:
			if (USART_HasReceived(0)){
				ledstatus = USART_Receive(0);
			}
	}
}

void LedSecTask()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick();
		vTaskDelay(1);
	}
}

void RecSecTask()
{
	REC_Init();
	for(;;)
	{
		REC_Tick();
		vTaskDelay(1);
	}
}

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartRecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(RecSecTask, (signed portCHAR *)"LedRecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	//usart
	initUSART(0);
	initUSART(1);
	
	DDRA = 0xFF; PORTA= 0x00; //output
	DDRD = 0x00; PORTD = 0xFF; //input
	
	//Start Tasks
	StartSecPulse(1);
	StartRecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
