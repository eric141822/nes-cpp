# NES Emulator in C++

This is a NES emulator written in C++. Following the fantastic [write up](https://bugzmanov.github.io/nes_ebook/chapter_1.html) by [bugzmanov](https://github.com/bugzmanov/).

The project is orginally written in Rust, but I ported it to C++ for learning purposes.

## Build

This project uses CMake. If there is no `build` directory, create one:

`mkdir -p build`

Run the following commands to build the project:

`cd build && cmake .. && make`

For tests, run:

`make test`

This project needs SDL2 to run. Install it using your package manager.

### MacOS

`brew install sdl2`

### Debian/Ubuntu

`sudo apt-get install libsdl2-dev`

## Run

Run the following command to run the snake game using the 6502 CPU emulator:

`./build/snake.out`

## To-do List

- [x] CPU (6502) with snake game.
- [] BUS - IN PROGRESS
- [] NES ROM - IN PROGRESS
- [] PPU
- [] GamePad
- [] APU
