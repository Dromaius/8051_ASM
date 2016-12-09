#include "8051_ADRESSES.h"
#include "OPCODE_2_C-COMMAND.h"


#define bit_address(X,n) ((X>>(n))&0x01)
#define bit_set(X,n) X|=(0x01<<(n))
#define bit_clr(X,n) X&=(X|(~X))^(0x01<<(n))
#define bit_not(X,n) X^=(0x01<<(n))
#define bit_clr_8(X,n) X&=0xFF^(0x01<<(n))

#ifdef __WIN32
#define API_HIDE 
#define API_SHOW __attribute__ ((dllexport))
#else
#define API_HIDE __attribute__ ((visibility ("hidden")))
#define API_SHOW __attribute__ ((visibility ("default")))
#endif

API_HIDE uint8_t EEPROM[EEPROM_SIZE];
API_HIDE uint8_t extPort[4] = { 0x00,0x00,0x00,0x00 };
API_HIDE uint8_t RAM[RAM_SIZE+SFR_SIZE];
API_HIDE uint8_t *SFR=&RAM[SFR_SIZE];
API_HIDE uint16_t pc = 0x0000;
API_HIDE bool run;

//EMULATOR ALU
API_HIDE uint16_t temp = 0;
#define temp2 *(uint8_t *)(&temp)
#define carry ((uint8_t *)(&temp))[1]
#define hcarry (temp & 0x10)
#define overflow (temp & 0x80)

API_HIDE uint8_t *R = &RAM[(SFR[PSW] & 0x18) >> 3];
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
API_HIDE uint8_t interrupt_flags = 0;


#define ASM_COMMAND(opcode,command) API_HIDE void func_##opcode {command; }
#define ASM_Rn(opcode_4,command) ASM_COMMAND(##opcode_4 _0x08,command(0)) ASM_COMMAND(##opcode_4 _0x09,command(1)) ASM_COMMAND(##opcode_4 _0x0A,command(2)) ASM_COMMAND(##opcode_4 _0x0B,command(3)) ASM_COMMAND(##opcode_4 _0x0C,command(4)) ASM_COMMAND(##opcode_4 _0x0D,command(5)) ASM_COMMAND(##opcode_4 _0x0E,command(6)) ASM_COMMAND(##opcode_4 _0x0F,command(7))
#define ASM_Ri(opcode_4,command) ASM_COMMAND(##opcode_4 _0x06,command(0)) ASM_COMMAND(##opcode_4 _0x07,command(1))
#define ASM_Ri_Custom(opcode_7,command) ASM_COMMAND(##opcode_7 _0x00,command(0)) ASM_COMMAND(##opcode_7 _0x01,command(1))

ASM_COMMAND(0x00, NOP)
ASM_COMMAND(0x01, AJMP_ADDR11_(0))
ASM_COMMAND(0x02, LJMP_ADDR16)
ASM_COMMAND(0x03, RR_A)
ASM_COMMAND(0x04, INC_A)
ASM_COMMAND(0x05, INC_DADR)
ASM_Ri(0x00, INC_atRi)
ASM_Rn(0x00, INC_Rn)

ASM_COMMAND(0x10, JBC_BADR_REL)
ASM_COMMAND(0x11, ACALL_ADDR11_(0))
ASM_COMMAND(0x12, LCALL_ADDR16)
ASM_COMMAND(0x13, RRC_A)
ASM_COMMAND(0x14, DEC_A)
ASM_COMMAND(0x15, DEC_DADR)
ASM_Ri(0x10, DEC_atRi)
ASM_Rn(0x10, DEC_Rn)

ASM_COMMAND(0x20, JB_BADR_REL) 
ASM_COMMAND(0x21, AJMP_ADDR11_(1)) 
ASM_COMMAND(0x22, RET) 
ASM_COMMAND(0x23, RL_A) 
ASM_COMMAND(0x24, ADD_A_hC8) 
ASM_COMMAND(0x25, ADD_A_DADR) 
ASM_Ri(0x20, ADD_A_atRi) 
ASM_Rn(0x20, ADD_A_Rn) 

ASM_COMMAND(0x30, JNB_BADR_REL) 
ASM_COMMAND(0x31, ACALL_ADDR11_(1)) 
ASM_COMMAND(0x32, RETI) 
ASM_COMMAND(0x33, RLC_A) 
ASM_COMMAND(0x34, ADDC_A_hC8) 
ASM_COMMAND(0x35, ADDC_A_DADR) 
ASM_Ri(0x30, ADDC_A_atRi) 
ASM_Rn(0x30, ADDC_A_Rn) 

ASM_COMMAND(0x40, JC_REL) 
ASM_COMMAND(0x41, AJMP_ADDR11_(2)) 
ASM_COMMAND(0x42, ORL_DADR_A) 
ASM_COMMAND(0x43, ORL_DADR_hC8) 
ASM_COMMAND(0x44, ORL_A_hC8) 
ASM_COMMAND(0x45, ORL_A_DADR) 
ASM_Ri(0x40, ORL_A_atRi) 
ASM_Rn(0x40, ORL_A_Rn) 

ASM_COMMAND(0x50, JNC_REL) 
ASM_COMMAND(0x51, ACALL_ADDR11_(2)) 
ASM_COMMAND(0x52, ANL_DADR_A) 
ASM_COMMAND(0x53, ANL_DADR_hC8) 
ASM_COMMAND(0x54, ANL_A_hC8) 
ASM_COMMAND(0x55, ANL_A_DADR) 
ASM_Ri(0x50, ANL_A_atRi) 
ASM_Rn(0x50, ANL_A_Rn) 

ASM_COMMAND(0x60, JZ_REL) 
ASM_COMMAND(0x61, AJMP_ADDR11_(3)) 
ASM_COMMAND(0x62, XRL_DADR_A) 
ASM_COMMAND(0x63, XRL_DADR_hC8) 
ASM_COMMAND(0x64, XRL_A_hC8) 
ASM_COMMAND(0x65, XRL_A_DADR) 
ASM_Ri(0x60, XRL_A_atRi) 
ASM_Rn(0x60, XRL_A_Rn) 

ASM_COMMAND(0x70, JNZ_REL) 
ASM_COMMAND(0x71, ACALL_ADDR11_(3)) 
ASM_COMMAND(0x72, ORL_C_BADR) 
ASM_COMMAND(0x73, JMP_atAplusDPTR) 
ASM_COMMAND(0x74, MOV_A_hC8) 
ASM_COMMAND(0x75, MOV_DADR_hC8) 
ASM_Ri(0x70, MOV_atRi_hC8) 
ASM_Rn(0x70, MOV_Rn_hC8) 

ASM_COMMAND(0x80, SJUMP_REL) 
ASM_COMMAND(0x81, AJMP_ADDR11_(4)) 
ASM_COMMAND(0x82, ANL_C_BADR) 
ASM_COMMAND(0x83, MOVC_A_atAlpusPC) 
ASM_COMMAND(0x84, DIV_AB) 
ASM_COMMAND(0x85, MOV_DADR_DADR) 
ASM_Ri(0x80, MOV_DADR_atRi) 
ASM_Rn(0x80, MOV_DADR_Rn) 

ASM_COMMAND(0x90, SJUMP_REL) 
ASM_COMMAND(0x91, ACALL_ADDR11_(4)) 
ASM_COMMAND(0x92, MOV_BADR_C) 
ASM_COMMAND(0x93, MOVC_A_atAlpusDPTR) 
ASM_COMMAND(0x94, SUBB_A_hC8) 
ASM_COMMAND(0x95, SUBB_A_DADR) 
ASM_Ri(0x90, SUBB_A_atRi) 
ASM_Rn(0x90, SUBB_A_Rn) 

ASM_COMMAND(0xA0, ORL_C_nBADR) 
ASM_COMMAND(0xA1, AJMP_ADDR11_(4)) 
ASM_COMMAND(0xA2, MOV_C_BADR) 
ASM_COMMAND(0xA3, INC_DPTR) 
ASM_COMMAND(0xA4, MUL_AB) 
ASM_COMMAND(0xA5, reserviert) 
ASM_Ri(0xA0, MOV_atRi_DADR) 
ASM_Rn(0xA0, MOV_Rn_DADR) 

ASM_COMMAND(0xB0, ANL_C_nBADR) 
ASM_COMMAND(0xB1, ACALL_ADDR11_(5)) 
ASM_COMMAND(0xB2, CPL_BADR) 
ASM_COMMAND(0xB3, CPL_C) 
ASM_COMMAND(0xB4, CJNE_A_hC8_REL) 
ASM_COMMAND(0xB5, CJNE_A_DADR_REL) 
ASM_Ri(0xB0, CJNE_atRi_hC8_REL) 
ASM_Rn(0xB0, CJNE_Rn_hC8_REL) 

ASM_COMMAND(0xC0, PUSH_DADR) 
ASM_COMMAND(0xC1, AJMP_ADDR11_(6)) 
ASM_COMMAND(0xC2, CLR_BADR) 
ASM_COMMAND(0xC3, CLR_C) 
ASM_COMMAND(0xC4, SWAP_A) 
ASM_COMMAND(0xC5, XCH_A_DADR) 
ASM_Ri(0xC0, XCH_A_atRi) 
ASM_Rn(0xC0, XCH_A_atRn) 

ASM_COMMAND(0xD0, POP_DADR) 
ASM_COMMAND(0xD1, ACALL_ADDR11_(6)) 
ASM_COMMAND(0xD2, SETB_BADR) 
ASM_COMMAND(0xD3, SETB_C) 
ASM_COMMAND(0xD4, DA_A) 
ASM_COMMAND(0xD5, DJNZ_DADR_REL) 
ASM_Ri(0xD0, XCHD_A_atRi) 
ASM_Rn(0xD0, DJNZ_Rn_REL) 

ASM_COMMAND(0xE0, MOVX_A_atDPTR) 
ASM_COMMAND(0xE1, AJMP_ADDR11_(7)) 
ASM_Ri_Custom(0xE2, MOVX_A_atRi) 
ASM_COMMAND(0xE4, CLR_A) 
ASM_COMMAND(0xE5, MOV_A_DADR) 
ASM_Ri(0xE0, MOV_A_atRi) 
ASM_Rn(0xE0, MOV_A_Rn) 

ASM_COMMAND(0xF0, MOVX_atDPTR_A) 
ASM_COMMAND(0xF1, ACALL_ADDR11_(7)) 
ASM_Ri_Custom(0xF2, MOVX_atRi_A) 
ASM_COMMAND(0xF4, CPL_A) 
ASM_COMMAND(0xF5, MOV_DADR_A) 
ASM_Ri(0xF0, MOV_atRi_A) 
ASM_Rn(0xF0, MOV_Rn_A) 

#undef ASM_Rn(opcode_4,command)
#undef ASM_Ri(opcode_4,command)
#undef ASM_Ri_Custom(opcode_7,command)
#undef ASM_COMMAND(opcode,command)

#define NAME_COMMAND(opcode) func_##opcode
#define NAME_Rn(opcode_4) NAME_COMMAND(##opcode_4 _0x08), NAME_COMMAND(##opcode_4 _0x09), NAME_COMMAND(##opcode_4 _0x0A), NAME_COMMAND(##opcode_4 _0x0B), NAME_COMMAND(##opcode_4 _0x0C), NAME_COMMAND(##opcode_4 _0x0D), NAME_COMMAND(##opcode_4 _0x0E), NAME_COMMAND(##opcode_4 _0x0F)
#define NAME_Ri(opcode_4) NAME_COMMAND(##opcode_4 _0x06), NAME_COMMAND(##opcode_4 _0x07)
#define NAME_Ri_Custom(opcode_7) NAME_COMMAND(##opcode_7 _0x00), NAME_COMMAND(##opcode_7 _0x01)

API_HIDE const void (const (*pointers)())[256]={
NAME_COMMAND(0x00),
NAME_COMMAND(0x01),
NAME_COMMAND(0x02),
NAME_COMMAND(0x03),
NAME_COMMAND(0x04),
NAME_COMMAND(0x05),
NAME_Ri(0x00),
NAME_Rn(0x00),

NAME_COMMAND(0x10),
NAME_COMMAND(0x11),
NAME_COMMAND(0x12),
NAME_COMMAND(0x13),
NAME_COMMAND(0x14),
NAME_COMMAND(0x15),
NAME_Ri(0x10),
NAME_Rn(0x10),

NAME_COMMAND(0x20),
NAME_COMMAND(0x21),
NAME_COMMAND(0x22),
NAME_COMMAND(0x23),
NAME_COMMAND(0x24),
NAME_COMMAND(0x25),
NAME_Ri(0x20),
NAME_Rn(0x20),

NAME_COMMAND(0x30),
NAME_COMMAND(0x31),
NAME_COMMAND(0x32),
NAME_COMMAND(0x33),
NAME_COMMAND(0x34),
NAME_COMMAND(0x35),
NAME_Ri(0x30),
NAME_Rn(0x30),

NAME_COMMAND(0x40),
NAME_COMMAND(0x41),
NAME_COMMAND(0x42),
NAME_COMMAND(0x43),
NAME_COMMAND(0x44),
NAME_COMMAND(0x45),
NAME_Ri(0x40),
NAME_Rn(0x50),

NAME_COMMAND(0x50),
NAME_COMMAND(0x51),
NAME_COMMAND(0x52),
NAME_COMMAND(0x53),
NAME_COMMAND(0x54),
NAME_COMMAND(0x55),
NAME_Ri(0x50),
NAME_Rn(0x50),

NAME_COMMAND(0x60),
NAME_COMMAND(0x61),
NAME_COMMAND(0x62),
NAME_COMMAND(0x63),
NAME_COMMAND(0x64),
NAME_COMMAND(0x65),
NAME_Ri(0x60),
NAME_Rn(0x60),

NAME_COMMAND(0x70),
NAME_COMMAND(0x71),
NAME_COMMAND(0x72),
NAME_COMMAND(0x73),
NAME_COMMAND(0x74),
NAME_COMMAND(0x75),
NAME_Ri(0x70),
NAME_Rn(0x70),

NAME_COMMAND(0x80),
NAME_COMMAND(0x81),
NAME_COMMAND(0x82),
NAME_COMMAND(0x83),
NAME_COMMAND(0x84),
NAME_COMMAND(0x85),
NAME_Ri(0x80),
NAME_Rn(0x80),

NAME_COMMAND(0x90),
NAME_COMMAND(0x91),
NAME_COMMAND(0x92),
NAME_COMMAND(0x93),
NAME_COMMAND(0x94),
NAME_COMMAND(0x95),
NAME_Ri(0x90),
NAME_Rn(0x90),

NAME_COMMAND(0xA0),
NAME_COMMAND(0xA1),
NAME_COMMAND(0xA2),
NAME_COMMAND(0xA3),
NAME_COMMAND(0xA4),
NAME_COMMAND(0xA5),
NAME_Ri(0xA0),
NAME_Rn(0xA0),

NAME_COMMAND(0xB0),
NAME_COMMAND(0xB1),
NAME_COMMAND(0xB2),
NAME_COMMAND(0xB3),
NAME_COMMAND(0xB4),
NAME_COMMAND(0xB5),
NAME_Ri(0xB0),
NAME_Rn(0xB0),

NAME_COMMAND(0xC0),
NAME_COMMAND(0xC1),
NAME_COMMAND(0xC2),
NAME_COMMAND(0xC3),
NAME_COMMAND(0xC4),
NAME_COMMAND(0xC5),
NAME_Ri(0xC0),
NAME_Rn(0xC0),

NAME_COMMAND(0xD0),
NAME_COMMAND(0xD1),
NAME_COMMAND(0xD2),
NAME_COMMAND(0xD3),
NAME_COMMAND(0xD4),
NAME_COMMAND(0xD5),
NAME_Ri(0xD0),
NAME_Rn(0xD0),

NAME_COMMAND(0xE0),
NAME_COMMAND(0xE1),
NAME_Ri_Custom(0xE2),
NAME_COMMAND(0xE4), 
NAME_COMMAND(0xE5),
NAME_Ri(0xE0), 
NAME_Rn(0xE0),

NAME_COMMAND(0xF0), 
NAME_COMMAND(0xF1),
NAME_Ri_Custom(0xF2), 
NAME_COMMAND(0xF4), 
NAME_COMMAND(0xF5), 
NAME_Ri(0xF0), 
NAME_Rn(0xF0) 
}

#undef NAME_Rn(opcode_4)
#undef NAME_Ri(opcode_4)
#undef NAME_Ri_Custom(opcode_7)
#undef NAME_COMMAND(opcode)

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
API_HIDE char Int2Char(uint8_t parse) 
{
    if(0 <=parse <= 9) return parse + '0';
	else if(0x0A <= parse <= 0x0F) return parse + 'A' - 10;
	else return 0;
}
API_HIDE uint8_t Char2Int(char hex)
{
	if( '0' <= hex <= '9') return hex - '0';
	else if('A' <= hex <= 'F') return hex - 'A' + 10;
	else if( 'a' <= hex <= 'f') return hex - 'a' + 10;
	else return 255;
}

API_SHOW void set_EEPROM(uint16_t address, uint8_t *data, uint32_t lenght) 
{
	for (lenght += address;address<lenght;address++, data++) {
		EEPROM[address] = *data;
	}
}
API_SHOW void set_RAM(uint8_t address, uint8_t *data, uint16_t lenght) 
{
	for (lenght += address;address<lenght;address++, data++) {
		RAM[address] = *data;
	}
}

API_SHOW void reset() {//Initialization of the SFR
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
API_SHOW void stop_interpreter()
{
	run=false;
}

API_SHOW uint8_t* get_RAM(uint8_t address, uint8_t lenght,bool sfr){
	uint8_t* ptr=malloc(lenght);
	if(sfr){
		if(address>RAM_SIZE-SFR_SIZE)
			memcpy(ptr,&SFR[address],lenght);
		else{
			memcpy(ptr,&RAM[address],RAM_SIZE-address);
			memcpy(ptr+RAM_SIZE-address,&SFR[RAM_SIZE-SFR_SIZE],length+address-RAM_SIZE);
		}
	}
	else memcpy(ptr,&RAM[address],lenght);
	return ptr;
}

API_SHOW uint8_t* get_EEPROM(uint16_t address, uint16_t lenght){
	uint8_t* ptr=malloc(lenght);
	memcpy(ptr,&EEPROM[address],lenght%(EEPROM_SIZE-address));
}

API_SHOW uint16_t get_pc(){
	return pc;
}