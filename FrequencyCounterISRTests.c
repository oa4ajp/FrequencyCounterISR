#include <stdio.h>
#include <string.h>
#include "FrequencyCounterISRTests.h"
#include <stdlib.h>

#define MAXBUF 10
#define MAXBUFDISPLAY 13

char frequencyCharArray[MAXBUF + 1];
char frequencyToDisplayCharArray[MAXBUFDISPLAY + 1];

void serialSend(char data)
{
	printf("%c", data);
}

void serialString(const char* s)
{
    while (*s)
    {
		serialSend(*s++);
	}
}

void printFrequency(uint32_t n) 
{	
	buildFrequency(n);
	buildFrequencyToDisplay();
	serialString(frequencyToDisplayCharArray);	
}

void buildFrequencyToDisplay() 
{
	printf("F = ");	
	memset(frequencyToDisplayCharArray, 0, sizeof(frequencyToDisplayCharArray));
	sprintf(frequencyToDisplayCharArray, "%13s", frequencyCharArray);
}

void buildFrequency(uint32_t n) 
{	
	char charArray[4 + 1]; //Max size = 4 ",%03u"  3 plus the comma	
	memset(charArray, 0, sizeof(charArray));
	memset(frequencyCharArray, 0, sizeof(frequencyCharArray));

	int n2 = 0;
    int scale = 1;

    while (n >= 1000) 
	{
        n2 = n2 + scale * (n % 1000);
        n /= 1000;
        scale *= 1000;
    }
    
	sprintf(charArray, "%u", (unsigned int) n);
	memcpy(frequencyCharArray, charArray, strlen(charArray));

    while (scale != 1) 
	{
        scale /= 1000;
        n = n2 / scale;
        n2 = n2 % scale;    
		sprintf(charArray, ",%03u", (unsigned int) n);
		memcpy(frequencyCharArray + strlen(frequencyCharArray), charArray, strlen(charArray));		
    }
}

/*
 ** valToStr
 *
 * Convert a value to a string of decimal digits with optional
 * thousands separators and a null termination.
 *
 * 'val'     is the value to convert
 * 'buf'     points to a buffer in which to place the characters
 * 'bufSize' gives the size of the buffer (should be >= 14)
 * 'sepChar' is the thousands separator (if non-zero)
 *
 * The return value will be null if the buffer is too small.  Otherwise,
 * the return value will point to the most significant digit of the string.
 *
 */
char * valToStr(uint32_t val, char *buf, uint8_t bufSize, char sepChar)
{
    // validate the parameters, return null on error
    if ((bufSize < 2) || (buf == (char *)0))
        return((char *)0);

    // put a null at the end of the buffer, adjust the size
    buf += bufSize--;
    *(--buf) = '\0';

    // special case: value equal zero
    if (val == 0)
    {
        *(--buf) = '0';
    }
    else
    {
        uint8_t digCnt = 0;

        // general case: possibly multiple digits with thousands separators
        while ((val != 0) && bufSize)
        {
            // add a thousands separator every three digits
            if (sepChar && (digCnt >= 3) && (bufSize > 1))
            {
                *(--buf) = sepChar;
                bufSize--;
                digCnt = 0;
            }

            // add another digit to the buffer
            *(--buf) = (char)(val % 10) | '0';
            digCnt++;
            bufSize--;

            // prepare for producing the next digit
            val /= 10;
        }
    }

    // return a pointer to the completed string
    return(buf);
}


void serialNumber(uint32_t frequency) 
{     
	printFrequency(frequency);
}
 
int main() {
	char buffer[15 + 1];
	memset(buffer, 0, sizeof(buffer));

	char bufferToDisplay[15 + 2];
	memset(bufferToDisplay, 0, sizeof(bufferToDisplay));

	//char *pointerBufferToDisplay = bufferToDisplay;

	char *pointerBufferToDisplay = malloc(17 * sizeof(char));

	//uint32_t frequency = 24999123;
	//uint32_t frequency = 1999123;
	//uint32_t frequency = 999123;
	//uint32_t frequency = 1234567899;
	//uint32_t frequency = 123456789;
	uint32_t frequency = 1;    
	//serial_number(24999123);

/*
	printFrequency(frequency); printf("\n");
	printFrequency(frequency + 10); printf("\n");
	printFrequency(frequency + 100);  printf("\n");
	printFrequency(frequency + 1000);  printf("\n");
	printFrequency(frequency + 10000);  printf("\n");
	printFrequency(frequency + 100000);  printf("\n");
	printFrequency(frequency + 1000000);  printf("\n");
	printFrequency(frequency + 10000000);  printf("\n");
	printFrequency(frequency + 100000000);  printf("\n");
	printFrequency(frequency + 1000000000);  printf("\n");
*/
	//serialString(valToStr(frequency, buffer, 16, '.'));
	//serialString(bufferToDisplay);

	memset(buffer, 0, sizeof(buffer));
	pointerBufferToDisplay = valToStr(frequency, buffer, 16, '.');
	serialString(pointerBufferToDisplay);
	printf("\n");

	frequency += 10;
	memset(buffer, 0, sizeof(buffer));
	pointerBufferToDisplay = valToStr(frequency, buffer, 16, '.');
	serialString(pointerBufferToDisplay);
	printf("\n");

	frequency += 100;
	memset(buffer, 0, sizeof(buffer));
	pointerBufferToDisplay = valToStr(frequency, buffer, 16, '.');
	serialString(pointerBufferToDisplay);

    return 0;
}