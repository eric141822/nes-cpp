# NES Emulator in C++

This is a NES emulator written in C++. Following the fantastic [write up](https://bugzmanov.github.io/nes_ebook/chapter_1.html) by [bugzmanov](https://github.com/bugzmanov/).

The project is orginally written in Rust, but I ported it to C++ for learning purposes.

## Dependencies

This project needs SDL2 to run. Install it using your package manager.

### MacOS

`brew install sdl2`

### Debian/Ubuntu

`sudo apt-get install libsdl2-dev`

You will also need the [fmt](https://fmt.dev/latest/index.html) library to run the project. CMake has been configured to use [vcpkg](https://vcpkg.io/en/getting-started). Follow the instructions on the vcpkg website to install it. You may also opt to build fmt from source or use you choice of package manager, such as conan. 

### Install fmt using vcpkg

`vcpkg install fmt`

### Integrate vcpkg with CMake

Run CMake with `-DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake` during configuration.

## Run

Run the following command to run the snake game using the 6502 CPU emulator:

`./build/snake.out`

## To-do List

- [x] CPU (6502) with snake game.
- [x] BUS
- [x] NES ROM
- [] Trace Logger and NES Test ROM - In progress
- [] PPU
- [] GamePad
- [] APU
