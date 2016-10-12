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

unsigned char recieivedData = 0x00;
ISR(SPI_STC_vect) { // this is enabled in with the SPCR register’s “SPI
  // Interrupt Enable”
    // SPDR contains the received data, e.g. unsigned char receivedData =
	recieivedData = SPDR;
// SPDR;
}
unsigned char temp;
unsigned char pattern;
unsigned char speed;

enum ServantState {servantwait, servantrecieve} servant_state;
enum Pat1State {pat1wait, pat1start, pat1next} pat1_state;
enum Pat2State {pat2wait, pat2start, pat2next} pat2_state;
enum Pat3State {pat3wait, pat3start, pat3next} pat3_state;
enum Pat4State {pat4wait, pat4start, pat4next} pat4_state;

void Servant_Init(){
	SPI_ServantInit();
	servant_state = servantwait;
}
void Pat1_Init(){pat1_state = pat1wait;}
void Pat2_Init(){pat2_state = pat2wait;}
void Pat3_Init(){pat3_state = pat3wait;}
void Pat4_Init(){pat4_state = pat4wait;}

void Servant_Tick() {
	//Transitions
	switch (servant_state){
		case servantwait:
			servant_state = servantrecieve;
			break;
		case servantrecieve:
			servant_state = servantrecieve;
			break;
	}
	//actions
	switch (servant_state){
		case servantwait:
			break;
		case servantrecieve:
			 temp = recieivedData;
			 speed = temp & 0x0F;
			 pattern = temp >> 4;
			 break;
	}
}

void Pat1_Tick() {
	static int count = 0;
	//trans
	switch (pat1_state){
		case pat1wait:
			count = 0;
			if (pattern == 1)
				pat1_state = pat1start;
			else
				pat1_state = pat1wait;
			break;
		case pat1start:
			count++;
			if (speed == 1) {
				if (count < 40)
					break;
			}
			else if (speed == 2) {
				if (count < 20)
					break;
			}
			else if (speed == 3) {
				if (count < 10)
					break;
			}
			else if (speed == 4) {
				if (count < 5)
					break;
			}
			else if (speed == 5) {
				if (count < 2)
					break;
			}
			
			if (pattern == 1) {
				count = 0;
				pat1_state = pat1next;
			}
			else {
				count = 0;
				pat1_state = pat1wait;
			}
			break;
		case pat1next:
			count++;
			if (speed == 1) {
				if (count < 40)
					break;
			}
			else if (speed == 2) {
				if (count < 20)
					break;
			}
			else if (speed == 3) {
				if (count < 10)
					break;
			}
			else if (speed == 4) {
				if (count < 5)
					break;
			}
			else if (speed == 5) {
				if (count < 2)
					break;
			}
			
			if (pattern == 1) {
				count = 0;
				pat1_state = pat1start;
			}
			else {
				count = 0;
				pat1_state = pat1wait;
			}
			break;
	
	}
	//action
	switch (pat1_state){
		case pat1wait: 
			break;
		case pat1start:
			PORTA = 0x0F;
			break;
		case pat1next:
			PORTA = 0xF0;
			break;
	}
}


void Pat2_Tick() {
	static int count = 0;
	//trans
	switch (pat2_state){
		case pat2wait:
			if (pattern == 2)
				pat2_state = pat2start;
			else
				pat2_state = pat2wait;
			break;
		case pat2start:
			count++;
			if (speed == 1) {
				if (count < 40)
					break;
			}
			else if (speed == 2) {
				if (count < 20)
					break;
			}
			else if (speed == 3) {
				if (count < 10)
					break;
			}
			else if (speed == 4) {
				if (count < 5)
					break;
			}
			else if (speed == 5) {
				if (count < 2)
					break;
			}

			if (pattern == 2) {
				count = 0;
				pat2_state = pat2next;
			}
			else {
				count = 0;
				pat2_state = pat2wait;
			}
			break;
		case pat2next:
			count++;
			if (speed == 1) {
				if (count < 40)
					break;
			}
			else if (speed == 2) {
				if (count < 20)
					break;
			}
			else if (speed == 3) {
				if (count < 10)
					break;
			}
			else if (speed == 4) {
				if (count < 5)
					break;
			}
			else if (speed == 5) {
				if (count < 2)
					break;
			}

			if (pattern == 2) {
				count = 0;
				pat2_state = pat2start;
			}
			else {
				count = 0;
				pat2_state = pat2wait;
			}
			break;
	}
	//action
	switch (pat2_state){
		case pat2wait:
			break;
		case pat2start:
			PORTA = 0xAA;
			break;
		case pat2next:
			PORTA = 0x55;
			break; 
		
		
	}
}

void Pat3_Tick() {
	static char pat = 0x01;
	static int count = 0;
		
	//trans
	switch (pat3_state) {
		case pat3wait:
			if (pattern == 3) {
				pat=0x80;
				pat3_state = pat3start;
			}
			else
				pat3_state = pat3wait;
			break;
		case pat3start:
			if(pattern == 3 && pat != 0x80) {
				pat3_state = pat3start;
			}
			else if(pattern == 3 && pat != 0x01) {
				pat3_state = pat3next;	
			}
			else if(pattern != 3) {
				pat3_state = pat3wait;
			}
			break;
		case pat3next:
			if(pattern == 3 && pat != 0x01) {
				pat3_state = pat3next;
			}
			else if(pattern == 3 && pat == 0x01) {
				pat3_state = pat3start;
			}
			else if(pattern != 3) {
				pat3_state = pat3wait;
			}
			break;
	}
	//action
	switch (pat3_state) {
		case pat3wait:
			break;
		case pat3start:
			count++;
			if (speed == 1) {
				if (count < 40)
				break;
			}
			else if (speed == 2) {
				if (count < 20)
				break;
			}
			else if (speed == 3) {
				if (count < 10)
				break;
			}
			else if (speed == 4) {
				if (count < 5)
				break;
			}
			else if (speed == 5) {
				if (count < 2)
				break;
			}			
			if (pat != 0x80)
				pat = pat << 1;
			PORTA = pat;
			count = 0;
			break;
		case pat3next:
			count++;
			if (speed == 1) {
				if (count < 40)
				break;
			}
			else if (speed == 2) {
				if (count < 20)
				break;
			}
			else if (speed == 3) {
				if (count < 10)
				break;
			}
			else if (speed == 4) {
				if (count < 5)
				break;
			}
			else if (speed == 5) {
				if (count < 2)
				break;
			}
			if (pat != 0x01)
				pat = pat >> 1;
			PORTA = pat;
			count = 0;
			break;
	}
}

void Pat4_Tick() {
	static int count = 0;
	//trans
	switch (pat4_state){
		case pat4wait:
			if (pattern == 4)
				pat4_state = pat4start;
			else
				pat4_state = pat4wait;
			break;
		case pat4start:
			count++;
			if (speed == 1) {
				if (count < 40)
					break;
			}
			else if (speed == 2) {
				if (count < 20)
					break;
			}
			else if (speed == 3) {
				if (count < 10)
					break;
			}
			else if (speed == 4) {
				if (count < 5)
					break;
			}
			else if (speed == 5) {
				if (count < 2)
					break;
			}

			if (pattern == 4) {
				count = 0;
				pat4_state = pat4next;
			}
			else {
				count = 0;
				pat4_state = pat4wait;
			}
			break;
		case pat4next:
			count++;
			if (speed == 1) {
				if (count < 40)
				break;
			}
			else if (speed == 2) {
				if (count < 20)
				break;
			}
			else if (speed == 3) {
				if (count < 10)
				break;
			}
			else if (speed == 4) {
				if (count < 5)
				break;
			}
			else if (speed == 5) {
				if (count < 2)
				break;
			}
	
			if (pattern == 4) {
				count = 0;
				pat4_state = pat4start;
			}
			else {
				count = 0;
				pat4_state = pat4wait;
			}
			break;
	}
	//action
	switch (pat4_state){
		case pat4wait:
			break;
		case pat4start:
			PORTA = 0xFF;
			break;
		case pat4next:
			PORTA = 0x00;
			break;
	}
}
void ServantTask()
{
	Servant_Init();
	for(;;)
	{
		Servant_Tick();
		vTaskDelay(50);
	}
}

void Pat1Task()
{
	Pat1_Init();
	for(;;)
	{
		Pat1_Tick();
		vTaskDelay(50);
	}
}

void Pat2Task()
{
	Pat2_Init();
	for(;;)
	{
		Pat2_Tick();
		vTaskDelay(50);
	}
}

void Pat3Task()
{
	Pat3_Init();
	for(;;)
	{
		Pat3_Tick();
		vTaskDelay(50);
	}
}

void Pat4Task()
{
	Pat4_Init();
	for(;;)
	{
		Pat4_Tick();
		vTaskDelay(50);
	}
}

void StartSecPulse1(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(ServantTask, (signed portCHAR *)"ServantTask", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}

//patterns
void StartSecPulse2(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(Pat1Task, (signed portCHAR *)"pat1Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse3(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(Pat2Task, (signed portCHAR *)"pat2Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse4(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(Pat3Task, (signed portCHAR *)"pat3Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}
void StartSecPulse5(unsigned portBASE_TYPE Priority)
{
	xTaskCreate(Pat4Task, (signed portCHAR *)"pat4Task", configMINIMAL_STACK_SIZE, NULL, Priority, NULL );
}


int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	//Start Tasks
	//servant task
	StartSecPulse1(1);
	StartSecPulse2(1);
	StartSecPulse3(1);
	StartSecPulse4(1);
	StartSecPulse5(1);

	//RunSchedular
	vTaskStartScheduler();
	
	return 0;
}

