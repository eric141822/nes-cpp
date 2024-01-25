#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>

enum AddressingMode
{
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    IndirectX,
    IndirectY,
    NoneAddressing,
};

class Mem
{
public:
    virtual ~Mem() = default;

    virtual uint8_t mem_read(uint16_t address) = 0;
    virtual void mem_write(uint16_t address, uint8_t value) = 0;

    virtual uint16_t mem_read_u16(uint16_t pos)
    {
        uint16_t lo = static_cast<uint16_t>(mem_read(pos));
        uint16_t hi = static_cast<uint16_t>(mem_read(pos + 1));
        return (hi << 8) | lo;
    }

    virtual void mem_write_u16(uint16_t pos, uint16_t data)
    {
        uint8_t hi = static_cast<uint8_t>(data >> 8);
        uint8_t lo = static_cast<uint8_t>(data & 0xFF);
        mem_write(pos, lo);
        mem_write(pos + 1, hi);
    }
};

#endif // !GLOBAL_H