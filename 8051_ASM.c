/*includes*/
#include <stdint.h>/*uint<X>_t*/
#include <stdlib.h>/*malloc*/
#include <stdbool.h>/*bool*/
#include <string.h>/*memcpy*/

#include "8051_ASM.h"
#include "API_header.h"
#include "8051_ADRESSES.h"
#include "8051_OPCODE_2_C-COMMAND.h"

typedef void (*exec_func)();

#define bit_address(X,n) ((X>>(n))&0x01)
#define bit_set(X,n) X|=(0x01<<(n))
#define bit_clr(X,n) X&=(X|(~X))^(0x01<<(n))
#define bit_not(X,n) X^=(0x01<<(n))
#define bit_clr_8(X,n) X&=0xFF^(0x01<<(n))

API_HIDE_var uint8_t EEPROM[EEPROM_SIZE];
API_HIDE_var uint8_t extPort[4] = { 0x00,0x00,0x00,0x00 };
API_HIDE_var uint8_t RAM[RAM_SIZE+SFR_SIZE];
API_HIDE_var uint8_t *SFR=&RAM[SFR_SIZE];
API_HIDE_var uint16_t pc = 0x0000;
API_HIDE_var bool run;

/*EMULATOR ALU*/
API_HIDE_var uint16_t temp = 0;
#define temp2 *(uint8_t *)(&temp)
#define carry ((uint8_t *)(&temp))[1]
#define hcarry (temp & 0x10)
#define overflow (temp & 0x80)

API_HIDE_var uint8_t *R = &RAM[0];
#define tmod SFR[TMOD]
#define tcon SFR[TCON]
#define stack SFR[stack_pointer]
#define psw SFR[PSW]
#define akku SFR[AKKU]
#define register_B SFR[B_address]
#define timer0_l SFR[TIMER_0]
#define timer0_h SFR[TIMER_0 + 2]
#define timer1_l SFR[TIMER_1]
#define timer1_h SFR[TIMER_1 + 2]
API_HIDE_var uint8_t interrupt_flags = 0;

API_HIDE void time(uint8_t wait) 
{
	for (;wait > 0;wait--) {
		if (tcon & 0x04) {
			switch (tmod & 0x07) {
			case 0x01:
				if (++timer0_l == 0) {
					if (++timer0_h == 0) {
						if (get_Bit_SFR(T0INT_FLAG)) { bit_set(interrupt_flags, 1); }
					}
				}
				break;
			case 0x02:
				if (++timer0_l == 0) {
					timer0_l = timer0_h;
					if (get_Bit_SFR(T0INT_FLAG)) { bit_set(interrupt_flags, 1); }
				}
				break;
			case 0x03:
				if (++timer0_l == 0) {
					if (get_Bit_SFR(T0INT_FLAG)) { bit_set(interrupt_flags, 1); }
				}
				if (++timer0_h == 0) {
					if (get_Bit_SFR(T0INT_FLAG)) { bit_set(interrupt_flags, 1); }
				}
				break;
			}
		}
		if (tcon & 0x40) {
			switch (tmod & 0x70) {
			case 0x10:
				if (++timer1_l == 0) {
					if (++timer1_h == 0) {
						if (get_Bit_SFR(T1INT_FLAG)) { bit_set(interrupt_flags, 3); }
					}
				}
				break;
			case 0x20:
				if (++timer1_l == 0) {
					timer1_l = timer0_h;
					if (get_Bit_SFR(T1INT_FLAG)) { bit_set(interrupt_flags, 3); }
				}
				break;
			case 0x30:
				if (++timer1_l == 0) {
					if (get_Bit_SFR(T1INT_FLAG)) { bit_set(interrupt_flags, 3); }
				}
				if (++timer1_h == 0) {
					if (get_Bit_SFR(T1INT_FLAG)) { bit_set(interrupt_flags, 3); }
				}
				break;
			}
		}
	}
}
/*Create interpreter functions*/
#define ASM_COMMAND(opcode,command) API_HIDE void func_##opcode(){command; }
#define seperator
#include "8051_ASM_list.h"
#undef seperator
#undef ASM_COMMAND

/*Functionpointer Array of all functionpointers of the interpreter*/
API_HIDE_var const exec_func pointers[256]={
#define ASM_COMMAND(opcode,command) func_##opcode
#define seperator ,
#include "8051_ASM_list.h"
#undef seperator
#undef ASM_COMMAND
};

API_SHOW void write_EEPROM(uint16_t address, uint8_t *data, uint32_t lenght) 
{
	for (lenght += address;address<lenght;address++, data++) {
		EEPROM[address] = *data;
	}
}
API_SHOW void write_RAM(uint8_t address, uint8_t *data, uint16_t lenght) 
{
	for (lenght += address;address<lenght;address++, data++) {
		RAM[address] = *data;
	}
}
API_SHOW void stop_interpreter()
{
	run=false;
}

API_SHOW void reset() {/*Initialization of the SFR*/
	stop_interpreter();
	SFR[stack_pointer] = 0x07;
	temp=3;
	while(temp--){
		SFR[PORT(temp)] = 0xFF;
	}
}

API_SHOW uint8_t get_Port(uint8_t n) 
{
	return SFR[PORT(n)] & extPort[n];
}

API_SHOW void set_Port(uint8_t n, uint8_t value) 
{
	extPort[n] = value;
}

API_SHOW void interpret_stepping(int steps)
{
	while(steps--){
		pointers[EEPROM[pc++]]();
		if (interrupt_flags) {
			if (get_Bit(EA_FLAG)) {
				for (uint8_t a = 0;a < 5;a++) {
					if (bit_address(interrupt_flags, a)) {
						set_stack();
						pc = 0x03 + a * 0x08;
						set_Bit(INT_FLAG);
					}
				}
			}
			interrupt_flags = 0;
		}
	}
}
API_SHOW void run_interpreter()
{
	run=true;
	while(run){
		pointers[EEPROM[pc++]]();
		if (interrupt_flags) {
			if (get_Bit(EA_FLAG)) {
				for (uint8_t a = 0;a < 5;a++) {
					if (bit_address(interrupt_flags, a)) {
						set_stack();
						pc = 0x03 + a * 0x08;
						set_Bit(INT_FLAG);
					}
				}
			}
			interrupt_flags = 0;
		}
	}
}


API_SHOW uint8_t* dump_RAM(uint8_t address, uint8_t lenght,bool sfr){
	uint8_t* ptr=malloc(lenght);
	if(sfr){
		if(address>RAM_SIZE-SFR_SIZE)
			memcpy(ptr,&SFR[address],lenght);
		else{
			memcpy(ptr,&RAM[address],RAM_SIZE-address);
			memcpy(ptr+RAM_SIZE-address,&SFR[RAM_SIZE-SFR_SIZE],lenght+address-RAM_SIZE);
		}
	}
	else memcpy(ptr,&RAM[address],lenght);
	return ptr;
}

API_SHOW uint8_t* dump_EEPROM(uint16_t address, uint16_t lenght){
	uint8_t* ptr=malloc(lenght);
	memcpy(ptr,&EEPROM[address],lenght%(EEPROM_SIZE-address));
}

API_SHOW uint16_t get_pc(){
	return pc;
}
