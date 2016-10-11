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
	// set SS low
	PORTB=SetBit(PORTB,4,0);
	while(!(SPSR & (1<<SPIF))) { // wait for transmission to complete
		;								
	}
	// set SS high
	PORTB = SetBit(PORTB,4,1);      
}

 
unsigned char button;
unsigned char data = 0x00;
enum MasterState {masterread} master_state;

void Master_Init(){
	SPI_MasterInit();
	master_state = masterread;
}

void Master_Tick() {
	
	//Transitions
	switch (master_state){
		case masterread:
			master_state = masterread;
			break;
	}
	//Actions
	switch(master_state){
		case masterread:
		
			button=GetKeypadKey();
			switch (button){
				case 'A':
					data &= 0x0F;
					data |=  0x10;
					break;
				case 'B':
					data &= 0x0F;
					data |=  0x20;
					break;
				case 'C':
					data &= 0x0F;
					data |=  0x30;
					break;
				case 'D':
					data &= 0x0F;
					data |=  0x40;
					break;
				case '1':
					data &= 0xF0;
					data |=  0x01;
					break;
				case '2':
					data &= 0xF0;
					data |=  0x02;
					break;
				case '3':
					data &= 0xF0;
					data |=  0x03;
					break;
				case '4':
					data &= 0xF0;
					data |=  0x04;
					break;
				case '5':
					data &= 0xF0;
					data |=  0x05;
					break;
				case '6':
					data &= 0xF0;
					data |=  0x06;
					break;
				default:
					break;
			}
			//write to lcd
			LCD_ClearScreen();
            char tmp = data;
            char speed = tmp & 0x0F;
            char pattern = tmp >> 4;
            LCD_DisplayString(1,"Ptrn:    Spd:");
			if (pattern == 0) {
				LCD_Cursor(6);
				LCD_WriteData('0');
			}
			else {
				LCD_Cursor(6);
				LCD_WriteData(pattern + '@');
			}
		   LCD_Cursor(14);
           LCD_WriteData(speed + '0');
			
			//send to servant*/
			//SPI_MasterTransmit(data);
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
	DDRA = 0xFF; PORTA = 0x00;
	//ddrb is used to communicate 
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	//Start Tasks
	//master task
	StartSecPulse1(1);
	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}
