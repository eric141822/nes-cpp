#include "cpu.h"
#include <iostream>
uint8_t CPU::mem_read(uint16_t address)
{
    return this->memory[address];
}

void CPU::mem_write(uint16_t address, uint8_t value)
{
    this->memory[address] = value;
}

// Little-endian read.
uint16_t CPU::mem_read_u16(uint16_t address)
{
    uint16_t lo = (uint16_t)this->mem_read(address);
    uint16_t hi = (uint16_t)this->mem_read(address + 1);
    return (hi << 8) | lo;
}

// Little-endian write.
void CPU::mem_write_u16(uint16_t address, uint16_t value)
{
    uint8_t lo = (uint8_t)value & 0xFF;
    uint8_t hi = (uint8_t)(value >> 8);
    this->mem_write(address, lo);
    this->mem_write(address + 1, hi);
}

void CPU::reset()
{
    this->register_a = 0;
    this->register_x = 0;
    this->status = 0;

    this->pc = this->mem_read_u16(0xFFFC);
}

void CPU::load_and_run(std::vector<uint8_t> program)
{
    this->load(program);
    this->reset();
    this->run();
}

void CPU::load(std::vector<uint8_t> program)
{
    for (uint16_t i = 0x8000, j = 0; i < (0x8000 + program.size()); ++i, ++j)
    {
        this->memory[i] = program[j];
    }
    this->mem_write_u16(0xFFFC, 0x8000);
}

void CPU::run()
{
    while (true)
    {
        int opcode = this->memory[this->pc];
        this->pc++;
        switch (opcode)
        {
        // LDA
        case 0xA9:
        {
            uint8_t param = this->memory[this->pc];
            this->pc++;
            this->lda(param);
            break;
        }
        // BRK
        case 0x00:
        {
            return;
        }
        case 0xAA:
        {
            this->tax();
            break;
        }
        case 0xE8:
        {
            this->inx();
            break;
        }
        default:
        {
            break;
        }
        }
    }
};

void CPU::set_zero_and_negative_flags(uint8_t register_value)
{
    if (register_value == 0)
    {
        this->status |= 0b0000'0010;
    }
    else
    {
        this->status &= 0b1111'1101;
    }

    if ((register_value & 0b1000'0000) != 0)
    {
        this->status |= 0b1000'0000;
    }
    else
    {
        this->status &= 0b0111'1111;
    }
}

void CPU::lda(uint8_t param)
{
    this->register_a = param;
    this->set_zero_and_negative_flags(this->register_a);
}

void CPU::tax()
{
    this->register_x = this->register_a;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::inx()
{
    this->register_x++;
    this->set_zero_and_negative_flags(this->register_x);
}