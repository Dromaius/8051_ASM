.PHONY: release debug preprocessor

default: release

release:
	gcc -shared -fPIC -O3 -std=c99 -o bin/8051_ASM.so 8051_ASM.c
	gcc -shared -fPIC -Os -std=c99 -o bin/file_reader.so file_reader.c
	gcc -ldl -fPIC -Os -std=c99 -o bin/launcher Launcher/launcher.c
	chmod +x bin/launcher

debug:
	gcc -shared -fPIC -std=c99 -o bin/8051_ASM.so 8051_ASM.c
	gcc -shared -fPIC -std=c99 -o bin/file_reader.so file_reader.c
	gcc -ldl -fPIC -std=c99 -o bin/launcher Launcher/launcher.c
	chmod +x bin/launcher

preprocessor:
	gcc -E -std=c99 *.c | sed 's/^#/\/\//' > *.pp.c
