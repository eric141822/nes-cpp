#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdint>
namespace NES
{
    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef int8_t i8;
    typedef int16_t i16;
    typedef int32_t i32;
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

        virtual u8 mem_read(u16 address) = 0;
        virtual void mem_write(u16 address, u8 value) = 0;

        virtual u16 mem_read_u16(u16 pos)
        {
            u16 lo = static_cast<u16>(mem_read(pos));
            u16 hi = static_cast<u16>(mem_read(pos + 1));
            return (hi << 8) | lo;
        }

        virtual void mem_write_u16(u16 pos, u16 data)
        {
            u8 hi = static_cast<u8>(data >> 8);
            u8 lo = static_cast<u8>(data & 0xFF);
            mem_write(pos, lo);
            mem_write(pos + 1, hi);
        }
    };
}

#endif // !GLOBAL_H