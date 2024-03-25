#include "bus.h"
namespace NES
{
    u8 Bus::mem_read(u16 address)
    {
        if (address >= RAM && address <= RAM_END)
        {
            u16 mirrored_addr = address & 0b00000111'11111111;
            return cpu_vram[mirrored_addr];
        }
        else if (address >= PPU_REGISTERS && address <= PPU_REGISTERS_END)
        {
            u16 _mirrored_addr = address & 0b00100000'00000111;
            // std::cout << "PPU not implemented yet\n";
            return 0x00;
        }
        else if (address >= 0x8000 && address <= 0xFFFF)
        {
            return read_prog_rom(address);
        }
        else
        {
            std::cout << "Ignoring mem read-access at address" << (int)address << "\n";
            return 0x00;
        }
    }

    void Bus::mem_write(u16 address, u8 value)
    {
        if (address >= RAM && address <= RAM_END)
        {
            u16 mirrored_addr = address & 0b11111111111;
            cpu_vram[mirrored_addr] = value;
        }
        else if (address >= PPU_REGISTERS && address <= PPU_REGISTERS_END)
        {
            u16 _mirrored_addr = address & 0b00100000'00000111;
            // std::cout << "PPU not implemented yet\n";
        }
        else if (address >= 0x8000 && address <= 0xFFFF)
        {
            std::cout << "Attempting to write to ROM space!\n";
            exit(1);
        }
        else
        {
            std::cout << "Ignoring mem write-access at address" << (int)address << "\n";
        }
    }

    u8 Bus::read_prog_rom(u16 address)
    {
        address -= 0x8000;

        // mirror address if needed.
        if ((rom.prg_rom.size() == 0x4000) && (address >= 0x4000))
        {
            address %= 0x4000;
        }

        return rom.prg_rom[address];
    }
}