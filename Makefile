default:
	gcc -shared -fPIC -O3 -std=c99 -o 8051_ASM.so 8051_ASM.c
release:
	gcc -shared -fPIC -O3 -std=c99 -o 8051_ASM.so 8051_ASM.c
debug:
	gcc -shared -fPIC -std=c99 -o 8051_ASM.so 8051_ASM.c