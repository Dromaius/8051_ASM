#define RAM_SIZE 256
#define SFR_SIZE 128
#define EEPROM_SIZE (1<<16)

#include <iostream>

uint8_t EEPROM[EEPROM_SIZE];

//µC
uint8_t extPort[4] = { 0xFF,0xFF,0xFF,0xFF };
uint8_t RAM[RAM_SIZE+SFR_SIZE];
uint8_t *SFR=&RAM[SFR_SIZE];
uint16_t pc = 0x0000;
bool run;