#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>
#include "opcode.h"
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

struct CPU_flags
{
    const uint8_t CARRY = 0b00000001;
    const uint8_t ZERO = 0b00000010;
    const uint8_t INTERRUPT = 0b00000100;
    const uint8_t DECIMAL_UNUSED = 0b00001000;
    const uint8_t BREAK = 0b00010000;
    const uint8_t BREAK2 = 0b00100000;
    const uint8_t OVERFLOW = 0b01000000;
    const uint8_t NEGATIVE = 0b10000000;
};
struct CPU
{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    uint8_t status;
    uint16_t pc;
    uint8_t memory[0xFFFF];

    CPU() : register_a(0), register_x(0), register_y(0), status(0), pc(0){};
    uint8_t mem_read(uint16_t address);
    void mem_write(uint16_t address, uint8_t value);
    uint16_t mem_read_u16(uint16_t address);
    void mem_write_u16(uint16_t address, uint16_t value);
    void reset();
    void load_and_run(std::vector<uint8_t> program);
    void load(std::vector<uint8_t> program);
    void run();
    void set_zero_and_negative_flags(uint8_t register_value);
    void lda(AddressingMode mode);
    void tax();
    void inx();
    void sta(AddressingMode mode);

    uint16_t get_operand_address(AddressingMode mode);
};

#endif // !CPU_H