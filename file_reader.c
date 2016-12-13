/*includes*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include "API_header.h"
typedef void (*writeROM)(uint16_t, uint8_t, uint32_t);
typedef void (*writeRAM)(uint8_t, uint8_t, uint16_t);

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


API_SHOW void read_IntelHEX(void* ROM,void* RAM,char* file){

}

API_SHOW void write_IntelHEX(void* ROM,void* RAM,char* file){

}

API_SHOW void clean_IntelHEX(char* file){

}

API_SHOW void IntelHEX2Bin(){}
API_SHOW void Bin2IntelHEX(){}