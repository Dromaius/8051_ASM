.PHONY: release debug preprocessor

default: release

release:
	gcc -shared -fPIC -O3 -std=c99 -o *.so *.c
	gcc -ldl -fPIC -Os -std=c99 -o launcher Launcher/launcher.c

debug:
	gcc -shared -fPIC -std=c99 -o *.so *.c
	gcc -ldl -fPIC -std=c99 -o launcher Launcher/launcher.c

preprocessor:
	gcc -E -std=c99 *.c | sed 's/^#/\/\//' > *.pp.c
