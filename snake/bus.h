#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <iostream>
#include "rom.h"
#include "global.h"
const uint16_t RAM = 0x0000;
const uint16_t RAM_END = 0x1FFF;

const uint16_t PPU_REGISTERS = 0x2000;
const uint16_t PPU_REGISTERS_END = 0x3FFF;

struct Bus : public Mem
{
    uint8_t cpu_vram[2048] = {};
    Rom rom;
    Bus(){};
    explicit Bus(Rom rom) : rom(rom) {};

    uint8_t mem_read(uint16_t address) override;
    void mem_write(uint16_t address, uint8_t value) override;
    uint8_t read_prog_rom(uint16_t address);
};

#endif // !BUS_H