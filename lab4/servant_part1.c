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
//lab parts includes
#include "bit.h"
#include "keypad.h"
#include "lcd.h"

//FreeRTOS include files
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

// Servant code
void SPI_ServantInit(void) {
    // set DDRB to have MISO line as output and MOSI, SCK, and SS as input
    DDRB = 0x40; PORTB = ~0x40;
	// set SPCR register to enable SPI and enable SPI interrupt (pg. 168)
    SPCR |= (1<<SPE) | (1<<SPIE);
	// make sure global interrupts are enabled on SREG register (pg. 9)
	SREG = 0x80;
}

ISR(SPI_STC_vect) { // this is enabled in with the SPCR register’s “SPI
  // Interrupt Enable”
    // SPDR contains the received data, e.g. unsigned char receivedData =
	recieivedData = SPDR;
// SPDR;
}

enum ServantState {servantwait, servantrecieve} servant_state;

void Servant_Init(){
	SPI_ServantInit();
	servant_state = servantwait;
}

void Servant_Tick() {
	//Transitions
	switch (servant_state){
		case servantwait:
			servant_state = servantrecieve;
			break;
		case servantrecieve:
			
			break;
	}
}

void ServantTask()
{
	Servant_Init();
	for(;;)
	{
		Servant_Tick();
		vTaskDelay(100);
	}
	
}

void StartSecPulse1(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(ServantTask, (signed portCHAR *)"ServantTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	//Start Tasks
	//servant task
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

