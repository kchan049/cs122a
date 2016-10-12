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

// Master code
void SPI_MasterInit(void) {
	// Set DDRB to have MOSI, SCK, and SS as output and MISO as input
	DDRB = 0xBF; PORTB = 0x40;
	// Set SPCR register to enable SPI, enable master, and use SCK frequency
	SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0);
	//   of fosc/16  (pg. 168)
	// Make sure global interrupts are enabled on SREG register (pg. 9)
	SREG=0x80;
}

void SPI_MasterTransmit(unsigned char cData) {
	// data in SPDR will be transmitted, e.g. SPDR = cData;
	SPDR = cData;
	// set SS low
	PORTB=SetBit(PORTB,4,0);
	while(!(SPSR & (1<<SPIF))) { // wait for transmission to complete
		;
	}
	// set SS high
	PORTB = SetBit(PORTB,4,1);      
}

enum MasterState {masterwait, mastersend} master_state;

void Master_Init(){
	SPI_MasterInit();
	master_state = masterwait;
}

void Master_Tick() {
	//Transitions
	switch (master_state){
		case masterwait:
			master_state = mastersend;
			break;
		case mastersend:
			SPI_MasterTransmit('c');
			break;
	}
}

void MasterTask()
{
	Master_Init();
	for(;;)
	{
		Master_Tick();
		vTaskDelay(100);
	}
	
}

void StartSecPulse1(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(MasterTask, (signed portCHAR *)"MasterTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

int main(void)
{
	//Start Tasks
	//master task
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

