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
    std::copy(program.begin(), program.end(), this->memory + 0x0600);
    this->mem_write_u16(0xFFFC, 0x0600);
}

void CPU::run()
{
    this->run_with_callback([](CPU &) {});
}

void CPU::run_with_callback(std::function<void(CPU &)> callback)
{
    while (true)
    {
        // std::cerr << "PC at: " << std::hex << static_cast<int>(this->pc) << std::endl;
        uint8_t code = this->mem_read(this->pc);
        this->pc += 1;
        uint16_t pc_state = this->pc;

        OpCode opcode = OP_CODES_MAP[code];
        // std::cerr << "opcode: " << std::hex << static_cast<int>(opcode.opcode) << " with mode: " << opcode.mode << std::endl;

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

        default:
        {
            break;
        }
        }
        if (this->pc == pc_state)
        {
            this->pc += static_cast<uint16_t>((opcode.len - 1));
        }

        callback(*this);
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

uint16_t CPU::get_operand_address(AddressingMode mode)
{
    switch (mode)
    {
    case Immediate:
        return this->pc;
    case ZeroPage:
        return static_cast<uint16_t>(this->mem_read(this->pc));
    case Absolute:
        return this->mem_read_u16(this->pc);
    case ZeroPageX:
    {
        uint8_t pos = this->mem_read(this->pc);
        uint16_t addr = static_cast<uint16_t>((pos + this->register_x));
        return addr;
    }
    case ZeroPageY:
    {
        uint8_t pos = this->mem_read(this->pc);
        uint16_t addr = static_cast<uint16_t>((pos + this->register_y));
        return addr;
    }
    case AbsoluteX:
    {
        uint16_t base = this->mem_read_u16(this->pc);
        uint16_t addr = base + this->register_x;
        return addr;
    }
    case AbsoluteY:
    {
        uint16_t base = this->mem_read_u16(this->pc);
        uint16_t addr = base + this->register_y;
        return addr;
    }
    case IndirectX:
    {
        uint8_t base = this->mem_read(this->pc);
        uint8_t ptr = base + this->register_x;
        uint16_t lo = this->mem_read(static_cast<uint16_t>(ptr));
        uint16_t hi = this->mem_read(static_cast<uint16_t>(ptr + 1));
        return (hi << 8) | lo;
    }
    case IndirectY:
    {
        uint8_t base = this->mem_read(this->pc);
        uint16_t lo = this->mem_read(static_cast<uint16_t>(base));
        uint16_t hi = this->mem_read(static_cast<uint16_t>(base + 1));
        uint16_t deref_base = (hi << 8) | lo;
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

    if ((val & 0b0100'0000) > 0)
    {
        this->status |= cpu_flags::OVERFLW;
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
    if ((mem_addr & 0x00FF) == 0x00FF) {
        uint8_t lo = this->mem_read(mem_addr);
        uint8_t hi = this->mem_read(mem_addr & 0xFF00);
        uint16_t jump_addr = (static_cast<uint16_t>(hi) << 8) | static_cast<uint16_t>(lo);
        this->pc = jump_addr;
    } else {
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
