/*	Partner(s) Name & E-mail: Kenneth Chan (kchan049@ucr.edu) and Zihang Zhou(zzhou007@ucr.edu)
 *	Lab Section: 21 
 *	Assignment: Lab #2  Exercise #2
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


enum LEDState {ledon, ledoff} led_state1, led_state2, led_state3;

void LEDS_Init(){
	led_state1 = ledoff;
	led_state2 = ledoff;
	led_state3 = ledoff;
}

void LEDS_Tick1(){
	//Actions
	switch(led_state1){
		case ledon:
			PORTD = PORTD | (0x01);
			break;
		case ledoff:
			PORTD = PORTD & (0xFE);
			break;
		default:
			led_state1 = ledoff;
			PORTD = PORTD & (0xFE);
			break;
	}
	switch(led_state1){
		case ledon:
			led_state1 = ledoff;
			break;
		case ledoff:
			led_state1 = ledon;
			break;
		default:
			led_state1 = ledoff;
			break;
	}
}

void LEDS_Tick2(){
	//Actions
	switch(led_state2){
		case ledon:
			PORTD = PORTD | (0x04);
			break;
		case ledoff:
			PORTD = PORTD & (0xFB);
			break;
		default:
			PORTD = PORTD & (0xFB);
			break;
	}
	switch(led_state2){
		case ledon:
			led_state2 = ledoff;
			break;
		case ledoff:
			led_state2 = ledon;
			break;
		default:
			led_state2 = ledoff;
			break;
	}
}

void LEDS_Tick3(){
	//Actions
	switch(led_state3){
		case ledon:
			PORTD = PORTD | (0x10);
			break;
		case ledoff:
			PORTD = PORTD & (0xEF);
			break;
		default:
			led_state3 = ledoff;
			PORTD = PORTD & (0xEF);
			break;
	}
	switch(led_state3){
		case ledon:
			led_state3 = ledoff;
			break;
		case ledoff:
			led_state3 = ledon;
			break;
		default:
			led_state3 = ledoff;
			break;
	}
}







void LedSecTask1()
{
	LEDS_Init();
   for(;;) 
   { 	
	LEDS_Tick1();
	vTaskDelay(500); 
   } 
}
void LedSecTask2()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick2();
		vTaskDelay(1000);
	}
}
void LedSecTask3()
{
	LEDS_Init();
	for(;;)
	{
		LEDS_Tick3();
		vTaskDelay(2500);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask1, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
void StartSecPulse2(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask2, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse3(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask3, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
} 
int main(void) 
{ 
   DDRA = 0x00;
   DDRD = 0xFF;
   //Start Tasks  
   StartSecPulse1(1);
   StartSecPulse2(1);
   StartSecPulse3(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}
