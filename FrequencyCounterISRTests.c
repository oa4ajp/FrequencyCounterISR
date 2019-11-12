#include <stdio.h>
#include <string.h>
#include "FrequencyCounterISRTests.h"
 
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

void serialNumber(uint32_t frequency) 
{     
	printFrequency(frequency);
}
 
int main() {
	//uint32_t frequency = 24999123;
	//uint32_t frequency = 1999123;
	//uint32_t frequency = 999123;
	//uint32_t frequency = 1234567899;
	//uint32_t frequency = 123456789;
	uint32_t frequency = 1;    
	//serial_number(24999123);	

	//printFrequency(frequency);

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

    return 0;
}