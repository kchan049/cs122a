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
enum LEDState {ledon, ledoff} led_state;

void LEDS_Init(){
	led_state = ledoff;
}

void LEDS_Tick(){
	//Actions
	switch(led_state){
		case ledon:
			PORTD = 21;
			break;
		case ledoff:
			PORTD = 0;
			break;	
		default:
			led_state = ledoff;
			PORTD = 0;
		break;
	}
	switch(led_state){
		case ledon:
			led_state = ledoff;
			break;
		case ledoff:
			led_state = ledon;
			break;
		default:
			led_state - ledoff;
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

void StartSecPulse(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(LedSecTask, (signed portCHAR *)"LedSecTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}	
 
int main(void) 
{ 
   DDRA = 0x00;
   DDRD = 0xFF;
   //Start Tasks  
   StartSecPulse(1);
    //RunSchedular 
   vTaskStartScheduler(); 
 
   return 0; 
}
