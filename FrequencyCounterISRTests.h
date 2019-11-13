#ifndef FrequencyCounterISRTests
#define FrequencyCounterISRTests

void serialNumber(uint32_t  val);
void serialSend(char data);
void serialString(const char* s);
void buildFrequencyToDisplay();
void buildFrequency(uint32_t n);
void printFrequency(uint32_t n);
char * valToStr(uint32_t val, char *buf, uint8_t bufSize, char sepChar);
#endif