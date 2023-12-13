.PHONY: run, clean

all: program

clean:
	rm -rf *.o *.out

test: tests/test.cpp
	g++ -std=c++17 -Wall -Wextra -pedantic -O2 -o test.out tests/test.cpp src/cpu.cpp src/opcode.cpp
	./test.out

run: program
	./program.out

program: src/main.cpp
	g++ -std=c++17 -Wall -Wextra -pedantic -O2 -o program.out src/main.cpp src/cpu.cpp src/opcode.cpp