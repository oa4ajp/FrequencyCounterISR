/*
This program interfacs an ATMega328p with an SN74LV8154N (32-bit counter). 
No interrupts are being used here. We expect the RCLK pin to handle the gating.
Output is [count, difference] via serial protocol. 19200 baud, 8-bit, no parity.
Connect the Atmega328 INT0 pin to the RCLK of the SN74LV8154
Compile with WinAVR

Map of Pins:
************
SN74LV8154N | Atmega328P
RCLK				| PD2
GAL					| PD5
GAU					| PD6
GBL					| PD7
GBU					| PB0
Y0					| PC5
Y1					| PC4
Y2					| PC3
Y3					| PC2
Y4					| PC1
Y5					| PC0
Y6					| PB2
Y7					| PB1

*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h> /* memset */
#include "Serial.h"
#include "FrequencyManager.h"

volatile char gateCycled = 0;

void setRegister(char registerNumber)
{    	
    // DDRB: Data Direction register for port "B" (Define input(0) or output(1))
    DDRB |= 0b00000001; // set B0 as outputs 
    DDRD |= 0b11100000; // set PD5 PD6 PD7 as outputs, PD2 remains as input   	

    switch (registerNumber)
    {
        case (3):
            // GBU            
            PORTB = 0b00000000;
            PORTD = 0b11100100; //Also Set Pull up to PD2 INt0
            break;        
        case (2):
            // GBL
            PORTB = 0b00000001;
            PORTD = 0b01100100; //Also Set Pull up to PD2 INt0
            break;
        case (1):
            // GAU
            PORTB = 0b00000001;
            PORTD = 0b10100100; //Also Set Pull up to PD2 INt0
            break;
        case (0):
            // GAL
            PORTB = 0b00000001;
            PORTD = 0b11000100; //Also Set Pull up to PD2 INt0
            break;
    }
}

int readRegister(char registerNumber)
{
    setRegister(registerNumber);

    // set inputs
    DDRC &= ~(1 << PC5);
    DDRC &= ~(1 << PC4);
    DDRC &= ~(1 << PC3);
    DDRC &= ~(1 << PC2);
    DDRC &= ~(1 << PC1);
    DDRC &= ~(1 << PC0);
    DDRB &= ~(1 << PB2);
    DDRB &= ~(1 << PB1);

    int value = 0;

    if ((PINC & (1 << PC5)))
        value |= 0b00000001;

    if ((PINC & (1 << PC4)))
        value |= 0b00000010;

    if ((PINC & (1 << PC3)))
        value |= 0b00000100;

    if ((PINC & (1 << PC2)))
        value |= 0b00001000;

    if ((PINC & (1 << PC1)))
        value |= 0b00010000;

    if ((PINC & (1 << PC0)))
        value |= 0b00100000;

    if ((PINB & (1 << PB2)))
        value |= 0b01000000;

    if ((PINB & (1 << PB1)))
        value |= 0b10000000;

    return value;
}

uint32_t readCount(void)
{
    uint32_t value = 0;

    // GBU
    value += readRegister(3);
    value = value << 8;

    // GBL
    value += readRegister(2);
    value = value << 8;

    // GAU
    value += readRegister(1);
    value = value << 8;

    // GAL
    value += readRegister(0);

    return value;
}

void timerInitialize(void)
{
	// INT0 is pin PD02
    DDRD &= ~(1 << DDD2); // Clear the PD2 pin
    // PD2 (PCINT0 pin) is now an input

    PORTD |= (1 << PORTD2);    // turn On the Pull-up
    // PD2 is now an input with pull-up enabled    
        
    EICRA  = 0b00000010;  // The falling edge of INTx generates an interrupt request (To alway get the latest value from the register before it is changed in the next rising edge)
    EIMSK  = 0b00000001;  // Turns on INT0
	
    sei(); // enable global interrupts
}

ISR (INT0_vect)
{		
	gateCycled = 1;
}

int main(void)
{	  
	// count reads: PC5,PC4,PC3,PC2,PC1,PC0,PB2,PB1
	// INPUTS NOT OUTPUTS
	
	serialInit();
    timerInitialize();
	serialString("Frequency Counter");
	serialBreak();  

    uint32_t countLast = 0;
	uint32_t countNow;
	uint32_t countDiff;
    
	while(1)
    {			
        if (gateCycled)
		{	            
            cli();            
            countNow = readCount();
            
            if(countLast > 0) 
            {
                countDiff = countNow - countLast;
            }
            else
            {
               countDiff = 0; 
            }

            serialString("C = ");
            buildFrequency(countNow, 10);        
            serialString(", F = ");                                         
            buildFrequency(countDiff, 9); // send the difference
            serialBreak();                                        

            countLast = countNow;
            gateCycled = 0;
            sei();
        }
    }
}