#include <avr/io.h>
#include "Serial.h"

#define USART_BAUDRATE 9600
#define UBRR_VALUE ( ((F_CPU/(USART_BAUDRATE*16UL))) - 1 )

void serialInit(void)
{
	// initialize USART (must call this before using it)
	UBRR0 = UBRR_VALUE; // set baud rate
	UCSR0B |= (1 << TXEN0); //enable transmission only
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ01); // no parity, 1 stop bit, 8-bit data
}

void serialSend(unsigned char data)
{
	// send a single character via USART
	while( !(UCSR0A & (1 << UDRE0)) ) { }; //wait while previous byte is completed
	UDR0 = data; // Transmit data
}

void serialBreak(void)
{
	serialSend(10); // new line 
	serialSend(13); // carriage return
}

void serialString(const char* s)
{
    while (*s)
    {
		serialSend(*s++);
	}
}

void serialComma(void)
{
	serialSend(','); // comma
	serialSend(' '); // space
}

void serialSendThreeDigit(int val)
{
    int divby = 100;
    while (divby >= 1)
    {
        serialSend('0' + val / divby);
        val -= (val / divby) * divby;
        divby /= 10;
    }
}

void serialTest(void)
{
	char i;
	serialBreak();
	for (i=65; i<(65+26); i++)
    {
		serialSend(i);
	}
	serialBreak();
}
