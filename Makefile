.PHONY: run, clean

all: program

clean:
	rm -rf *.o *.out

run: program
	./program.out

program: main.cpp
	g++ -std=c++17 -Wall -Wextra -pedantic -O2 -o program.out main.cpp cpu.cpp