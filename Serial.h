#ifndef SERIAL_H
#define SERIAL_H

void serialInit(void);
void serialSend(unsigned char data);
void serialBreak(void);
void serialString(const char* s);
void serialComma(void);
void serialSendThreeDigit(int val);
void serialTest(void);

#endif