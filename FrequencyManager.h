#ifndef FREQUENCYMANAGER_H
#define FREQUENCYMANAGER_H

#include <avr/io.h>

void buildFrequency(uint32_t frequency, uint8_t numOfDigitsToDisplay);
char *unsignedLongToChar(uint32_t val, char *buffer, uint8_t numOfDigitsToDisplay);

#endif