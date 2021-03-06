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
enum SENDState {initsend, sendsend} send_state;
enum RECState {initrec, recrec} rec_state;

unsigned char getbit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

//inits
void LEDS_Init(){
	led_state = initled;
}

void SEND_Init(){
	send_state = initsend;
}

void REC_Init(){
	rec_state = initrec;
}

char master = 0;
char ledstatus = 0;

void LEDS_Tick(){
	char timer = 10; //number of cycles 
	static char counttimer = 0; //number of loops you want
	
	unsigned char switchms = 30; //number of cycles to change follower to master
	static char switchmstimer = 0;
	
	//lights up master
	if (master)
		PORTC = 0xFF; 
	else
		PORTC = 0x00;
		
	//Transitions
	switch(led_state){
		case initled:
			led_state = offled;
			break;
		case offled:
			if (master) {
				counttimer++;
				switchmstimer = 0;
				if (counttimer == timer) {
					led_state = onled;
					counttimer = 0;
				}
			}
			else {
				switchmstimer++;
				if (switchmstimer == switchms) {
					master = 1;
					switchmstimer = 0;
				}
				if (ledstatus) {
					led_state = onled;
				}
			}
			break;
		case onled:
			if (master) {
				counttimer++;
				switchmstimer = 0;
				if (counttimer == timer) {
					led_state = offled;
					counttimer = 0;
				}
			}
			else {
				switchmstimer++;
				if (switchmstimer == switchms){
					master = 1;
					switchmstimer = 0;
				}
				if (!ledstatus) {
					led_state = offled;
				}
			}
			break;
		default:
			break;
	}
	//Actions
	switch(led_state){
		case initled:
			PORTA = 0x00;
			if (master)
				ledstatus = 0;
			break;
		case offled:
			PORTA = 0x00;
			if (master)
				ledstatus = 0;
			break;
		case onled:
			PORTA = 0xFF;
			if (master)
				ledstatus = 1;
			break;
		default:
			break;
	}
}

//only master can send
void SEND_Tick() {
	//transition
	switch(send_state){
		case initsend:
			send_state = sendsend;
			break;
		case sendsend:
			send_state = sendsend;
			break;
		default:
			break;
	}
	//action
	switch(send_state){
		case initsend:
			break;
		case sendsend:
			if (USART_IsSendReady(1))
				USART_Send(ledstatus, 1);
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
				master = 0;
			}
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

void SendSecTask()
{
	SEND_Init();
	for(;;)
	{
		SEND_Tick();
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

void StartSendPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(SendSecTask, (signed portCHAR *)"SendSendTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

void StartRecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(RecSecTask, (signed portCHAR *)"LedRecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	//usart
	initUSART(0); //rec
	initUSART(1); //send
	
	DDRA = 0xFF; PORTA= 0x00; //output
	DDRB = 0x00; PORTB = 0xFF; //input
	DDRC = 0xFF; PORTC = 0x00; //output
	
	
	
	//Start Tasks
	StartSecPulse(1);
	StartSendPulse(1);
	StartRecPulse(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
