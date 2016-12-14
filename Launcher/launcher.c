/*includes*/
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dlfcn.h>
#include <stdio.h>
typedef bool (*loadHEX_func)(uint8_t*,char*);
typedef void (*exec_func)();
typedef void (*exec_func)();

char Int2Char(uint8_t parse) 
{
    if(0 <=parse <= 9) return parse + '0';
	else if(0x0A <= parse <= 0x0F) return parse + 'A' - 10;
	else return 0;
}

uint8_t Char2Int(char hex)
{
	if( '0' <= hex <= '9') return hex - '0';
	else if('A' <= hex <= 'F') return hex - 'A' + 10;
	else if( 'a' <= hex <= 'f') return hex - 'a' + 10;
	else return 255;
}

int main(){
	uint8_t eeprom_test[64*1024];
	uint8_t* test_eeprom=eeprom_test;
	void* file_reader = dlopen("./file_reader.so",RTLD_LAZY);
	printf("debug\n");
	if(((loadHEX_func)dlsym(file_reader,"read_IntelHEX"))(test_eeprom,"../tests/test2.hex"))
		printf("loading complete\n");
	else
		printf("loading failed");
	while(test_eeprom+64*1024-(test_eeprom++)) 
		if(*(test_eeprom)) {printf(Char2Int(*test_eeprom&0xF0));printf(Char2Int(*test_eeprom&0x0F));};
	return 0;
}
