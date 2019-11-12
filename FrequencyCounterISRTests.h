#ifndef FrequencyCounterISRTests
#define FrequencyCounterISRTests

void serial_number(uint32_t  val);
void serial_send(char data);
void serial_string(const char* s);
void buildFrequencyToDisplay();
void buildFrequency(uint32_t n);
void printFrequency(uint32_t n);

#endif