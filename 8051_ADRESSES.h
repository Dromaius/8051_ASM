#define RAM_SIZE 256
#define SFR_SIZE 128
#define EEPROM_SIZE (1<<16)

//Bit
#define CARRY 0xD7
#define OF 0xD2
#define HCARRY 0xD6
#define INT_FLAG 0x00
#define EA_FLAG 0xAF
#define T0INT_FLAG 0xA9
#define T1INT_FLAG 0xAB
#define T0OF_FLAG 0x8D
#define T1OF_FLAG 0x8F
//Byte
#define AKKU 0xE0
#define B_address 0xF0
#define PSW 0xD0
#define stack_pointer 0x81
#define DTPTR 0x82
#define TCON 0x88
#define TMOD 0x89
#define TIMER_0 0x8A
#define TIMER_1 0x8B
#define PORT(a) 0x80+a*0x10
#define PORT0 0x80
#define PORT1 0x90
#define PORT2 0xA0
#define PORT3 0xB0
//Feste Adressen
