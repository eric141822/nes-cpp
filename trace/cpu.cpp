#include "cpu.h"
#include <iostream>
// helpers.
void CPU::stack_push(uint8_t val)
{
    this->mem_write(STACK_START + static_cast<uint16_t>(this->stack_pointer), val);
    this->stack_pointer -= 1;
}

uint8_t CPU::stack_pop()
{
    this->stack_pointer += 1;
    return this->mem_read(STACK_START + static_cast<uint16_t>(this->stack_pointer));
}

void CPU::stack_push_u16(uint16_t val)
{
    uint8_t lo = static_cast<uint8_t>(val & 0xFF);
    uint8_t hi = static_cast<uint8_t>(val >> 8);
    this->stack_push(hi);
    this->stack_push(lo);
}

uint16_t CPU::stack_pop_u16()
{
    // reversal of stack_push_u16.
    uint16_t lo = static_cast<uint16_t>(this->stack_pop());
    uint16_t hi = static_cast<uint16_t>(this->stack_pop());
    return (hi << 8) | lo;
}

uint8_t CPU::mem_read(uint16_t address)
{
    return bus.mem_read(address);
}

void CPU::mem_write(uint16_t address, uint8_t value)
{
    bus.mem_write(address, value);
}

// Little-endian read.
uint16_t CPU::mem_read_u16(uint16_t address)
{
    return bus.mem_read_u16(address);
}

// Little-endian write.
void CPU::mem_write_u16(uint16_t address, uint16_t value)
{
    bus.mem_write_u16(address, value);
}

void CPU::reset()
{
    init_op_codes_map();
    this->register_a = 0;
    this->register_x = 0;
    this->register_y = 0;
    this->status = STATUS_RESET;
    this->stack_pointer = STACK_RESET;
    this->pc = this->mem_read_u16(0xFFFC);
}

void CPU::load_and_run(std::vector<uint8_t> program)
{
    init_op_codes_map();
    this->load(program);
    this->reset();
    this->run();
}

// modified for snake game.
void CPU::load(std::vector<uint8_t> program)
{
    init_op_codes_map();
    for (size_t i = 0; i < program.size(); ++i)
    {
        this->mem_write(0x8600 + static_cast<uint16_t>(i), program[i]);
    }
    this->mem_write_u16(0xFFFC, 0x8600);
}

void CPU::run()
{
    this->run_with_callback([](CPU &) {});
}

void CPU::run_with_callback(std::function<void(CPU &)> callback)
{
    while (true)
    {
        callback(*this);

        uint8_t code = this->mem_read(this->pc);

        this->pc++;

        uint16_t pc_state = this->pc;

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

        // ADC
        case 0x69:
        case 0x65:
        case 0x75:
        case 0x6D:
        case 0x7D:
        case 0x79:
        case 0x61:
        case 0x71:
        {
            this->adc(opcode.mode);
            break;
        }

        // SBC
        case 0xE9:
        case 0xE5:
        case 0xF5:
        case 0xED:
        case 0xFD:
        case 0xF9:
        case 0xE1:
        case 0xF1:
        {
            this->sbc(opcode.mode);
            break;
        }

        // AND
        case 0x29:
        case 0x25:
        case 0x35:
        case 0x2D:
        case 0x3D:
        case 0x39:
        case 0x21:
        case 0x31:
        {
            this->and_op(opcode.mode);
            break;
        }

        // ASL Accumulator
        case 0x0A:
        {
            this->asl_acc();
            break;
        }
        case 0x06:
        case 0x16:
        case 0x0E:
        case 0x1E:
        {
            this->asl(opcode.mode);
            break;
        }

        // BCC
        case 0x90:
        {
            this->branch(!(this->status & cpu_flags::CARRY));
            break;
        }

        // BCS
        case 0xB0:
        {
            this->branch((this->status & cpu_flags::CARRY));
            break;
        }

        // BEQ
        case 0xF0:
        {
            this->branch((this->status & cpu_flags::ZERO));
            break;
        }

        // BIT
        case 0x24:
        case 0x2C:
        {
            this->bit(opcode.mode);
            break;
        }

        // BMI
        case 0x30:
        {
            this->branch((this->status & cpu_flags::NEGATIVE));
            break;
        }

        // BNE
        case 0xD0:
        {
            this->branch(!(this->status & cpu_flags::ZERO));
            break;
        }

        // BPL
        case 0x10:
        {
            this->branch(!(this->status & cpu_flags::NEGATIVE));
            break;
        }

        // BRK
        case 0x00:
        {
            return;
        }

        // BVC
        case 0x50:
        {
            this->branch(!(this->status & cpu_flags::OVERFLW));
            break;
        }

        // BVS
        case 0x70:
        {
            this->branch((this->status & cpu_flags::OVERFLW));
            break;
        }

        // CLC
        case 0x18:
        {
            this->status &= ~cpu_flags::CARRY;
            break;
        }

        // CLD
        case 0xD8:
        {
            this->status &= ~cpu_flags::DECIMAL_UNUSED;
            break;
        }

        // CLI
        case 0x58:
        {
            this->status &= ~cpu_flags::INTERRUPT;
            break;
        }

        // CLV
        case 0xB8:
        {
            this->status &= ~cpu_flags::OVERFLW;
            break;
        }

        // CMP
        case 0xC9:
        case 0xC5:
        case 0xD5:
        case 0xCD:
        case 0xDD:
        case 0xD9:
        case 0xC1:
        case 0xD1:
        {
            this->cmp_op(opcode.mode, this->register_a);
            break;
        }

        // CPX
        case 0xE0:
        case 0xE4:
        case 0xEC:
        {
            this->cmp_op(opcode.mode, this->register_x);
            break;
        }

        // CPY
        case 0xC0:
        case 0xC4:
        case 0xCC:
        {
            this->cmp_op(opcode.mode, this->register_y);
            break;
        }

        // DEC
        case 0xC6:
        case 0xD6:
        case 0xCE:
        case 0xDE:
        {
            this->dec(opcode.mode);
            break;
        }

        // DEX
        case 0xCA:
        {
            this->dex();
            break;
        }

        // DEY
        case 0x88:
        {
            this->dey();
            break;
        }

        // EOR
        case 0x49:
        case 0x45:
        case 0x55:
        case 0x4D:
        case 0x5D:
        case 0x59:
        case 0x41:
        case 0x51:
        {
            this->eor(opcode.mode);
            break;
        }

        // INC
        case 0xE6:
        case 0xF6:
        case 0xEE:
        case 0xFE:
        {
            this->inc(opcode.mode);
            break;
        }

        // INX
        case 0xE8:
        {
            this->inx();
            break;
        }

        // INY
        case 0xC8:
        {
            this->iny();
            break;
        }

        // JMP Absolute
        case 0x4C:
        {
            this->jmp_abs();
            break;
        }

        // JMP Indirect
        case 0x6C:
        {
            this->jmp();
            break;
        }

        // JSR
        case 0x20:
        {
            this->jsr();
            break;
        }

        // LDX
        case 0xA2:
        case 0xA6:
        case 0xB6:
        case 0xAE:
        case 0xBE:
        {
            this->ldx(opcode.mode);
            break;
        }

        // LDY
        case 0xA0:
        case 0xA4:
        case 0xB4:
        case 0xAC:
        case 0xBC:
        {
            this->ldy(opcode.mode);
            break;
        }

        // LSR
        case 0x4A:
        {
            this->lsr_acc();
            break;
        }
        case 0x46:
        case 0x56:
        case 0x4E:
        case 0x5E:
        {
            this->lsr(opcode.mode);
            break;
        }

        // NOP, NO OP.
        case 0xEA:
        {
            break;
        }

        // ORA
        case 0x09:
        case 0x05:
        case 0x15:
        case 0x0D:
        case 0x1D:
        case 0x19:
        case 0x01:
        case 0x11:
        {
            this->ora(opcode.mode);
            break;
        }

        // PHA
        case 0x48:
        {
            this->pha();
            break;
        }

        // PHP
        case 0x08:
        {
            this->php();
            break;
        }

        // PLA
        case 0x68:
        {
            this->pla();
            break;
        }

        // PLP
        case 0x28:
        {
            this->plp();
            break;
        }

        // ROL Accumulator
        case 0x2A:
        {
            this->rol_acc();
            break;
        }

        // ROL
        case 0x26:
        case 0x36:
        case 0x2E:
        case 0x3E:
        {
            this->rol(opcode.mode);
            break;
        }

        // ROR Accumulator
        case 0x6A:
        {
            this->ror_acc();
            break;
        }

        // ROR
        case 0x66:
        case 0x76:
        case 0x6E:
        case 0x7E:
        {
            this->ror(opcode.mode);
            break;
        }

        // RTI
        case 0x40:
        {
            this->rti();
            break;
        }

        // RTS
        case 0x60:
        {
            this->rts();
            break;
        }

        // SEC
        case 0x38:
        {
            this->status |= cpu_flags::CARRY;
            break;
        }

        // SED
        case 0xF8:
        {
            this->status |= cpu_flags::DECIMAL_UNUSED;
            break;
        }

        // SEI
        case 0x78:
        {
            this->status |= cpu_flags::INTERRUPT;
            break;
        }

        // STX
        case 0x86:
        case 0x96:
        case 0x8E:
        {
            this->stx(opcode.mode);
            break;
        }

        // STY
        case 0x84:
        case 0x94:
        case 0x8C:
        {
            this->sty(opcode.mode);
            break;
        }

        // TAX
        case 0xAA:
        {
            this->tax();
            break;
        }

        // TAY
        case 0xA8:
        {
            this->tay();
            break;
        }

        // TSX
        case 0xBA:
        {
            this->tsx();
            break;
        }

        // TXA
        case 0x8A:
        {
            this->txa();
            break;
        }

        // TXS
        case 0x9A:
        {
            this->txs();
            break;
        }

        // TYA
        case 0x98:
        {
            this->tya();
            break;
        }

        // unoffical opcodes.

        // DCP
        case 0xC7:
        case 0xD7:
        case 0xCF:
        case 0xDF:
        case 0xDB:
        case 0xC3:
        case 0xD3:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            data--;
            this->mem_write(addr, data);
            if (data <= this->register_a)
            {
                this->status |= cpu_flags::CARRY;
            }
            this->set_zero_and_negative_flags((this->register_a - data));
            break;
        }

        // RLA
        case 0x27:
        case 0x37:
        case 0x2F:
        case 0x3F:
        case 0x3B:
        case 0x23:
        case 0x33:
        {
            uint8_t data = this->rol(opcode.mode);
            this->register_a &= data;
            break;
        }

        // SLO
        case 0x07:
        case 0x17:
        case 0x0F:
        case 0x1F:
        case 0x1B:
        case 0x03:
        case 0x13:
        {
            uint8_t data = this->asl(opcode.mode);
            this->register_a |= data;
            break;
        }

        // SRE
        case 0x47:
        case 0x57:
        case 0x4F:
        case 0x5F:
        case 0x5B:
        case 0x43:
        case 0x53:
        {
            uint8_t data = this->lsr(opcode.mode);
            this->register_a ^= data;
            break;
        }

        // SKB (NOP)
        case 0x80:
        case 0x82:
        case 0x89:
        case 0xc2:
        case 0xe2:
        {
            break;
        }

        // AXS
        case 0xCB:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            uint8_t res = (this->register_a & this->register_x) - data;
            if (data <= (this->register_x & this->register_a))
            {
                this->status |= cpu_flags::CARRY;
            }
            this->set_zero_and_negative_flags(res);
            this->register_x = res;
            break;
        }

        // ARR
        case 0x6B:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->add_to_register_a(data);
            this->ror_acc();
            uint8_t res = this->register_a;

            // bit 6
            if ((res & 0b0100'0000) > 0)
            {
                this->status |= cpu_flags::CARRY;
            }
            else
            {
                this->status &= ~cpu_flags::CARRY;
            }

            // bit 6 ^ bit 5
            if (((res & 0b0100'0000) ^ (res & 0b0010'0000)) == 1)
            {
                this->status |= cpu_flags::OVERFLW;
            }
            else
            {
                this->status &= ~cpu_flags::OVERFLW;
            }

            this->set_zero_and_negative_flags(res);
            break;
        }

        // unoffical SBC
        case 0xEB:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->register_a--;
            break;
        }

        // ANC
        case 0x0B:
        case 0x2B:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->register_a &= data;
            if (this->status & cpu_flags::NEGATIVE)
            {
                this->status |= cpu_flags::CARRY;
            }
            else
            {
                this->status &= ~cpu_flags::CARRY;
            }
            break;
        }

        // ALR
        case 0x4B:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->register_a &= data;
            this->lsr_acc();
            break;
        }

        // NOP read
        case 0x04:
        case 0x44:
        case 0x64:
        case 0x14:
        case 0x34:
        case 0x54:
        case 0x74:
        case 0xD4:
        case 0xF4:
        case 0x0C:
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
        {
            this->mem_read(this->get_operand_address(opcode.mode));
            break;
        }

        // RRA
        case 0x67:
        case 0x77:
        case 0x6F:
        case 0x7F:
        case 0x7B:
        case 0x63:
        case 0x73:
        {
            uint8_t data = this->ror(opcode.mode);
            this->add_to_register_a(data);
            break;
        }

        // ISB
        case 0xE7:
        case 0xF7:
        case 0xEF:
        case 0xFF:
        case 0xFB:
        case 0xE3:
        case 0xF3:
        {
            uint8_t data = this->inc(opcode.mode);
            this->register_a -= data;
            break;
        }
        // NOP
        case 0x02:
        case 0x12:
        case 0x22:
        case 0x32:
        case 0x42:
        case 0x52:
        case 0x62:
        case 0x72:
        case 0x92:
        case 0xB2:
        case 0xD2:
        case 0xF2:
        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xDA:
        case 0xFA:
        {
            break;
        }

        // LAX
        case 0xA7:
        case 0xB7:
        case 0xAF:
        case 0xBF:
        case 0xA3:
        case 0xB3:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->set_register_a(data);
            this->register_x = this->register_a;
            break;
        }

        // SAX
        case 0x87:
        case 0x97:
        case 0x8F:
        case 0x83:
        {
            uint8_t data = this->register_a & this->register_x;
            uint8_t addr = this->get_operand_address(opcode.mode);
            this->mem_write(addr, data);
            break;
        }

        // LXA
        case 0xAB:
        {
            this->lda(opcode.mode);
            this->tax();
            break;
        }

        // XAA
        case 0x8B:
        {
            this->register_a = this->register_x;
            this->set_zero_and_negative_flags(this->register_a);
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            this->register_a &= data;
            break;
        }

        // LAS
        case 0xBB:
        {
            uint8_t addr = this->get_operand_address(opcode.mode);
            uint8_t data = this->mem_read(addr);
            data &= this->stack_pointer;
            this->register_a = data;
            this->register_x = data;
            this->stack_pointer = data;
            this->set_zero_and_negative_flags(data);
            break;
        }

        // TAS
        case 0x9B:
        {
            uint8_t data = this->register_a & this->register_x;
            this->stack_pointer = data;
            uint16_t mem_addr = this->mem_read_u16(this->pc) + static_cast<uint16_t>(this->register_y);
            data = (static_cast<uint8_t>((mem_addr >> 8)) + 1) & this->stack_pointer;
            this->mem_write(mem_addr, data);
            break;
        }

        // AHX Indirect Y
        case 0x93:
        {
            uint8_t pos = this->mem_read(this->pc);
            uint16_t mem_addr = this->mem_read_u16(pos) + static_cast<uint16_t>(this->register_y);
            uint8_t data = this->register_a & this->register_x & static_cast<uint8_t>((mem_addr >> 8));
            this->mem_write(mem_addr, data);
            break;
        }

        // AHX Absolute Y
        case 0x9F:
        {
            uint16_t mem_addr = this->mem_read_u16(this->pc) + static_cast<uint16_t>(this->register_y);
            uint8_t data = this->register_a & this->register_x & static_cast<uint8_t>((mem_addr >> 8));
            this->mem_write(mem_addr, data);
            break;
        }

        // SHX
        case 0x9E:
        {
            uint16_t mem_addr = this->mem_read_u16(this->pc) + static_cast<uint16_t>(this->register_y);
            uint8_t data = this->register_x & (static_cast<uint8_t>((mem_addr >> 8)) + 1);
            this->mem_write(mem_addr, data);
            break;
        }

        // SHY
        case 0x9C:
        {
            uint16_t mem_addr = this->mem_read_u16(this->pc) + static_cast<uint16_t>(this->register_x);
            uint8_t data = this->register_y & (static_cast<uint8_t>((mem_addr >> 8)) + 1);
            this->mem_write(mem_addr, data);
            break;
        }

        default:
        {
            std::cerr << "Not implemented: " << std::hex << static_cast<int>(code) << std::endl;
            exit(1);
            // break;
        }
        }
        if (this->pc == pc_state)
        {
            this->pc += static_cast<uint16_t>((opcode.len - 1));
        }

    }
};

void CPU::set_zero_and_negative_flags(uint8_t register_value)
{
    if (register_value == 0)
    {
        this->status |= cpu_flags::ZERO;
    }
    else
    {
        this->status &= ~cpu_flags::ZERO;
    }

    if ((register_value >> 7) == 1)
    {
        this->status |= cpu_flags::NEGATIVE;
    }
    else
    {
        this->status &= ~cpu_flags::NEGATIVE;
    }
}

void CPU::lda(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->set_register_a(val);
}

void CPU::tax()
{
    this->register_x = this->register_a;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::inx()
{
    this->register_x += 1;
    this->set_zero_and_negative_flags(this->register_x);
}

uint16_t CPU::get_abs_address(AddressingMode mode, uint16_t begin)
{
    switch (mode)
    {
    case ZeroPage:
        return static_cast<uint16_t>(this->mem_read(begin));
    case Absolute:
        return this->mem_read_u16(begin);
    case ZeroPageX:
    {
        uint8_t pos = this->mem_read(begin);
        uint16_t addr = static_cast<uint16_t>((pos + this->register_x));
        return addr;
    }
    case ZeroPageY:
    {
        uint8_t pos = this->mem_read(begin);
        uint16_t addr = static_cast<uint16_t>((pos + this->register_y));
        return addr;
    }
    case AbsoluteX:
    {
        uint16_t base = this->mem_read_u16(begin);
        uint16_t addr = base + static_cast<uint16_t>(this->register_x);
        return addr;
    }
    case AbsoluteY:
    {
        uint16_t base = this->mem_read_u16(begin);
        uint16_t addr = base + static_cast<uint16_t>(this->register_y);
        return addr;
    }
    case IndirectX:
    {
        uint8_t base = this->mem_read(begin);

        uint8_t ptr = static_cast<uint8_t>(base) + this->register_x;
        uint8_t lo = this->mem_read(static_cast<uint16_t>(ptr));
        uint8_t hi = this->mem_read(static_cast<uint16_t>((ptr + 1)));
        return (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
    }
    case IndirectY:
    {
        uint8_t base = this->mem_read(begin);
        uint8_t lo = this->mem_read(static_cast<uint16_t>(base));
        uint8_t hi = this->mem_read(static_cast<uint16_t>(base + 1));
        uint16_t deref_base = (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
        uint16_t deref = deref_base + static_cast<uint16_t>(this->register_y);
        return deref;
    }
    case NoneAddressing:
    default:
    {
        // not supported, cout to stderr;
        std::cerr << "Not supported addressing mode: " << mode << std::endl;
        exit(1);
    }
    }
}

uint16_t CPU::get_operand_address(AddressingMode mode)
{
    switch (mode)
    {
    case Immediate:
        return this->pc;
    default:
        return this->get_abs_address(mode, this->pc);
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

void CPU::set_register_x(uint8_t val)
{
    this->register_x = val;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::set_register_y(uint8_t val)
{
    this->register_y = val;
    this->set_zero_and_negative_flags(this->register_y);
}

void CPU::add_to_register_a(uint8_t val)
{
    // add 1 if carry flag is set.
    uint16_t result = static_cast<uint16_t>(this->register_a) +
                      static_cast<uint16_t>(val) + static_cast<uint16_t>(this->status & cpu_flags::CARRY);

    // set carry flag if result is greater than 255, else unset.
    bool carry = result > 0xFF;
    if (carry)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    uint8_t result8 = static_cast<uint8_t>(result);

    // set overflow flag if result is greater than 127, else unset.
    if (((val ^ result8) & (result8 ^ this->register_a) & 0x80) != 0)
    {
        this->status |= cpu_flags::OVERFLW;
    }
    else
    {
        this->status &= ~cpu_flags::OVERFLW;
    }

    this->set_register_a(result8);
}

void CPU::adc(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->add_to_register_a(val);
}

void CPU::sbc(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    int8_t val = static_cast<int8_t>(this->mem_read(addr));
    this->add_to_register_a(static_cast<uint8_t>((-val - 1)));
}

void CPU::and_op(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->set_register_a(val & this->register_a);
}

void CPU::asl_acc()
{
    // set CARRY.
    uint8_t data = this->register_a;
    if ((data >> 7) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    data <<= 1;
    this->set_register_a(data);
}

uint8_t CPU::asl(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    // set CARRY.
    if ((val >> 7) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    val <<= 1;
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::branch(bool cond)
{
    if (cond)
    {
        int8_t jump = static_cast<int8_t>(this->mem_read(this->pc));
        uint16_t jump_addr = this->pc + 1 + static_cast<uint16_t>(jump);
        this->pc = jump_addr;
    }
}

void CPU::bit(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    uint8_t result = this->register_a & val;
    if (result == 0)
    {
        this->status |= cpu_flags::ZERO;
    }
    else
    {
        this->status &= ~cpu_flags::ZERO;
    }

    if ((val & 0b1000'0000) > 0)
    {
        this->status |= cpu_flags::NEGATIVE;
    }
    else
    {
        this->status &= ~cpu_flags::NEGATIVE;
    }

    if ((val & 0b0100'0000) > 0)
    {
        this->status |= cpu_flags::OVERFLW;
    }
    else
    {
        this->status &= ~cpu_flags::OVERFLW;
    }
}

void CPU::cmp_op(AddressingMode mode, uint8_t reg)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    if (val <= reg)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }
    this->set_zero_and_negative_flags((reg - val));
}

uint8_t CPU::dec(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    val--;
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::dex()
{
    this->register_x--;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::dey()
{
    this->register_y--;
    this->set_zero_and_negative_flags(this->register_y);
}

void CPU::eor(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->set_register_a(val ^ this->register_a);
}

uint8_t CPU::inc(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    val += 1;
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::iny()
{
    this->register_y += 1;
    this->set_zero_and_negative_flags(this->register_y);
}

void CPU::jmp_abs()
{
    uint16_t jump_addr = this->mem_read_u16(this->pc);
    this->pc = jump_addr;
}

// Indirect jump
void CPU::jmp()
{
    uint16_t mem_addr = this->mem_read_u16(this->pc);
    if ((mem_addr & 0x00FF) == 0x00FF)
    {
        uint8_t lo = this->mem_read(mem_addr);
        uint8_t hi = this->mem_read(mem_addr & 0xFF00);
        uint16_t jump_addr = (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
        this->pc = jump_addr;
    }
    else
    {
        uint16_t jump_addr = this->mem_read_u16(mem_addr);
        this->pc = jump_addr;
    }
}

void CPU::jsr()
{
    this->stack_push_u16((this->pc + 2 - 1));
    uint16_t jump_addr = this->mem_read_u16(this->pc);
    this->pc = jump_addr;
}

void CPU::ldx(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->register_x = val;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::ldy(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->register_y = val;
    this->set_zero_and_negative_flags(this->register_y);
}

// LSR for accumulator.
void CPU::lsr_acc()
{
    uint8_t data = this->register_a;
    // old bit 0 is the new carry.
    if ((data & 1) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }
    data >>= 1;
    this->set_register_a(data);
}

uint8_t CPU::lsr(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    // old bit 0 is the new carry.
    if ((val & 1) != 0)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }
    val >>= 1;
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::ora(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);
    this->set_register_a(val | this->register_a);
}

void CPU::pha()
{
    this->stack_push(this->register_a);
}

// check: https://www.nesdev.org/wiki/Status_flags#The_B_flag
// php sets BREAK and UNUSED flag to 1.
// none are these flags are used by the processor itself.
void CPU::php()
{
    uint8_t status = this->status;
    status |= cpu_flags::BREAK;
    status |= cpu_flags::UNUSED;
    this->stack_push(status);
}

void CPU::pla()
{
    uint8_t data = this->stack_pop();
    this->set_register_a(data);
}

void CPU::plp()
{
    this->status = this->stack_pop();
    this->status &= ~cpu_flags::BREAK;
    this->status |= cpu_flags::UNUSED; // reset this bit to 1.
}

// rotate to left, the accumulator.
void CPU::rol_acc()
{
    // old bit 7 becomes new carry.
    // bit 0 is the old carry.
    uint8_t data = this->register_a;
    bool old_carry = (this->status & cpu_flags::CARRY) == 1;

    if ((data >> 7) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    data <<= 1;
    if (old_carry)
    {
        data |= 1;
    }
    this->set_register_a(data);
}

uint8_t CPU::rol(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    // old bit 7 becomes new carry.
    // bit 0 is the old carry.
    bool old_carry = (this->status & cpu_flags::CARRY) == 1;

    if ((val >> 7) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    val <<= 1;
    if (old_carry)
    {
        val |= 1;
    }
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::ror_acc()
{
    // old bit 0 becomes new carry.
    // bit 7 is the old carry.
    uint8_t data = this->register_a;
    bool old_carry = (this->status & cpu_flags::CARRY) == 1;

    if ((data & 1) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    data >>= 1;
    if (old_carry)
    {
        data |= 0b1000'0000;
    }
    this->set_register_a(data);
}

uint8_t CPU::ror(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    uint8_t val = this->mem_read(addr);

    bool old_carry = (this->status & cpu_flags::CARRY) == 1;
    if ((val & 1) == 1)
    {
        this->status |= cpu_flags::CARRY;
    }
    else
    {
        this->status &= ~cpu_flags::CARRY;
    }

    val >>= 1;
    if (old_carry)
    {
        val |= 0b1000'0000;
    }
    this->mem_write(addr, val);
    this->set_zero_and_negative_flags(val);
    return val;
}

void CPU::rti()
{
    this->status = this->stack_pop();
    this->status &= ~cpu_flags::BREAK;
    this->status |= cpu_flags::UNUSED; // reset this bit to 1.
    this->pc = this->stack_pop_u16();
}

void CPU::rts()
{
    this->pc = this->stack_pop_u16() + 1;
}

void CPU::stx(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    this->mem_write(addr, this->register_x);
}

void CPU::sty(AddressingMode mode)
{
    uint16_t addr = this->get_operand_address(mode);
    this->mem_write(addr, this->register_y);
}

void CPU::tay()
{
    this->register_y = this->register_a;
    this->set_zero_and_negative_flags(this->register_y);
}

void CPU::tsx()
{
    this->register_x = this->stack_pointer;
    this->set_zero_and_negative_flags(this->register_x);
}

void CPU::txa()
{
    this->register_a = this->register_x;
    this->set_zero_and_negative_flags(this->register_a);
}

void CPU::txs()
{
    this->stack_pointer = this->register_x;
}

void CPU::tya()
{
    this->register_a = this->register_y;
    this->set_zero_and_negative_flags(this->register_a);
}