#include "bus.h"

uint8_t Bus::mem_read(uint16_t address)
{
    if (address >= RAM && address <= RAM_END)
    {
        uint16_t mirrored_addr = address & 0b00000111'11111111;
        return this->cpu_vram[mirrored_addr];
    }
    else if (address >= PPU_REGISTERS && address <= PPU_REGISTERS_END)
    {
        // uint16_t _mirrored_addr = address & 0b00100000'00000111;
        std::cout << "PPU not implemented yet\n";
        return 0x00;
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        return read_prog_rom(address);
    }
    else
    {
        std::cout << "Invalid address\n";
        return 0x00;
    }
}

void Bus::mem_write(uint16_t address, uint8_t value)
{
    if (address >= RAM && address <= RAM_END)
    {
        uint16_t mirrored_addr = address & 0b00000111'11111111;
        this->cpu_vram[mirrored_addr] = value;
    }
    else if (address >= PPU_REGISTERS && address <= PPU_REGISTERS_END)
    {
        // uint16_t _mirrored_addr = address & 0b00100000'00000111;
        std::cout << "PPU not implemented yet\n";
    }
    else if (address >= 0x8000 && address <= 0xFFFF)
    {
        std::cout << "Attempting to write to ROM space!\n";
        exit(1);
    }
    else
    {
        std::cout << "Invalid address\n";
    }
}

uint8_t Bus::read_prog_rom(uint16_t address)
{
    address -= 0x8000;

    // mirror address if needed.
    if ((this->rom.prg_rom.size() == 0x4000) && (address >= 0x4000))
    {
        address %= 0x4000;
    }

    return this->rom.prg_rom[address];
}