#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <iostream>
#include "rom.h"
#include "global.h"
namespace NES
{

    const u16 RAM = 0x0000;
    const u16 RAM_END = 0x1FFF;

    const u16 PPU_REGISTERS = 0x2000;
    const u16 PPU_REGISTERS_END = 0x3FFF;

    struct Bus : public Mem
    {
        u8 cpu_vram[2048] = {};
        Rom rom;
        Bus(){};
        explicit Bus(Rom rom) : rom(rom){};

        u8 mem_read(u16 address) override;
        void mem_write(u16 address, u8 value) override;
        u8 read_prog_rom(u16 address);
    };
}

#endif // !BUS_H