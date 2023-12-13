# NES Emulator in C++

This is a NES emulator written in C++. Following the fantastic [write up](https://bugzmanov.github.io/nes_ebook/chapter_1.html) by [bugzmanov](https://github.com/bugzmanov/).

The project is orginally written in Rust, but I ported it to C++ for learning purposes.

## Build

This project uses CMake. If there is no `build` directory, create one:

`mkdir build`

Run the following commands to build the project:

`cd build && cmake .. && make`

For tests, run:

`make test`

## To-do List

- [] CPU (6502) - In progress.
- [] BUS
- [] NES ROM
- [] PPU
- [] GamePad
- [] APU
