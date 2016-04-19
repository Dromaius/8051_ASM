default:
	g++ -O3 -std=c++11 -o 8051_ASM 8051_ASM.cpp
release:
	g++ -O3 -std=c++11 -o 8051_ASM 8051_ASM.cpp
debug:
	g++ -std=c++11 -o 8051_ASM 8051_ASM.cpp
