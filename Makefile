.PHONY: release debug preprocessor

default: release

release:
	gcc -shared -fPIC -O3 -std=c99 -o 8051_ASM.so 8051_ASM.c

debug:
	gcc -shared -fPIC -std=c99 -o 8051_ASM.so 8051_ASM.c

preprocessor:
	gcc -E -std=c99 8051_ASM.c | sed 's/^#/\/\//' > 8051_ASM.pp.c
