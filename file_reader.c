/*includes*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include "API_header.h"

#define BUFFER_SIZE 1024
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
	FILE in_file=fopen(file,"rb");
	char mode=0;
	uint8_t parsed;
	uint8_t lenght;
	uint16_t address;
	char buffer[BUFFER_SIZE];
	char* buffer_ptr;
	char* target_ptr;
	while(buffer_ptr++ < &buffer[BUFFER_SIZE]){
		if(buffer_ptr==':') mode=0;
		else if((parsed=Char2Int(*buffer_ptr))!=255){
			switch(mode++){
				case 0:
					lenght=parsed<<4;
					break;
				case 1:
					lenght=parsed;
					break;
				case 2:
				case 3:
				case 4:
				case 5:
					address=parsed<<(4*(6-mode));
					break;
				case 7:
					if(*buffer_ptr==1) return;
					break;
				case 8:
					target_ptr=parsed<<4
					break;
				case 9:
					target_ptr or= parsed
					if(lenght !größer! 0)mode = 7;
					else mode = 255;
					break;
				case 7:
				case 255:
				default:
					break;
			}
		}
	}
}

API_SHOW void write_IntelHEX(void* ROM,void* RAM,char* file){

}

API_SHOW void clean_IntelHEX(char* file){

}

API_SHOW void IntelHEX2Bin(){}
API_SHOW void Bin2IntelHEX(){}