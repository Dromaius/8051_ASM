/*includes*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
#include "API_header.h"

//get size
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



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


API_SHOW bool read_IntelHEX(void* ROM,char* file){
	FILE* in_file;
	uint8_t parsed,lenght,checksum,mode = 0;
	uint16_t address;
	char buffer[BUFFER_SIZE];
	char* buffer_ptr;
	char* target_ptr;
	struct stat file_disc;
	if(!(in_file=fopen(file,"rb"))) return false;
	fstat(in_file, &file_disc);
	for(int size = file_disc.st_size;size;){
		if(size>=1024){
			fread(buffer_ptr = buffer,BUFFER_SIZE,1,in_file);
			size -= BUFFER_SIZE;
		}
		else{
			fread(buffer_ptr = &buffer[BUFFER_SIZE-size],size,1,in_file);
			size = 0;
			fclose(in_file);
		}
		while(buffer_ptr++ < &buffer[BUFFER_SIZE]){
			if(*buffer_ptr == ':') mode=0;
			else if((parsed=Char2Int(*buffer_ptr))!=255){
				switch(mode++){
					case 0:
						lenght = parsed << 4;
						break;
					case 1:
						lenght |= parsed;
						checksum = lenght;
						break;
					case 2:
						address = 0;
					case 3:
					case 4:
					case 5:
						address|=parsed<<(4*(6-mode));
						checksum -= address + (address>>8);
						break;
					case 7:
						if(parsed==1){
							mode += (252-8);
							target_ptr = NULL;
							address = 0;
						}
						break;
					case 8:
						*target_ptr = parsed << 4;
						break;
					case 9:
						*target_ptr |= parsed;
						checksum-=*(target_ptr++);
						if(--lenght > 0)mode = 7;
						else mode += (252-10);
						break;
					case 253:
						if(!(checksum&0x0F==parsed)) return false;
						break;
					case 254:
						if(!(checksum&0xF0==parsed<<4)) return false;
						else if((target_ptr==NULL)&&(address==0)) return true;
						break;
					case 6:
					case 255:
					default:
						mode--;
						break;
				}
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