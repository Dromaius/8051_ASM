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


API_SHOW void read_IntelHEX(void* ROM,void RAM,char* file){

}

API_SHOW void write_IntelHEX(void* ROM,void RAM,char* file){

}

API_SHOW void clean_IntelHEX(char* file){

}

API_SHOW void IntelHEX2Bin(){}
API_SHOW void Bin2IntelHEX(){}