#include <cstdint>

#define RAM_SIZE 256
#define SFR_SIZE 128
#define EEPROM_SIZE (1<<16)

uint8_t EEPROM[EEPROM_SIZE];

//�C
uint8_t extPort[4] = { 0x00,0x00,0x00,0x00 };
uint8_t RAM[RAM_SIZE+SFR_SIZE];
uint8_t *SFR=&RAM[SFR_SIZE];
uint16_t pc = 0x0000;
bool run;
