#include <iostream>
#include <cassert>
#include "cpu.h"
#include "trace.h"
#include <random>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <cassert>

const std::string FILE_NAME = "../trace/nestest.nes";

std::vector<uint8_t> read_rom(const std::string &file)
{
    std::ifstream rom_file(file, std::ios::binary);

    if (!rom_file)
    {
        std::cerr << "Failed to open file: " << file << std::endl;
        exit(1);
    }

    std::vector<uint8_t> rom_data((std::istreambuf_iterator<char>(rom_file)),
                                  std::istreambuf_iterator<char>());

    rom_file.close();

    return rom_data;
}

int main()
{
    std::vector<uint8_t> rom_code = read_rom(FILE_NAME);

    Rom rom(rom_code);
    Bus bus(rom);
    CPU cpu(bus);
    cpu.reset();
    cpu.pc = 0xc000;

    cpu.run_with_callback([&](CPU &cpu)
                          {
                              std::cout << trace(cpu) << std::endl;
                          });

    return 0;
}
