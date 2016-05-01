#define bit_opperation(func,address) if (address & 0x80) {switch(address&0xF8){case PSW:func (psw, address & 0x07);refresh_Rn;break;default:func (SFR[address & 0xF8], address & 0x07);break;}}else{func (RAM[(address >> 3)+0x20], address & 0x07);}
#define set_Bit(address) bit_opperation(bit_set, address)
#define clr_Bit(address) bit_opperation(bit_clr_8, address)
#define not_Bit(address) bit_opperation(bit_not, address)

#define refresh_Rn R = &RAM[(psw & 0x18) >> 3]
#define carry_get get_Bit(CARRY)
#define carry_set if(carry){set_Bit(CARRY);}else{clr_Bit(CARRY);}
#define hcarry_set if(hcarry){set_Bit(HCARRY);}else{clr_Bit(HCARRY);}
#define of_set if(overflow){set_Bit(OF);}else{clr_Bit(OF);}
#define register_set carry_set hcarry_set of_set
#define bit_asm_command(a,b,opp) if (get_Bit(a) opp get_Bit(b)) {set_Bit(a);}else { clr_Bit(a);}
#define set_RAM(address,value) if(address & 0x80) {switch(address){case PSW:psw=value;refresh_Rn;break;default:SFR[address] = value;}}else{RAM[address]=value;}
#define get_stack pc = RAM[stack - 1] <<8| (RAM[stack]);stack -= 2
//#define set_stack(a) *(uint16_t *)&RAM[stack]=pc a;stack+=2
#define set_stack(a) RAM[++stack] = (pc a)<<8;RAM[++stack] = pc a; 

#define get_Bit_SFR(address) bit_address (SFR[address & 0xF8], address & 0x07)
//PARAMETER_REGISTERBLOCKS
#define ADDR11 EEPROM[pc]
//#define ADDR16 *((uint16_t *)(&EEPROM[pc]))
#define REL(a) EEPROM[pc a]
#define hC8(a) EEPROM[pc a]
#define hC16 *((uint16_t *)(&EEPROM[pc]))
#define atx(a) RAM[a]
#define BADR(a) EEPROM[pc a]
#define Rn(a) R[a]
#define Ri(a) atx(R[a])
#define dtptr *((uint16_t *)(&SFR[DTPTR]))
#define dptruC (uint16_t)(SFR[DTPTR]<<8|SFR[DTPTR+1])

#define get_DADR(a) get_RAM(EEPROM[pc a])
#define set_DADR(a,b) set_RAM(EEPROM[pc a],b)
//ASSEMBLER MACROS
#define ASM_COMMAND(opcode,command) case opcode:command;break
#define ASM_Rn(opcode_4,command) ASM_COMMAND(opcode_4|0x08,command(0));ASM_COMMAND(opcode_4|0x09,command(1));ASM_COMMAND(opcode_4|0x0A,command(2));ASM_COMMAND(opcode_4|0x0B,command(3));ASM_COMMAND(opcode_4|0x0C,command(4));ASM_COMMAND(opcode_4|0x0D,command(5));ASM_COMMAND(opcode_4|0x0E,command(6));ASM_COMMAND(opcode_4|0x0F,command(7))
#define ASM_Ri(opcode_4,command) ASM_COMMAND(opcode_4|0x06,command(0));ASM_COMMAND(opcode_4|0x07,command(1))
#define ASM_Ri_Custom(opcode_7,command) ASM_COMMAND(opcode_7|0x00,command(0));ASM_COMMAND(opcode_7|0x01,command(1))
//COMMANDS
#define inc_a(a) a++
#define dec_a(a) a--
#define mov_b2a(a,b) a=b
#define movc_b2a(a,b) a=EEPROM[b]
#define xchd_ab(a,b) temp2=a;a=(a&0xF0)|(b&0x0F);b=(b&0xF0)|(temp2&0x0F)
#define xch_ab(a,b) temp2=a;a=b;b=temp2
#define add_b2a(a,b) a=temp=a+b;register_set
#define addc_b2a(a,b) a=temp=a+b+carry_get;register_set
#define orl_b2a(a,b) a|=b
#define anl_b2a(a,b) a&=b
#define xor_b2a(a,b) a^=b
#define subb_b2a(a,b) a=temp=a-b+carry_get;register_set
#define rr_a(a) a=(a>>1)|(a<<7)
#define rrc_a(a) if(carry_get){a=(a>>1)|(a<<7);}
#define rl_a(a) a=(a<<1)|(a>>7)
#define rlc_a(a) if(carry_get){a=(a<<1)|(a>>7);}
#define div_ab(a,b) a/=b
#define mul_ab(a,b) a*=b
#define cpl_a(a) a=~a
#define clr_a(a) a=0
#define swap_a(a) a=a<<4|a>>4;
#define da_a(a) if(a&0x0F>=0x0A){a-=0x0A;a+=0x10;} if(a&0xF0>0xA0){a-=0xA0;}
//Bits
#define mov_bit_b2a(a,b) if(get_Bit(b)){set_Bit(a);}else{clr_Bit(a);}
#define orl_bit_b2a(a,b) bit_asm_command(a,b,||)
#define orl_notbit_b2a(a,b) bit_asm_command(a,b,|| !)
#define anl_bit_b2a(a,b) bit_asm_command(a,b,&&)
#define anl_notbit_b2a(a,b) bit_asm_command(a,b,&& !)
#define xor_bit_b2a(a,b) bit_asm_command(a,b,^^)
#define xor_notbit_b2a(a,b) bit_asm_command(a,b,^^ !)
#define cpl_bit_a(a) not_Bit(a)
#define clr_bit_a(a) clr_Bit(a)
#define setb_a(a) set_Bit(a)
//jumps&calls
#define jb_ab(a,b) if(get_Bit(a)){pc+=(int8_t)b-1;}else{pc+=2;}
#define jnb_ab(a,b) if(get_Bit(a)){pc+=2;}else{pc+=(int8_t)b-1;}
#define jbc_ab(a,b) if(get_Bit(a)){clr_Bit(a);pc+=(int8_t)b-1;}else{pc+=2;}
#define jc_a(a) if(carry_get){pc+=(int8_t)a-1;}
#define jnc_a(a) if(!carry_get){pc+=(int8_t)a-1;}
#define jz_a(a) if(akku){pc++;}else{pc+=(int8_t)a-1;}
#define jnz_a(a) if(akku){pc+=(int8_t)a-1;}else{pc++;}
#define sjmp_a(a) pc+=(int8_t)a-1
#define jmp_a(a) pc=a
#define cjne_abc(a,b,c) if(a==b){pc+=(int8_t)c-1;}else{pc+=2;}
#define djnz_ab(a,b) if(--a){pc++;}else{pc+=(int8_t)b-1;}
//SFR_BOOST

//ASSEMBLER INTERFACE
#define NOP time(1)
#define AJMP_ADDR11_(page) time(2);pc = (page<<8) | EEPROM[pc]
#define ACALL_ADDR11_(page) time(2);set_stack(+1);pc = (page<<8) | EEPROM[pc]
#define LJMP_ADDR16 time(2);pc=(EEPROM[pc]<<8)|(EEPROM[pc+1]);pc+=2
#define RR_A time(1);rr_a(akku)
#define INC_A time(1);inc_a(akku)
#define INC_DADR time(1);set_DADR(++,get_DADR()+1)
#define INC_atRi(a) time(1);inc_a(Ri(a))
#define INC_Rn(a) time(1);inc_a(Rn(a))

#define JBC_BADR_REL time(2);jbc_ab(BADR(),REL()) 
#define LCALL_ADDR16 time(2);set_stack(+2);pc=(EEPROM[pc]<<8)|(EEPROM[pc+1]);pc+=2
#define RRC_A time(1);rrc_a(akku)
#define DEC_A time(1);dec_a(akku)
#define DEC_DADR time(1); time(1);set_DADR(++,get_DADR()-1)
#define DEC_atRi(a) time(1);dec_a(Ri(a))
#define DEC_Rn(a) time(1);dec_a(Rn(a))

#define JB_BADR_REL time(2);jb_ab(BADR(),REL())
#define RET time(2);get_stack
#define RL_A time(1);rl_a(akku)
#define ADD_A_hC8 time(1);add_b2a(akku,hC8(++))
#define ADD_A_DADR time(1);add_b2a(akku,get_DADR(++))
#define ADD_A_atRi(a) time(1);add_b2a(akku,Ri(a))
#define ADD_A_Rn(a) time(1);add_b2a(akku,Rn(a))

#define JNB_BADR_REL time(2);jnb_ab(BADR(),REL())
#define RETI time(2);get_stack;clr_Bit(INT_FLAG)
#define RLC_A time(1);rlc_a(akku)
#define ADDC_A_hC8 time(1);addc_b2a(akku,hC8(++))
#define ADDC_A_DADR time(1);addc_b2a(akku,get_DADR(++))
#define ADDC_A_atRi(a) time(1);addc_b2a(akku,Ri(a))
#define ADDC_A_Rn(a) time(1);addc_b2a(akku,Rn(a))

#define JC_REL time(2);jc_a(REL())
#define ORL_DADR_A time(1);set_DADR(++,akku|get_DADR())
#define ORL_DADR_hC8 time(2);set_DADR(,get_DADR()|hC8(+1));pc+=2
#define ORL_A_hC8 time(1);orl_b2a(akku,hC8(++))
#define ORL_A_DADR time(1);orl_b2a(akku,get_DADR(++))
#define ORL_A_atRi(a) time(1);orl_b2a(akku,Ri(a))
#define ORL_A_Rn(a) time(1);orl_b2a(akku,Rn(a))

#define JNC_REL time(2);jnc_a(REL())
#define ANL_DADR_A time(1);set_DADR(++,akku&get_DADR())
#define ANL_DADR_hC8 time(2);set_DADR(,get_DADR()&hC8());pc+=2
#define ANL_A_hC8 time(1);anl_b2a(akku,hC8(++))
#define ANL_A_DADR time(1);anl_b2a(akku,get_DADR(++))
#define ANL_A_atRi(a) time(1);anl_b2a(akku,Ri(a))
#define ANL_A_Rn(a) time(1);anl_b2a(akku,Rn(a))

#define JZ_REL time(2);jz_a(REL())
#define XRL_DADR_A time(1);set_DADR(++,akku^get_DADR())
#define XRL_DADR_hC8 time(2);set_DADR(,get_DADR()^hC8());pc+=2
#define XRL_A_hC8 time(1);xor_b2a(akku,hC8(++))
#define XRL_A_DADR time(1);xor_b2a(akku,get_DADR(++))
#define XRL_A_atRi(a) time(1);xor_b2a(akku,Ri(a))
#define XRL_A_Rn(a) time(1);xor_b2a(akku,Rn(a))

#define JNZ_REL time(2);jnz_a(REL())
#define ORL_C_BADR time(1);orl_bit_b2a(CARRY,BADR(++))
#define JMP_atAplusDPTR time(2);jmp_a(atx(akku)+dptruC)
#define MOV_A_hC8 time(1);mov_b2a(akku,hC8(++))
#define MOV_DADR_hC8 time(2);set_DADR(,hC8(+1));pc+=2
#define MOV_atRi_hC8(a) time(1);mov_b2a(Ri(a),hC8(++))
#define MOV_Rn_hC8(a) time(1);mov_b2a(Rn(a),hC8(++))

#define SJUMP_REL time(2);sjmp_a(REL())
#define ANL_C_BADR time(1);anl_bit_b2a(CARRY,BADR(++))
#define MOVC_A_atAlpusPC time(2);movc_b2a(akku,atx(akku)+(pc++)-1)
#define DIV_AB time(4);div_ab(akku,SFR[register_B])
#define MOV_DADR_DADR time(2);set_DADR(,get_DADR(+1));pc+=2
#define MOV_DADR_atRi(a) time(1);set_DADR(++,Ri(a));
#define MOV_DADR_Rn(a) time(1);set_DADR(++,Rn(a));

#define MOV_DPTR_hC16 time(2); mov_b2a(dtptr,hC16);pc+=2//FIXed :)
#define MOV_BADR_C time(1);mov_bit_b2a(BADR(+1),CARRY);pc++
#define MOVC_A_atAlpusDPTR time(2);movc_b2a(akku,atx(akku)+dptruC)
#define SUBB_A_hC8 time(1);subb_b2a(akku,hC8(++))
#define SUBB_A_DADR time(1);subb_b2a(akku,get_DADR(++))
#define SUBB_A_atRi(a) time(1);subb_b2a(akku,Ri(a))
#define SUBB_A_Rn(a) time(1);subb_b2a(akku,Rn(a))

#define ORL_C_nBADR time(1);orl_notbit_b2a(CARRY,BADR(++))
#define MOV_C_BADR time(1);mov_bit_b2a(CARRY,BADR(++))
#define INC_DPTR time(2);if(!(++SFR[DTPTR+1])){SFR[DTPTR]++;}
#define MUL_AB time(4);mul_ab(akku,register_B)
#define reserviert time(1);/*RESET*/
#define MOV_atRi_DADR(a) time(1);mov_b2a(Ri(a),get_DADR(++))
#define MOV_Rn_DADR(a) time(1);mov_b2a(Rn(a),get_DADR(++))

#define ANL_C_nBADR time(1);anl_notbit_b2a(CARRY,BADR(++))
#define CPL_BADR time(1);cpl_bit_a(BADR(++))
#define CPL_C time(1);cpl_bit_a(BADR(++))
#define CJNE_A_hC8_REL  time(2);cjne_abc(akku,hC8(),REL(+1))
#define CJNE_A_DADR_REL  time(2);cjne_abc(akku,get_DADR(),REL(+1))
#define CJNE_atRi_hC8_REL(a)  time(2);cjne_abc(Ri(a),hC8(),REL(+1))
#define CJNE_Rn_hC8_REL(a)  time(2);cjne_abc(Rn(a),hC8(),REL(+1))

#define PUSH_DADR time(2); RAM[++stack]=get_DADR(++)
#define CLR_BADR time(1); clr_bit_a(BADR(++))
#define CLR_C time(1);clr_bit_a(CARRY)
#define SWAP_A time(1);swap_a(akku)
#define XCH_A_DADR time(1);temp2=akku;akku=get_DADR();set_DADR(++,temp2)
#define XCH_A_atRi(a) time(1);xch_ab(akku,Ri(a))
#define XCH_A_atRn(a) time(1);xch_ab(akku,Rn(a))

#define POP_DADR time(2); set_DADR(++,RAM[stack--])
#define SETB_BADR time(1); setb_a(BADR(++))
#define SETB_C time(1);setb_a(CARRY)
#define DA_A time(1);da_a(akku)
#define DJNZ_DADR_REL  time(2);set_DADR(,(temp2=get_DADR()-1))if(temp2){pc+=2;}else{pc+=(int8_t)REL(+1)-1;}
#define XCHD_A_atRi(a)  time(1);xchd_ab(akku,Ri(a))
#define DJNZ_Rn_REL(a)  time(2);djnz_ab(Rn(a),REL())

#define MOVX_A_atDPTR time(2);/*w/o external memory*/
#define MOVX_A_atRi(a) time(2);/*w/o external memory*/
#define CLR_A time(1);clr_a(akku)
#define MOV_A_DADR time(1);mov_b2a(akku,get_DADR(++))
#define MOV_A_atRi(a) time(1);mov_b2a(akku,Ri(a))
#define MOV_A_Rn(a) time(1);mov_b2a(akku,Rn(a))

#define MOVX_atDPTR_A time(2);/*w/o external memory*/
#define MOVX_atRi_A(a) time(2);/*w/o external memory*/
#define CPL_A time(1);cpl_a(akku)
#define MOV_DADR_A time(1);set_DADR(++,akku)
#define MOV_atRi_A(a) time(1);mov_b2a(Ri(a),akku)
#define MOV_Rn_A(a) time(1);mov_b2a(Rn(a),akku)
