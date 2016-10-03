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

//inits
void LEDS_Init(){
	led_state = initled;
}

void SEND_Init(){
	send_state = initsend;
}

char ledstatus = 0;
void LEDS_Tick(){
	//Transitions
	switch(led_state){
		case initled:
			led_state = offled;
			break;
		case offled:
			led_state = onled;
			break;
		case onled:
			led_state = offled;
			break;
		default:
			break;
	}

	//Actions
	switch(led_state){
		case initled:
			PORTA = 0x00;
			ledstatus = 0;
			break;
		case offled:
			PORTA = 0x00;
			ledstatus = 0;
			break;
		case onled:
			PORTA = 0xFF;
			ledstatus = 1;
			break;
		default:
			break;
	}
}

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
			if (USART_IsSendReady(0)){
				USART_Send(ledstatus, 0);	
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
	vTaskDelay(1000); 
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

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	

void StartSendPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(SendSecTask, (signed portCHAR *)"LedSendTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
 
int main(void) 
{ 
   //usart 
   initUSART(0);
   initUSART(1);
   
   DDRA = 0xFF; PORTA= 0x00; //output
   DDRD = 0xFF; PORTD = 0x00; //output
   
   //Start Tasks  
   StartSecPulse(1);
   StartSendPulse(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}
