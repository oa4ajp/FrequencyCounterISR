/*
This program interfacs an ATMega328p with an SN74LV8154N (32-bit counter). 
No interrupts are being used here. We expect the RCLK pin to handle the gating.
Output is [count, difference] via serial protocol. 19200 baud, 8-bit, no parity.
Connect the Atmega328 INT0 pin to the RCLK of the SN74LV8154

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

#define F_CPU 16000000ul
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define USART_BAUDRATE 9600
#define UBRR_VALUE (((F_CPU/(USART_BAUDRATE*16UL)))-1)

volatile int millisecPerUpdate;
volatile char gateCycled = 0;
volatile int seconds = 0;
volatile int milliseconds = 0;

void serial_init()
{
	// initialize USART (must call this before using it)
	UBRR0 = UBRR_VALUE; // set baud rate
	UCSR0B |= (1 << TXEN0); //enable transmission only
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serial_send(unsigned char data)
{
	// send a single character via USART
	while(!(UCSR0A & (1 << UDRE0))){}; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

void serial_string(const char* s)
{
    while (*s)
    {
		serial_send(*s++);
	}
}

void serial_break()
{
	serial_send(10); // new line 
	serial_send(13); // carriage return
}

void serial_comma()
{
	serial_send(','); // comma
	serial_send(' '); // space
}

void SerialSend_intThreeDigit(int val)
{
    int divby = 100;
    while (divby >= 1)
    {
        serial_send('0' + val / divby);
        val -= (val / divby) * divby;
        divby /= 10;
    }
}

// send a number as ASCII text
void serial_number(uint32_t  val) 
{     
	uint32_t divby = 1000000000; // change by dataType 1 cycles per sec (Default)

	while (divby >= 1)
    {
		serial_send('0' + val/divby);
		val -= (val/divby) * divby;
		divby /= 10;
	}
}

char setRegister(char registerNumber)
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

uint32_t readCount()
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

void serial_test()
{
	char i;
	serial_break();
	for (i=65; i<(65+26); i++)
    {
		serial_send(i);
	}
	serial_break();
}

void TimerInitialize()
{
	// INT0 is pin PD02    	
    int updatesPerSecond = 1;    
	millisecPerUpdate = 1000 / updatesPerSecond;

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
    milliseconds += millisecPerUpdate;
	if (milliseconds >= 1000)
	{
		milliseconds = 0;
		seconds += 1;
		if (seconds >= 1000)
			seconds = 0;
	}
    
	gateCycled = 1;
}

int main(void)
{	  
	// count reads: PC5,PC4,PC3,PC2,PC1,PC0,PB2,PB1
	// INPUTS NOT OUTPUTS
	
	serial_init();
    TimerInitialize();
	serial_string("Frequency Counter");
	serial_break();  

    uint32_t countLast;
	uint32_t countNow;
	uint32_t countDiff;

	while(1)
    {			
        if (gateCycled)
		{	            
            cli();            
            countNow = readCount();
            countDiff = countNow - countLast;
            SerialSend_intThreeDigit(seconds);
			serial_comma();
			SerialSend_intThreeDigit(milliseconds);
			serial_comma();
            serial_string("C = ");
            serial_number(countNow);
            serial_string(", F = ");                    
            serial_number(countDiff); // send the difference                        
            serial_break();                                        

            countLast = countNow;
            gateCycled = 0;
            sei();
        }
    }
}