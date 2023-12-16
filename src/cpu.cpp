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
    uint16_t lo = static_cast<uint16_t>(this->mem_read(address));
    uint16_t hi = static_cast<uint16_t>(this->mem_read(address + 1));
    return (hi << 8) | lo;
}

// Little-endian write.
void CPU::mem_write_u16(uint16_t address, uint16_t value)
{
    uint8_t lo = value & 0xFF;
    uint8_t hi = value >> 8;
    this->mem_write(address, lo);
    this->mem_write(address + 1, hi);
}

void CPU::reset()
{
    this->register_a = 0;
    this->register_x = 0;
    this->register_y = 0;
    this->status = 0;

    this->pc = this->mem_read_u16(0xFFFC);
}

void CPU::load_and_run(std::vector<uint8_t> program)
{
    init_op_codes_map();
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
        uint8_t code = this->mem_read(this->pc);
        this->pc++;
        uint8_t pc_state = pc;

        OpCode opcode = OP_CODES_MAP[code];

        switch (code)
        {
        // LDA
        case 0xA9:
        case 0xA5:
        case 0xB5:
        case 0xAD:
        case 0xBD:
        case 0xB9:
        case 0xA1:
        case 0xB1:
        {
            this->lda(opcode.mode);
            break;
        }
        case 0x85:
        case 0x95:
        case 0x8D:
        case 0x9D:
        case 0x99:
        case 0x81:
        case 0x91:
        {
            this->sta(opcode.mode);
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
        if (this->pc == pc_state)
        {
            this->pc += opcode.len - 1;
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

void CPU::lda(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    this->register_a = this->mem_read(addr);
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

uint16_t CPU::get_operand_address(AddressingMode mode)
{
    switch (mode)
    {
    case Immediate:
        return this->pc;
    case ZeroPage:
        return this->mem_read(this->pc);
    case Absolute:
        return this->mem_read_u16(this->pc);
    case ZeroPageX:
    {
        uint8_t pos = this->mem_read(this->pc);
        uint16_t addr = static_cast<uint16_t>(pos + this->register_x);
        return addr;
    }
    case ZeroPageY:
    {
        uint8_t pos = this->mem_read(this->pc);
        uint16_t addr = static_cast<uint16_t>(pos + this->register_y);
        return addr;
    }
    case AbsoluteX:
    {
        uint16_t pos = this->mem_read_u16(this->pc);
        uint16_t addr = pos + this->register_x;
        return addr;
    }
    case AbsoluteY:
    {
        uint16_t pos = this->mem_read_u16(this->pc);
        uint16_t addr = pos + this->register_y;
        return addr;
    }
    case IndirectX:
    {
        uint8_t base = this->mem_read(this->pc);

        uint16_t ptr = static_cast<uint16_t>(base + this->register_x);
        uint16_t lo = this->mem_read(ptr);
        uint16_t hi = this->mem_read(ptr + 1);
        return (hi << 8) | lo;
    }
    case IndirectY:
    {
        uint8_t base = this->mem_read(this->pc);
        uint16_t lo = this->mem_read(static_cast<uint16_t>(base));
        uint16_t hi = this->mem_read(static_cast<uint16_t>(base + 1));
        uint16_t deref_base = (hi << 8) | lo;
        uint16_t deref = deref_base + this->register_y;
        return deref;
    }
    case NoneAddressing:
    default:
    {
        // not supported, cout to stderr;
        std::cerr << "Not supported addressing mode: " << mode << std::endl;
        return -1;
    }
    }
}

void CPU::sta(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    this->mem_write(addr, this->register_a);
}

void CPU::set_register_a(uint8_t val)
{
    this->register_a = val;
    this->set_zero_and_negative_flags(this->register_a);
}

void CPU::add_to_register_a(uint8_t val)
{
    // add 1 if carry flag is set.
    uint16_t result = static_cast<uint16_t>(this->register_a) +
                      static_cast<uint16_t>(val) + static_cast<uint16_t>(this->status & CPU_FLAGS::CARRY);

    // set carry flag if result is greater than 255, else unset.
    if (result > 0xFF)
    {
        this->status |= CPU_FLAGS::CARRY;
    }
    else
    {
        this->status &= ~CPU_FLAGS::CARRY;
    }

    // set overflow flag if result is greater than 127, else unset.
    if ((val ^ result) & (result ^ this->register_a) & 0x80)
    {
        this->status |= CPU_FLAGS::OVERFLOW;
    }
    else
    {
        this->status &= ~CPU_FLAGS::OVERFLOW;
    }

    this->set_register_a(static_cast<uint8_t>(result));
}

void CPU::adc(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->add_to_register_a(val);
}

uint8_t wrapping_neg(uint8_t value)
{
    return static_cast<uint8_t>(-value);
}

void CPU::sbc(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->add_to_register_a(wrapping_neg(val) - 1);
}

void CPU::and_op(AddressingMode mode) {
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->set_register_a(this->register_a & val);
}

uint8_t CPU::asl(AddressingMode mode) {
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    // set CARRY.
    if ((val >> 7) == 1) {
        this->status |= CPU_FLAGS::CARRY;
    } else {
        this->status &= ~CPU_FLAGS::CARRY;
    }

    val <<= 1;
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}