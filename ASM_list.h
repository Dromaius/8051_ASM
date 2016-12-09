/*usage:
#define ASM_COMMAND(opcode,command) case opcode:command break
*/
/*switch macros
#define ASM_Rn(opcode_4,command) ASM_COMMAND(opcode_4|0x08,command(0)) ASM_COMMAND(opcode_4|0x09,command(1)) ASM_COMMAND(opcode_4|0x0A,command(2)) ASM_COMMAND(opcode_4|0x0B,command(3)) ASM_COMMAND(opcode_4|0x0C,command(4)) ASM_COMMAND(opcode_4|0x0D,command(5)) ASM_COMMAND(opcode_4|0x0E,command(6)) ASM_COMMAND(opcode_4|0x0F,command(7))
#define ASM_Ri(opcode_4,command) ASM_COMMAND(opcode_4|0x06,command(0)) ASM_COMMAND(opcode_4|0x07,command(1))
#define ASM_Ri_Custom(opcode_7,command) ASM_COMMAND(opcode_7|0x00,command(0)) ASM_COMMAND(opcode_7|0x01,command(1))
*/
/*pointer macros*/
#define ASM_Rn(opcode_4,command) ASM_COMMAND(##opcode_4 _0x08,command(0)) seperator ASM_COMMAND(##opcode_4 _0x09,command(1)) seperator ASM_COMMAND(##opcode_4 _0x0A,command(2)) seperator ASM_COMMAND(##opcode_4 _0x0B,command(3)) seperator ASM_COMMAND(##opcode_4 _0x0C,command(4)) seperator ASM_COMMAND(##opcode_4 _0x0D,command(5)) seperator ASM_COMMAND(##opcode_4 _0x0E,command(6)) seperator ASM_COMMAND(##opcode_4 _0x0F,command(7))
#define ASM_Ri(opcode_4,command) ASM_COMMAND(##opcode_4 _0x06,command(0)) seperator ASM_COMMAND(##opcode_4 _0x07,command(1))
#define ASM_Ri_Custom(opcode_7,command) ASM_COMMAND(##opcode_7 _0x00,command(0)) seperator ASM_COMMAND(##opcode_7 _0x01,command(1))

ASM_COMMAND(0x00, NOP) seperator
ASM_COMMAND(0x01, AJMP_ADDR11_(0)) seperator
ASM_COMMAND(0x02, LJMP_ADDR16) seperator
ASM_COMMAND(0x03, RR_A) seperator
ASM_COMMAND(0x04, INC_A) seperator
ASM_COMMAND(0x05, INC_DADR) seperator
ASM_Ri(0x00, INC_atRi) seperator
ASM_Rn(0x00, INC_Rn) seperator

ASM_COMMAND(0x10, JBC_BADR_REL) seperator
ASM_COMMAND(0x11, ACALL_ADDR11_(0)) seperator
ASM_COMMAND(0x12, LCALL_ADDR16) seperator
ASM_COMMAND(0x13, RRC_A) seperator
ASM_COMMAND(0x14, DEC_A) seperator
ASM_COMMAND(0x15, DEC_DADR) seperator
ASM_Ri(0x10, DEC_atRi) seperator
ASM_Rn(0x10, DEC_Rn) seperator

ASM_COMMAND(0x20, JB_BADR_REL) seperator 
ASM_COMMAND(0x21, AJMP_ADDR11_(1)) seperator 
ASM_COMMAND(0x22, RET) seperator 
ASM_COMMAND(0x23, RL_A) seperator 
ASM_COMMAND(0x24, ADD_A_hC8) seperator 
ASM_COMMAND(0x25, ADD_A_DADR) seperator 
ASM_Ri(0x20, ADD_A_atRi) seperator 
ASM_Rn(0x20, ADD_A_Rn) seperator 

ASM_COMMAND(0x30, JNB_BADR_REL) seperator 
ASM_COMMAND(0x31, ACALL_ADDR11_(1)) seperator 
ASM_COMMAND(0x32, RETI) seperator 
ASM_COMMAND(0x33, RLC_A) seperator 
ASM_COMMAND(0x34, ADDC_A_hC8) seperator 
ASM_COMMAND(0x35, ADDC_A_DADR) seperator 
ASM_Ri(0x30, ADDC_A_atRi) seperator 
ASM_Rn(0x30, ADDC_A_Rn) seperator 

ASM_COMMAND(0x40, JC_REL) seperator 
ASM_COMMAND(0x41, AJMP_ADDR11_(2)) seperator 
ASM_COMMAND(0x42, ORL_DADR_A) seperator 
ASM_COMMAND(0x43, ORL_DADR_hC8) seperator 
ASM_COMMAND(0x44, ORL_A_hC8) seperator 
ASM_COMMAND(0x45, ORL_A_DADR) seperator 
ASM_Ri(0x40, ORL_A_atRi) seperator 
ASM_Rn(0x40, ORL_A_Rn) seperator 

ASM_COMMAND(0x50, JNC_REL) seperator 
ASM_COMMAND(0x51, ACALL_ADDR11_(2)) seperator 
ASM_COMMAND(0x52, ANL_DADR_A) seperator 
ASM_COMMAND(0x53, ANL_DADR_hC8) seperator 
ASM_COMMAND(0x54, ANL_A_hC8) seperator 
ASM_COMMAND(0x55, ANL_A_DADR) seperator 
ASM_Ri(0x50, ANL_A_atRi) seperator 
ASM_Rn(0x50, ANL_A_Rn) seperator 

ASM_COMMAND(0x60, JZ_REL) seperator 
ASM_COMMAND(0x61, AJMP_ADDR11_(3)) seperator 
ASM_COMMAND(0x62, XRL_DADR_A) seperator 
ASM_COMMAND(0x63, XRL_DADR_hC8) seperator 
ASM_COMMAND(0x64, XRL_A_hC8) seperator 
ASM_COMMAND(0x65, XRL_A_DADR) seperator 
ASM_Ri(0x60, XRL_A_atRi) seperator 
ASM_Rn(0x60, XRL_A_Rn) seperator 

ASM_COMMAND(0x70, JNZ_REL) seperator 
ASM_COMMAND(0x71, ACALL_ADDR11_(3)) seperator 
ASM_COMMAND(0x72, ORL_C_BADR) seperator 
ASM_COMMAND(0x73, JMP_atAplusDPTR) seperator 
ASM_COMMAND(0x74, MOV_A_hC8) seperator 
ASM_COMMAND(0x75, MOV_DADR_hC8) seperator 
ASM_Ri(0x70, MOV_atRi_hC8) seperator 
ASM_Rn(0x70, MOV_Rn_hC8) seperator 

ASM_COMMAND(0x80, SJUMP_REL) seperator 
ASM_COMMAND(0x81, AJMP_ADDR11_(4)) seperator 
ASM_COMMAND(0x82, ANL_C_BADR) seperator 
ASM_COMMAND(0x83, MOVC_A_atAlpusPC) seperator 
ASM_COMMAND(0x84, DIV_AB) seperator 
ASM_COMMAND(0x85, MOV_DADR_DADR) seperator 
ASM_Ri(0x80, MOV_DADR_atRi) seperator 
ASM_Rn(0x80, MOV_DADR_Rn) seperator 

ASM_COMMAND(0x90, SJUMP_REL) seperator 
ASM_COMMAND(0x91, ACALL_ADDR11_(4)) seperator 
ASM_COMMAND(0x92, MOV_BADR_C) seperator 
ASM_COMMAND(0x93, MOVC_A_atAlpusDPTR) seperator 
ASM_COMMAND(0x94, SUBB_A_hC8) seperator 
ASM_COMMAND(0x95, SUBB_A_DADR) seperator 
ASM_Ri(0x90, SUBB_A_atRi) seperator 
ASM_Rn(0x90, SUBB_A_Rn) seperator 

ASM_COMMAND(0xA0, ORL_C_nBADR) seperator 
ASM_COMMAND(0xA1, AJMP_ADDR11_(4)) seperator 
ASM_COMMAND(0xA2, MOV_C_BADR) seperator 
ASM_COMMAND(0xA3, INC_DPTR) seperator 
ASM_COMMAND(0xA4, MUL_AB) seperator 
ASM_COMMAND(0xA5, reserviert) seperator 
ASM_Ri(0xA0, MOV_atRi_DADR) seperator 
ASM_Rn(0xA0, MOV_Rn_DADR) seperator 

ASM_COMMAND(0xB0, ANL_C_nBADR) seperator 
ASM_COMMAND(0xB1, ACALL_ADDR11_(5)) seperator 
ASM_COMMAND(0xB2, CPL_BADR) seperator 
ASM_COMMAND(0xB3, CPL_C) seperator 
ASM_COMMAND(0xB4, CJNE_A_hC8_REL) seperator 
ASM_COMMAND(0xB5, CJNE_A_DADR_REL) seperator 
ASM_Ri(0xB0, CJNE_atRi_hC8_REL) seperator 
ASM_Rn(0xB0, CJNE_Rn_hC8_REL) seperator 

ASM_COMMAND(0xC0, PUSH_DADR) seperator 
ASM_COMMAND(0xC1, AJMP_ADDR11_(6)) seperator 
ASM_COMMAND(0xC2, CLR_BADR) seperator 
ASM_COMMAND(0xC3, CLR_C) seperator 
ASM_COMMAND(0xC4, SWAP_A) seperator 
ASM_COMMAND(0xC5, XCH_A_DADR) seperator 
ASM_Ri(0xC0, XCH_A_atRi) seperator 
ASM_Rn(0xC0, XCH_A_atRn) seperator 

ASM_COMMAND(0xD0, POP_DADR) seperator 
ASM_COMMAND(0xD1, ACALL_ADDR11_(6)) seperator 
ASM_COMMAND(0xD2, SETB_BADR) seperator 
ASM_COMMAND(0xD3, SETB_C) seperator 
ASM_COMMAND(0xD4, DA_A) seperator 
ASM_COMMAND(0xD5, DJNZ_DADR_REL) seperator 
ASM_Ri(0xD0, XCHD_A_atRi) seperator 
ASM_Rn(0xD0, DJNZ_Rn_REL) seperator 

ASM_COMMAND(0xE0, MOVX_A_atDPTR) seperator 
ASM_COMMAND(0xE1, AJMP_ADDR11_(7)) seperator 
ASM_Ri_Custom(0xE2, MOVX_A_atRi) seperator 
ASM_COMMAND(0xE4, CLR_A) seperator 
ASM_COMMAND(0xE5, MOV_A_DADR) seperator 
ASM_Ri(0xE0, MOV_A_atRi) seperator 
ASM_Rn(0xE0, MOV_A_Rn) seperator 

ASM_COMMAND(0xF0, MOVX_atDPTR_A) seperator 
ASM_COMMAND(0xF1, ACALL_ADDR11_(7)) seperator 
ASM_Ri_Custom(0xF2, MOVX_atRi_A) seperator 
ASM_COMMAND(0xF4, CPL_A) seperator 
ASM_COMMAND(0xF5, MOV_DADR_A) seperator 
ASM_Ri(0xF0, MOV_atRi_A) seperator 
ASM_Rn(0xF0, MOV_Rn_A) 

#undef ASM_Rn
#undef ASM_Ri
#undef ASM_Ri_Custom