#include "FrequencyManager.h"
#include "Serial.h"
#include <string.h> /* memset */

#define MAXBUF 13

void buildFrequency(uint32_t frequency, uint8_t numOfDigitsToDisplay) 
{   
    char frequencyCharArray[MAXBUF + 1];
    memset(frequencyCharArray, 0, sizeof(frequencyCharArray));
	char *pRespone;

	pRespone = unsignedLongToChar(frequency, frequencyCharArray, numOfDigitsToDisplay);
	
	serialString(pRespone);  
}

/*
	Length of parameter Buffer >= 14
*/
char *unsignedLongToChar(uint32_t val, char *buffer, uint8_t numOfDigitsToDisplay)
{
	const uint8_t bufferSize = 13; 
    char *p = buffer + bufferSize;
    *p = '\0';

    do 
    {						
        if ((p - buffer) % 4 == 2)
		{
            *--p = '.';
		}

        *--p = '0' + val % 10;
        val /= 10;
		--numOfDigitsToDisplay;  

    } while (numOfDigitsToDisplay);
    
    return p;
}