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

void LEDS_Init(){
	PORTD = 0x01;
	led_state = shiftleft;
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
		default:
		PORTD = 0;
		break;
	}
	switch(led_state){
		case shiftleft:
		if(PORTD == 0x80)
			led_state = shiftright;
		else
			led_state = shiftleft;
		break;
		case shiftright:
		if(PORTD == 0x01 )
			led_state = shiftleft;
		else
			led_state = shiftright;
		break;
		default:
		led_state = shiftleft;
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
