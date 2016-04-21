/*
A 8051 microcontroller emulator focused on speed and efficiency and NOT on code readability.

Author:  https://github.com/Dromaius/
Licence: MIT (See file "LICENCE")
*/

//#include "stdafx.h" // Uncomment for Windows compilation
#include "ADDRESS_8051.h"
#include "ASM_8051.h"
#include "Global_Vars.h"

#include <iostream>
#include <fstream>

//BLANK
#define bit_opperation(func,address) if (address & 0x80) {func (SFR[address & 0xF8], address & 0x07);}else{func (RAM[(address >> 3)+0x20], address & 0x07);}
#define bit_address(X,n) ((X>>(n))&0x01)
#define bit_set(X,n) X|=(0x01<<(n))
#define bit_clr(X,n) X&=(X|(~X))^(0x01<<(n))
#define bit_not(X,n) X^=(0x01<<(n))
#define bit_clr_8(X,n) X&=0xFF^(0x01<<(n))


//DEBUG & MANUAL
#define DEBUG 1
#define MANUAL_STEP 1
#define debug if(DEBUG)cout

using namespace std;

//EMULATOR ALU
uint16_t temp = 0;
uint8_t &temp2 = *(uint8_t *)(&temp);
uint8_t &carry = ((uint8_t *)(&temp))[1];
uint8_t hcarry() {
	return (temp & 0x10);
}
uint8_t overflow() {
	return (temp & 0x80);
}

uint8_t *R = &RAM[(SFR[PSW] & 0x18) >> 3];
uint16_t &dtptr = *((uint16_t *)(&SFR[DTPTR]));
uint8_t &tmod = SFR[TMOD];
uint8_t &tcon = SFR[TCON];
uint8_t &stack = SFR[stack_pointer];
uint8_t &psw = SFR[PSW];
uint8_t &akku = SFR[AKKU];
uint8_t &register_B = SFR[B_address];
uint8_t &timer0_l = SFR[TIMER_0];
uint8_t &timer0_h = SFR[TIMER_0 + 2];
uint8_t &timer1_l = SFR[TIMER_1];
uint8_t &timer1_h = SFR[TIMER_1 + 2];
uint8_t interrupt_flags = 0;

bool get_Bit(uint8_t address) {
	if (address & 0x80) {
		switch (address&0xF8){
		case PORT0:
			return bit_address (SFR[PORT0] & extPort[0], address & 0x07);
		case PORT1:
			return bit_address (SFR[PORT1] & extPort[1], address & 0x07);
		case PORT2:
			return bit_address (SFR[PORT2] & extPort[2], address & 0x07);
		case PORT3:
			return bit_address (SFR[PORT3] & extPort[3], address & 0x07);
		default:
			return bit_address (SFR[address & 0xF8], address & 0x07);
		}
	}
	else{return bit_address (RAM[(address >> 3)+0x20], address & 0x07);}
	//bit_opperation(return bit_address, address);
}

uint8_t get_RAM(uint8_t address) {
	if (address & 0x80) {
		switch (address) {
		case PORT0:
			return SFR[PORT0] & extPort[0];
		case PORT1:
			return SFR[PORT1] & extPort[1];
		case PORT2:
			return SFR[PORT2] & extPort[2];
		case PORT3:
			return SFR[PORT3] & extPort[3];
		default:
			return SFR[address];
		}
	}
	else {
		return RAM[address];
	}
}

void time(uint8_t wait) {
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


void interpreter() {
	do {
		switch (EEPROM[pc++]) {
			ASM_COMMAND(0x00, NOP);
			ASM_COMMAND(0x01, AJMP_ADDR11_(0));
			ASM_COMMAND(0x02, LJMP_ADDR16);
			ASM_COMMAND(0x03, RR_A);
			ASM_COMMAND(0x04, INC_A);
			ASM_COMMAND(0x05, INC_DADR);
			ASM_Ri(0x00, INC_atRi);
			ASM_Rn(0x00, INC_Rn);

			ASM_COMMAND(0x10, JBC_BADR_REL);
			ASM_COMMAND(0x11, ACALL_ADDR11_(0));
			ASM_COMMAND(0x12, LCALL_ADDR16);
			ASM_COMMAND(0x13, RRC_A);
			ASM_COMMAND(0x14, DEC_A);
			ASM_COMMAND(0x15, DEC_DADR);
			ASM_Ri(0x10, DEC_atRi);
			ASM_Rn(0x10, DEC_Rn);

			ASM_COMMAND(0x20, JB_BADR_REL);
			ASM_COMMAND(0x21, AJMP_ADDR11_(1));
			ASM_COMMAND(0x22, RET);
			ASM_COMMAND(0x23, RL_A);
			ASM_COMMAND(0x24, ADD_A_hC8);
			ASM_COMMAND(0x25, ADD_A_DADR);
			ASM_Ri(0x20, ADD_A_atRi);
			ASM_Rn(0x20, ADD_A_Rn);

			ASM_COMMAND(0x30, JNB_BADR_REL);
			ASM_COMMAND(0x31, ACALL_ADDR11_(1));
			ASM_COMMAND(0x32, RETI);
			ASM_COMMAND(0x33, RLC_A);
			ASM_COMMAND(0x34, ADDC_A_hC8);
			ASM_COMMAND(0x35, ADDC_A_DADR);
			ASM_Ri(0x30, ADDC_A_atRi);
			ASM_Rn(0x30, ADDC_A_Rn);

			ASM_COMMAND(0x40, JC_REL);
			ASM_COMMAND(0x41, AJMP_ADDR11_(2));
			ASM_COMMAND(0x42, ORL_DADR_A);
			ASM_COMMAND(0x43, ORL_DADR_hC8);
			ASM_COMMAND(0x44, ORL_A_hC8);
			ASM_COMMAND(0x45, ORL_A_DADR);
			ASM_Ri(0x40, ORL_A_atRi);
			ASM_Rn(0x40, ORL_A_Rn);

			ASM_COMMAND(0x50, JNC_REL);
			ASM_COMMAND(0x51, ACALL_ADDR11_(2));
			ASM_COMMAND(0x52, ANL_DADR_A);
			ASM_COMMAND(0x53, ANL_DADR_hC8);
			ASM_COMMAND(0x54, ANL_A_hC8);
			ASM_COMMAND(0x55, ANL_A_DADR);
			ASM_Ri(0x50, ANL_A_atRi);
			ASM_Rn(0x50, ANL_A_Rn);

			ASM_COMMAND(0x60, JZ_REL);
			ASM_COMMAND(0x61, AJMP_ADDR11_(3));
			ASM_COMMAND(0x62, XRL_DADR_A);
			ASM_COMMAND(0x63, XRL_DADR_hC8);
			ASM_COMMAND(0x64, XRL_A_hC8);
			ASM_COMMAND(0x65, XRL_A_DADR);
			ASM_Ri(0x60, XRL_A_atRi);
			ASM_Rn(0x60, XRL_A_Rn);

			ASM_COMMAND(0x70, JNZ_REL);
			ASM_COMMAND(0x71, ACALL_ADDR11_(3));
			ASM_COMMAND(0x72, ORL_C_BADR);
			ASM_COMMAND(0x73, JMP_atAplusDPTR);
			ASM_COMMAND(0x74, MOV_A_hC8);
			ASM_COMMAND(0x75, MOV_DADR_hC8);
			ASM_Ri(0x70, MOV_atRi_hC8);
			ASM_Rn(0x70, MOV_Rn_hC8);

			ASM_COMMAND(0x80, SJUMP_REL);
			ASM_COMMAND(0x81, AJMP_ADDR11_(4));
			ASM_COMMAND(0x82, ANL_C_BADR);
			ASM_COMMAND(0x83, MOVC_A_atAlpusPC);
			ASM_COMMAND(0x84, DIV_AB);
			ASM_COMMAND(0x85, MOV_DADR_DADR);
			ASM_Ri(0x80, MOV_DADR_atRi);
			ASM_Rn(0x80, MOV_DADR_Rn);

			ASM_COMMAND(0x90, SJUMP_REL);
			ASM_COMMAND(0x91, ACALL_ADDR11_(4));
			ASM_COMMAND(0x92, MOV_BADR_C);
			ASM_COMMAND(0x93, MOVC_A_atAlpusDPTR);
			ASM_COMMAND(0x94, SUBB_A_hC8);
			ASM_COMMAND(0x95, SUBB_A_DADR);
			ASM_Ri(0x90, SUBB_A_atRi);
			ASM_Rn(0x90, SUBB_A_Rn);

			ASM_COMMAND(0xA0, ORL_C_nBADR);
			ASM_COMMAND(0xA1, AJMP_ADDR11_(4));
			ASM_COMMAND(0xA2, MOV_C_BADR);
			ASM_COMMAND(0xA3, INC_DPTR);
			ASM_COMMAND(0xA4, MUL_AB);
			ASM_COMMAND(0xA5, reserviert);
			ASM_Ri(0xA0, MOV_atRi_DADR);
			ASM_Rn(0xA0, MOV_Rn_DADR);

			ASM_COMMAND(0xB0, ANL_C_nBADR);
			ASM_COMMAND(0xB1, ACALL_ADDR11_(5));
			ASM_COMMAND(0xB2, CPL_BADR);
			ASM_COMMAND(0xB3, CPL_C);
			ASM_COMMAND(0xB4, CJNE_A_hC8_REL);
			ASM_COMMAND(0xB5, CJNE_A_DADR_REL);
			ASM_Ri(0xB0, CJNE_atRi_hC8_REL);
			ASM_Rn(0xB0, CJNE_Rn_hC8_REL);

			ASM_COMMAND(0xC0, PUSH_DADR);
			ASM_COMMAND(0xC1, AJMP_ADDR11_(6));
			ASM_COMMAND(0xC2, CLR_BADR);
			ASM_COMMAND(0xC3, CLR_C);
			ASM_COMMAND(0xC4, SWAP_A);
			ASM_COMMAND(0xC5, XCH_A_DADR);
			ASM_Ri(0xC0, XCH_A_atRi);
			ASM_Rn(0xC0, XCH_A_atRn);

			ASM_COMMAND(0xD0, POP_DADR);
			ASM_COMMAND(0xD1, ACALL_ADDR11_(6));
			ASM_COMMAND(0xD2, SETB_BADR);
			ASM_COMMAND(0xD3, SETB_C);
			ASM_COMMAND(0xD4, DA_A);
			ASM_COMMAND(0xD5, DJNZ_DADR_REL);
			ASM_Ri(0xD0, XCHD_A_atRi);
			ASM_Rn(0xD0, DJNZ_Rn_REL);

			ASM_COMMAND(0xE0, MOVX_A_atDPTR);
			ASM_COMMAND(0xE1, AJMP_ADDR11_(7));
			ASM_Ri_Custom(0xE2, MOVX_A_atRi);
			ASM_COMMAND(0xE4, CLR_A);
			ASM_COMMAND(0xE5, MOV_A_DADR);
			ASM_Ri(0xE0, MOV_A_atRi);
			ASM_Rn(0xE0, MOV_A_Rn);

			ASM_COMMAND(0xF0, MOVX_atDPTR_A);
			ASM_COMMAND(0xF1, ACALL_ADDR11_(7));
			ASM_Ri_Custom(0xF2, MOVX_atRi_A);
			ASM_COMMAND(0xF4, CPL_A);
			ASM_COMMAND(0xF5, MOV_DADR_A);
			ASM_Ri(0xF0, MOV_atRi_A);
			ASM_Rn(0xF0, MOV_Rn_A);

		}
		//INTERRUPT CONTROLLER
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
	}while (run);
}


char _4BitHEX(uint8_t parse) {
	switch (parse & 0x0F) {
	case 0x00:
		return '0';
	case 0x01:
		return '1';
	case 0x02:
		return '2';
	case 0x03:
		return '3';
	case 0x04:
		return '4';
	case 0x05:
		return '5';
	case 0x06:
		return '6';
	case 0x07:
		return '7';
	case 0x08:
		return '8';
	case 0x09:
		return '9';
	case 0x0A:
		return 'A';
	case 0x0B:
		return 'B';
	case 0x0C:
		return 'C';
	case 0x0D:
		return 'D';
	case 0x0E:
		return 'E';
	case 0x0F:
		return 'F';
	}
}

void dump_array(uint8_t *array, uint32_t lenght) {
	
	uint8_t firstNullLine = 1;
	for (uint16_t a = 0, b = 0;a<lenght / 16;a++, b = 0) {
//		cout << (*((uint64_t *)&array[(a << 4)+8])) << endl;
//		cout << (*((uint64_t *)&array[(a << 4)])) << endl;
		if((*((uint64_t *)&array[a << 4])) | (*((uint64_t *)&array[(a << 4)+8])))
		{
			firstNullLine = 1;
			cout << _4BitHEX((uint8_t)(a >> 8)) << _4BitHEX((uint8_t)(a >> 4)) << _4BitHEX((uint8_t)a) << "0h: ";
			do {
				cout << _4BitHEX(array[b | a << 4] >> 4) << _4BitHEX(array[b | a << 4]) << ' ';
			} while ((++b & 0xF) != 0);
			cout << endl;
		}
		else if(firstNullLine)
		{
			cout << "..." << endl;
			firstNullLine = 0;
		}
	}
}


/* //Old (malfunctioning) version of the file read function
void read_HEX(string filename) {
	ifstream in_stream(filename, ios::in|ios::binary|ios::ate);
	uint16_t lenght,address;
	uint8_t *target;
	char buffer[FILE_READ_BUFFER_SIZE];
	char *buffer_ptr = buffer;
//	in_stream.seekg(0, in_stream.end);
	uint32_t size = in_stream.tellg();
	in_stream.seekg(0, in_stream.beg);
	while(true) {
		if (size > FILE_READ_BUFFER_SIZE) {
			size -= FILE_READ_BUFFER_SIZE;
			buffer_ptr = buffer;
			if (!in_stream.read(buffer_ptr, FILE_READ_BUFFER_SIZE)) { return; };
		}
		else if (!size) {
			in_stream.close();
			return;
		}
		else{
			buffer_ptr = &buffer[FILE_READ_BUFFER_SIZE - size];
			in_stream.read(buffer_ptr, size);
			size = 0;
		}
		//while (*buffer_ptr != ':') {buffer_ptr++;}
		for (uint8_t a = 0, b, c = 0, d = 0;buffer_ptr<&buffer[FILE_READ_BUFFER_SIZE];a++) {
			
			
			//Alternative to switch
//			b = *(buffer_ptr++);
//			if(b >= '0' && b <= '9') b -= '0';
//			else if(b >= 'A' && b <= 'F') b -= 'A' - 10;
//			else if(b >= 'a' && b <= 'f') b -= 'a' - 10;
//			else if(b == ':') b = -2;
//			else b = -1;
			
			
			switch (*(buffer_ptr++)) {
			case '0':
				b = 0;
				break;

			case '1':
				b = 1;
				break;

			case '2':
				b = 2;
				break;

			case '3':
				b = 3;
				break;

			case '4':
				b = 4;
				break;

			case '5':
				b = 5;
				break;

			case '6':
				b = 6;
				break;

			case '7':
				b = 7;
				break;

			case '8':
				b = 8;
				break;

			case '9':
				b = 9;
				break;

			case 'A':
				b = 0x0A;
				break;

			case 'B':
				b = 0x0B;
				break;

			case 'C':
				b = 0x0C;
				break;

			case 'D':
				b = 0x0D;
				break;

			case 'E':
				b = 0x0E;
				break;

			case 'F':
				b = 0x0F;
				break;

			case 'a':
				b = 0x0A;
				break;

			case 'b':
				b = 0x0B;
				break;

			case 'c':
				b = 0x0C;
				break;

			case 'd':
				b = 0x0D;
				break;

			case 'e':
				b = 0x0E;
				break;

			case 'f':
				b = 0x0F;
				break;

			case ':':
				b = -2;
				break;

			default:
				b = -1;
			}
			if (b == -1) {}
			else if (b == -2) {
				d = 0;
			}
			else {
				switch (d++) {
					case 0:
					case 1:
						lenght |= b << ((c == 0) * 4);
						cout << "LENGHT <<: " << hex << lenght << endl;
						c = !c;
						break;
					case 2:
					case 3:
					case 4:
					case 5:
						address |= b << ((4 - (++c)) * 4);
						break;
					case 6:
						--lenght *= 2;
						cout << "LENGHT *2: " << hex << lenght << endl;
						target = &EEPROM[address - 1];
						break;
					case 7:
						if (b == 1) {
							in_stream.close();
							return;
						}
						else { c = 0; }
						break;
					default:
						*(target += !c) |= b << ((c == 0) * 4);
						c = !c;
						lenght--;
						d--;
				}
			}
		}
	}
}*/

uint8_t hexCharToInt(char hex)
{
	if(hex >= '0' && hex <= '9') return hex - '0';
	else if(hex >= 'A' && hex <= 'F') return hex - 'A' + 10;
	else if(hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
	else return 255;
}


#define FILE_READ_BUFFER_SIZE 512

/**
Opens a file from a given path {filename} and writes the given OP codes into the EEPROM.
The file has to be of the Intel HEX format.
*/
void read_HEX_G(string filename)
{
	ifstream in_stream(filename);
	
	if(in_stream.is_open())
	{
		for(string buffer; getline(in_stream,buffer);)
		{
			if(buffer[buffer.length()-1] == '\r') buffer.pop_back();
//			cerr << "FILE: Read line: \"" << buffer << "\"" << endl; //DEBUG
			uint16_t d = 0;
			uint8_t size = 0;
			uint16_t addr = 0;
			
			for(uint16_t i = 0; i < buffer.length()-2; i++)
			{
				if(buffer[i] == ':')
				{
					d = 0;
				}
				else
				{
					switch(d)
					{
						case 0:
							size = hexCharToInt(buffer[i]) << 4;
							break;
						case 1:
							size |= hexCharToInt(buffer[i]);
							break;
						case 2:
							addr = hexCharToInt(buffer[i]) << 12;
							break;
						case 3:
						case 4:
						case 5:
							addr |= hexCharToInt(buffer[i]) << (20 - 4 * d);
							break;
						case 6:
//							cerr << "FILE: Size = " << (int)size << endl; //DEBUG
//							cerr << "FILE: Addr = " << hex << addr << endl; //DEBUG
							break;
						case 7:
							if(buffer[i] == '1')
							{
								in_stream.close();
//								cerr << "FILE: done." << endl; //DEBUG
								return;
							}
							break;
						default:
							EEPROM[addr+d-8] = (hexCharToInt(buffer[i]) << 4) | (hexCharToInt(buffer[i+1]));
					}
					d++;
				}
			}
		}
	}
	else
	{
		cerr << "Couldn't open HEX File..." << endl;
	}
}

void output() {
	cout << "ProgramCounter: " << pc << " HEX: " << _4BitHEX((uint8_t)(pc >> 12)) << _4BitHEX((uint8_t)(pc >> 8)) << ' ' << _4BitHEX((uint8_t)(pc >> 4)) << _4BitHEX((uint8_t)(pc)) << endl;
	//cout << "EEPROM:" << endl;
	//dump_array(EEPROM, EEPROM_SIZE);
	cout << "RAM:" << endl;
	dump_array(RAM, RAM_SIZE);
	cout << "SFR:" << endl;
	dump_array(SFR + SFR_SIZE, SFR_SIZE);
}


void load_EEPROM(uint16_t address, uint8_t *data, uint32_t lenght) {
	for (lenght += address;address<lenght;address++, data++) {
		EEPROM[address] = *data;
	}
}
void load_RAM(uint8_t address, uint8_t *data, uint16_t lenght) {
	for (lenght += address;address<lenght;address++, data++) {
		RAM[address] = *data;
	}
}


void input() {
	char *buffer;
	cin >> buffer;
	switch (buffer[0]) {
	case '+'://ADD
		break;
	case '-':
		break;
	case '#':
		break;
	}
}

/**
A test program:
*/
void init_beta_EEPROM() {
	int a = 0, b;
	int command = 0;
	EEPROM[a++] = 0x02;//LJMP
	EEPROM[a++] = 0x00;
	EEPROM[a++] = 0x01;
	EEPROM[0x03 + 1 * 0x08] = 0xA3;//INC DPTR
	EEPROM[0x03 + 1 * 0x08 + 1] = 0xE4;//CLR A
	EEPROM[0x03 + 1 * 0x08+2]=0x32;//RETI am Timer 0 interrupt
	EEPROM[0x03 + 3 * 0x08] = 0x32;//RETI am Timer 1 interrupt
	EEPROM[(a = 0x0100)++] = 0x74;//MOV A,#c
	EEPROM[a++] = 0xFE;
	EEPROM[a++] = 0x75;//MOV TIMER,#c
	EEPROM[a++] = TIMER_0;
	EEPROM[a++] = 0xE4;
	EEPROM[a++] = 0x75;//MOV TIMER,#c
	EEPROM[a++] = TIMER_0 + 2;
	EEPROM[a++] = 0xFF;
	EEPROM[a++] = 0x75;//MOV TCON,#c
	EEPROM[a++] = TCON;
	EEPROM[a++] = 0xFF;
	EEPROM[a++] = 0x75;//MOV TMOD,#c
	EEPROM[a++] = TMOD;
	EEPROM[a++] = 0x31;
	EEPROM[a++] = 0xD2;//MOV SETB,badr
	EEPROM[a++] = EA_FLAG;
	EEPROM[a++] = 0xD2;//MOV SETB,badr
	EEPROM[a++] = T0INT_FLAG;
	EEPROM[a++] = 0xD2;//MOV SETB,badr
	EEPROM[a++] = T1INT_FLAG;
	b = a;
	for (int i = 0;i < 8;i++) {//A-12
		EEPROM[a++] = 0xF8 + i;//MOV A,Rn
	}
	EEPROM[a++] = 2;//LJMP
	EEPROM[a++] = 0x00;
	EEPROM[a++] = 0xF0;
	EEPROM[(a = 0xF000)++] = 0x14;//INC A
	EEPROM[a++] = 2;//LJMP
	EEPROM[a++] = b;
	EEPROM[a++] = 0x01;
}


uint8_t get_Port(uint8_t n) {
	return SFR[PORT(n)] & extPort[n];
}

void set_Port(uint8_t n, uint8_t value) {
	extPort[n] = value;
}

void init() {//Initialization of the SFR
	SFR[stack_pointer] = 0x07;
	for (uint8_t a = 0;a<4;a++) {
		SFR[PORT(a)] = 0xFF;
	}
}

void exec(string filename)
{
	init();
	read_HEX_G(filename);
	debug << "EEPROM:" << endl;
	if(DEBUG) dump_array(EEPROM,EEPROM_SIZE);
	debug << endl;
	uint64_t a = 0;
	run = 0;
	while (true)
	{
		interpreter();
		if(MANUAL_STEP)
		{
			cout << "Press enter to proceed..." << endl;
			cin.ignore();
		}
		cout << endl;
		output();
		bit_not(SFR[0x60 + 0x80],7);
		cout << "Step: " << ++a << "\n" << endl;
		cout << "------------------------" << endl;
	}
}

int main(int argc, char* argv[]) {
	if(argc > 1)
	{
		exec(argv[1]);
	}
	else
	{
		char buffer[256];
		cout << "Usage: " << argv[0] << " FILE" << endl;
		cout << "Running in interactive mode..." << endl;
		cout << "HEX File path: ";
		cin >> buffer;
		exec(buffer);
	}
	
	return 0;
}

//Old test main
/*
init();
//init_beta_EEPROM();

if(argc > 1)
{
	read_HEX_G(argv[1]);
}
else
{
	char a[256];
	cout << "HEX File path: ";
	cin >> a;
	read_HEX_G(a);
}
cout << "EEPROM:" << endl;
dump_array(EEPROM, EEPROM_SIZE);
cin.ignore();
output();
cout << "Wait for enter\n";
cin.ignore();
long a = 0;
run = false;
while (true) {
	interpreter();
	cin.ignore();
	output();
	bit_not(SFR[0x60 + 0x80],7);
	cout << "Step: "<<++a<<"\n";
}
//output();
//bool stop = false;
//for (int i = 0;i < 10000;i++) { interpreter(stop); }
//cout << (int)get_Bit(0x94) << "\n";
//output();
cin.ignore();
return 0;
*/
