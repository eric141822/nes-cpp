#include "cpu.h"
#include <iostream>
// helpers.
namespace NES
{

    void CPU::stack_push(u8 val)
    {
        mem_write(STACK_START + static_cast<u16>(stack_pointer), val);
        stack_pointer -= 1;
    }

    u8 CPU::stack_pop()
    {
        stack_pointer += 1;
        return mem_read(STACK_START + static_cast<u16>(stack_pointer));
    }

    void CPU::stack_push_u16(u16 val)
    {
        u8 lo = static_cast<u8>(val & 0xFF);
        u8 hi = static_cast<u8>(val >> 8);
        stack_push(hi);
        stack_push(lo);
    }

    u16 CPU::stack_pop_u16()
    {
        // reversal of stack_push_u16.
        u16 lo = static_cast<u16>(stack_pop());
        u16 hi = static_cast<u16>(stack_pop());
        return (hi << 8) | lo;
    }

    u8 CPU::mem_read(u16 address)
    {
        return bus.mem_read(address);
    }

    void CPU::mem_write(u16 address, u8 value)
    {
        bus.mem_write(address, value);
    }

    // Little-endian read.
    u16 CPU::mem_read_u16(u16 address)
    {
        return bus.mem_read_u16(address);
    }

    // Little-endian write.
    void CPU::mem_write_u16(u16 address, u16 value)
    {
        bus.mem_write_u16(address, value);
    }

    void CPU::reset()
    {
        init_op_codes_map();
        register_a = 0;
        register_x = 0;
        register_y = 0;
        status = STATUS_RESET;
        stack_pointer = STACK_RESET;
        pc = mem_read_u16(0xFFFC);
    }

    void CPU::load_and_run(std::vector<u8> program)
    {
        init_op_codes_map();
        load(program);
        reset();
        run();
    }

    // modified for snake game.
    void CPU::load(std::vector<u8> program)
    {
        init_op_codes_map();
        for (size_t i = 0; i < program.size(); ++i)
        {
            mem_write(0x8600 + static_cast<u16>(i), program[i]);
        }
        mem_write_u16(0xFFFC, 0x8600);
    }

    void CPU::run()
    {
        run_with_callback([](CPU &) {});
    }

    void CPU::run_with_callback(std::function<void(CPU &)> callback)
    {
        while (true)
        {
            callback(*this);

            u8 code = mem_read(pc);

            pc++;

            u16 pc_state = pc;

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
                lda(opcode.mode);
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
                sta(opcode.mode);
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
                adc(opcode.mode);
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
                sbc(opcode.mode);
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
                and_op(opcode.mode);
                break;
            }

            // ASL Accumulator
            case 0x0A:
            {
                asl_acc();
                break;
            }
            case 0x06:
            case 0x16:
            case 0x0E:
            case 0x1E:
            {
                asl(opcode.mode);
                break;
            }

            // BCC
            case 0x90:
            {
                branch(!(status & cpu_flags::CARRY));
                break;
            }

            // BCS
            case 0xB0:
            {
                branch((status & cpu_flags::CARRY));
                break;
            }

            // BEQ
            case 0xF0:
            {
                branch((status & cpu_flags::ZERO));
                break;
            }

            // BIT
            case 0x24:
            case 0x2C:
            {
                bit(opcode.mode);
                break;
            }

            // BMI
            case 0x30:
            {
                branch((status & cpu_flags::NEGATIVE));
                break;
            }

            // BNE
            case 0xD0:
            {
                branch(!(status & cpu_flags::ZERO));
                break;
            }

            // BPL
            case 0x10:
            {
                branch(!(status & cpu_flags::NEGATIVE));
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
                branch(!(status & cpu_flags::OVERFLW));
                break;
            }

            // BVS
            case 0x70:
            {
                branch((status & cpu_flags::OVERFLW));
                break;
            }

            // CLC
            case 0x18:
            {
                status &= ~cpu_flags::CARRY;
                break;
            }

            // CLD
            case 0xD8:
            {
                status &= ~cpu_flags::DECIMAL_UNUSED;
                break;
            }

            // CLI
            case 0x58:
            {
                status &= ~cpu_flags::INTERRUPT;
                break;
            }

            // CLV
            case 0xB8:
            {
                status &= ~cpu_flags::OVERFLW;
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
                cmp_op(opcode.mode, register_a);
                break;
            }

            // CPX
            case 0xE0:
            case 0xE4:
            case 0xEC:
            {
                cmp_op(opcode.mode, register_x);
                break;
            }

            // CPY
            case 0xC0:
            case 0xC4:
            case 0xCC:
            {
                cmp_op(opcode.mode, register_y);
                break;
            }

            // DEC
            case 0xC6:
            case 0xD6:
            case 0xCE:
            case 0xDE:
            {
                dec(opcode.mode);
                break;
            }

            // DEX
            case 0xCA:
            {
                dex();
                break;
            }

            // DEY
            case 0x88:
            {
                dey();
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
                eor(opcode.mode);
                break;
            }

            // INC
            case 0xE6:
            case 0xF6:
            case 0xEE:
            case 0xFE:
            {
                inc(opcode.mode);
                break;
            }

            // INX
            case 0xE8:
            {
                inx();
                break;
            }

            // INY
            case 0xC8:
            {
                iny();
                break;
            }

            // JMP Absolute
            case 0x4C:
            {
                jmp_abs();
                break;
            }

            // JMP Indirect
            case 0x6C:
            {
                jmp();
                break;
            }

            // JSR
            case 0x20:
            {
                jsr();
                break;
            }

            // LDX
            case 0xA2:
            case 0xA6:
            case 0xB6:
            case 0xAE:
            case 0xBE:
            {
                ldx(opcode.mode);
                break;
            }

            // LDY
            case 0xA0:
            case 0xA4:
            case 0xB4:
            case 0xAC:
            case 0xBC:
            {
                ldy(opcode.mode);
                break;
            }

            // LSR
            case 0x4A:
            {
                lsr_acc();
                break;
            }
            case 0x46:
            case 0x56:
            case 0x4E:
            case 0x5E:
            {
                lsr(opcode.mode);
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
                ora(opcode.mode);
                break;
            }

            // PHA
            case 0x48:
            {
                pha();
                break;
            }

            // PHP
            case 0x08:
            {
                php();
                break;
            }

            // PLA
            case 0x68:
            {
                pla();
                break;
            }

            // PLP
            case 0x28:
            {
                plp();
                break;
            }

            // ROL Accumulator
            case 0x2A:
            {
                rol_acc();
                break;
            }

            // ROL
            case 0x26:
            case 0x36:
            case 0x2E:
            case 0x3E:
            {
                rol(opcode.mode);
                break;
            }

            // ROR Accumulator
            case 0x6A:
            {
                ror_acc();
                break;
            }

            // ROR
            case 0x66:
            case 0x76:
            case 0x6E:
            case 0x7E:
            {
                ror(opcode.mode);
                break;
            }

            // RTI
            case 0x40:
            {
                rti();
                break;
            }

            // RTS
            case 0x60:
            {
                rts();
                break;
            }

            // SEC
            case 0x38:
            {
                status |= cpu_flags::CARRY;
                break;
            }

            // SED
            case 0xF8:
            {
                status |= cpu_flags::DECIMAL_UNUSED;
                break;
            }

            // SEI
            case 0x78:
            {
                status |= cpu_flags::INTERRUPT;
                break;
            }

            // STX
            case 0x86:
            case 0x96:
            case 0x8E:
            {
                stx(opcode.mode);
                break;
            }

            // STY
            case 0x84:
            case 0x94:
            case 0x8C:
            {
                sty(opcode.mode);
                break;
            }

            // TAX
            case 0xAA:
            {
                tax();
                break;
            }

            // TAY
            case 0xA8:
            {
                tay();
                break;
            }

            // TSX
            case 0xBA:
            {
                tsx();
                break;
            }

            // TXA
            case 0x8A:
            {
                txa();
                break;
            }

            // TXS
            case 0x9A:
            {
                txs();
                break;
            }

            // TYA
            case 0x98:
            {
                tya();
                break;
            }

            default:
            {
                std::cerr << "Not implemented: " << std::hex << static_cast<int>(code) << std::endl;
                exit(1);
                // break;
            }
            }
            if (pc == pc_state)
            {
                pc += static_cast<u16>((opcode.len - 1));
            }

            // callback(*this);
        }
    };

    void CPU::set_zero_and_negative_flags(u8 register_value)
    {
        if (register_value == 0)
        {
            status |= cpu_flags::ZERO;
        }
        else
        {
            status &= ~cpu_flags::ZERO;
        }

        if ((register_value >> 7) == 1)
        {
            status |= cpu_flags::NEGATIVE;
        }
        else
        {
            status &= ~cpu_flags::NEGATIVE;
        }
    }

    void CPU::lda(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        set_register_a(val);
    }

    void CPU::tax()
    {
        register_x = register_a;
        set_zero_and_negative_flags(register_x);
    }

    void CPU::inx()
    {
        register_x += 1;
        set_zero_and_negative_flags(register_x);
    }

    u16 CPU::get_abs_address(AddressingMode mode, u16 begin)
    {
        switch (mode)
        {
        case ZeroPage:
            return static_cast<u16>(mem_read(begin));
        case Absolute:
            return mem_read_u16(begin);
        case ZeroPageX:
        {
            u8 pos = mem_read(begin);
            u16 addr = static_cast<u16>((pos + register_x));
            return addr;
        }
        case ZeroPageY:
        {
            u8 pos = mem_read(begin);
            u16 addr = static_cast<u16>((pos + register_y));
            return addr;
        }
        case AbsoluteX:
        {
            u16 base = mem_read_u16(begin);
            u16 addr = base + register_x;
            return addr;
        }
        case AbsoluteY:
        {
            u16 base = mem_read_u16(begin);
            u16 addr = base + register_y;
            return addr;
        }
        case IndirectX:
        {
            u8 base = mem_read(begin);
            u8 ptr = base + register_x;
            u16 lo = mem_read(static_cast<u16>(ptr));
            u16 hi = mem_read(static_cast<u16>(ptr + 1));
            return (hi << 8) | lo;
        }
        case IndirectY:
        {
            u8 base = mem_read(begin);
            u16 lo = mem_read(static_cast<u16>(base));
            u16 hi = mem_read(static_cast<u16>(base + 1));
            u16 deref_base = (hi << 8) | lo;
            u16 deref = deref_base + static_cast<u16>(register_y);
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

    u16 CPU::get_operand_address(AddressingMode mode)
    {
        switch (mode)
        {
        case Immediate:
            return pc;
        default:
            return get_abs_address(mode, pc);
        }
    }

    void CPU::sta(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        mem_write(addr, register_a);
    }

    void CPU::set_register_a(u8 val)
    {
        register_a = val;
        set_zero_and_negative_flags(register_a);
    }

    void CPU::set_register_x(u8 val)
    {
        register_x = val;
        set_zero_and_negative_flags(register_x);
    }

    void CPU::set_register_y(u8 val)
    {
        register_y = val;
        set_zero_and_negative_flags(register_y);
    }

    void CPU::add_to_register_a(u8 val)
    {
        // add 1 if carry flag is set.
        u16 result = static_cast<u16>(register_a) +
                          static_cast<u16>(val) + static_cast<u16>(status & cpu_flags::CARRY);

        // set carry flag if result is greater than 255, else unset.
        bool carry = result > 0xFF;
        if (carry)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        u8 result8 = static_cast<u8>(result);

        // set overflow flag if result is greater than 127, else unset.
        if (((val ^ result8) & (result8 ^ register_a) & 0x80) != 0)
        {
            status |= cpu_flags::OVERFLW;
        }
        else
        {
            status &= ~cpu_flags::OVERFLW;
        }

        set_register_a(result8);
    }

    void CPU::adc(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        add_to_register_a(val);
    }

    void CPU::sbc(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        int8_t val = static_cast<int8_t>(mem_read(addr));
        add_to_register_a(static_cast<u8>((-val - 1)));
    }

    void CPU::and_op(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        set_register_a(val & register_a);
    }

    void CPU::asl_acc()
    {
        // set CARRY.
        u8 data = register_a;
        if ((data >> 7) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        data <<= 1;
        set_register_a(data);
    }

    u8 CPU::asl(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);

        // set CARRY.
        if ((val >> 7) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        val <<= 1;
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::branch(bool cond)
    {
        if (cond)
        {
            int8_t jump = static_cast<int8_t>(mem_read(pc));
            u16 jump_addr = pc + 1 + static_cast<u16>(jump);
            pc = jump_addr;
        }
    }

    void CPU::bit(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        u8 result = register_a & val;
        if (result == 0)
        {
            status |= cpu_flags::ZERO;
        }
        else
        {
            status &= ~cpu_flags::ZERO;
        }

        if ((val & 0b1000'0000) > 0)
        {
            status |= cpu_flags::NEGATIVE;
        }

        if ((val & 0b0100'0000) > 0)
        {
            status |= cpu_flags::OVERFLW;
        }
    }

    void CPU::cmp_op(AddressingMode mode, u8 reg)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);

        if (val <= reg)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }
        set_zero_and_negative_flags((reg - val));
    }

    u8 CPU::dec(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        val--;
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::dex()
    {
        register_x--;
        set_zero_and_negative_flags(register_x);
    }

    void CPU::dey()
    {
        register_y--;
        set_zero_and_negative_flags(register_y);
    }

    void CPU::eor(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        set_register_a(val ^ register_a);
    }

    u8 CPU::inc(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        val += 1;
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::iny()
    {
        register_y += 1;
        set_zero_and_negative_flags(register_y);
    }

    void CPU::jmp_abs()
    {
        u16 jump_addr = mem_read_u16(pc);
        pc = jump_addr;
    }

    // Indirect jump
    void CPU::jmp()
    {
        u16 mem_addr = mem_read_u16(pc);
        if ((mem_addr & 0x00FF) == 0x00FF)
        {
            u8 lo = mem_read(mem_addr);
            u8 hi = mem_read(mem_addr & 0xFF00);
            u16 jump_addr = (static_cast<u16>(hi) << 8) | static_cast<u16>(lo);
            pc = jump_addr;
        }
        else
        {
            u16 jump_addr = mem_read_u16(mem_addr);
            pc = jump_addr;
        }
    }

    void CPU::jsr()
    {
        stack_push_u16((pc + 2 - 1));
        u16 jump_addr = mem_read_u16(pc);
        pc = jump_addr;
    }

    void CPU::ldx(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        register_x = val;
        set_zero_and_negative_flags(register_x);
    }

    void CPU::ldy(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        register_y = val;
        set_zero_and_negative_flags(register_y);
    }

    // LSR for accumulator.
    void CPU::lsr_acc()
    {
        u8 data = register_a;
        // old bit 0 is the new carry.
        if ((data & 1) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }
        data >>= 1;
        set_register_a(data);
    }

    u8 CPU::lsr(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);

        // old bit 0 is the new carry.
        if ((val & 1) != 0)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }
        val >>= 1;
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::ora(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);
        set_register_a(val | register_a);
    }

    void CPU::pha()
    {
        stack_push(register_a);
    }

    // check: https://www.nesdev.org/wiki/Status_flags#The_B_flag
    // php sets BREAK and UNUSED flag to 1.
    // none are these flags are used by the processor itself.
    void CPU::php()
    {
        u8 status = this->status;
        status |= cpu_flags::BREAK;
        status |= cpu_flags::UNUSED;
        stack_push(status);
    }

    void CPU::pla()
    {
        u8 data = stack_pop();
        set_register_a(data);
    }

    void CPU::plp()
    {
        status = stack_pop();
        status &= ~cpu_flags::BREAK;
        status |= cpu_flags::UNUSED; // reset this bit to 1.
    }

    // rotate to left, the accumulator.
    void CPU::rol_acc()
    {
        // old bit 7 becomes new carry.
        // bit 0 is the old carry.
        u8 data = register_a;
        bool old_carry = (status & cpu_flags::CARRY) == 1;

        if ((data >> 7) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        data <<= 1;
        if (old_carry)
        {
            data |= 1;
        }
        set_register_a(data);
    }

    u8 CPU::rol(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);

        // old bit 7 becomes new carry.
        // bit 0 is the old carry.
        bool old_carry = (status & cpu_flags::CARRY) == 1;

        if ((val >> 7) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        val <<= 1;
        if (old_carry)
        {
            val |= 1;
        }
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::ror_acc()
    {
        // old bit 0 becomes new carry.
        // bit 7 is the old carry.
        u8 data = register_a;
        bool old_carry = (status & cpu_flags::CARRY) == 1;

        if ((data & 1) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        data >>= 1;
        if (old_carry)
        {
            data |= 0b1000'0000;
        }
        set_register_a(data);
    }

    u8 CPU::ror(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        u8 val = mem_read(addr);

        bool old_carry = (status & cpu_flags::CARRY) == 1;
        if ((val & 1) == 1)
        {
            status |= cpu_flags::CARRY;
        }
        else
        {
            status &= ~cpu_flags::CARRY;
        }

        val >>= 1;
        if (old_carry)
        {
            val |= 0b1000'0000;
        }
        mem_write(addr, val);
        set_zero_and_negative_flags(val);
        return val;
    }

    void CPU::rti()
    {
        status = stack_pop();
        status &= ~cpu_flags::BREAK;
        status |= cpu_flags::UNUSED; // reset this bit to 1.
        pc = stack_pop_u16();
    }

    void CPU::rts()
    {
        pc = stack_pop_u16() + 1;
    }

    void CPU::stx(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        mem_write(addr, register_x);
    }

    void CPU::sty(AddressingMode mode)
    {
        u16 addr = get_operand_address(mode);
        mem_write(addr, register_y);
    }

    void CPU::tay()
    {
        register_y = register_a;
        set_zero_and_negative_flags(register_y);
    }

    void CPU::tsx()
    {
        register_x = stack_pointer;
        set_zero_and_negative_flags(register_x);
    }

    void CPU::txa()
    {
        register_a = register_x;
        set_zero_and_negative_flags(register_a);
    }

    void CPU::txs()
    {
        stack_pointer = register_x;
    }

    void CPU::tya()
    {
        register_a = register_y;
        set_zero_and_negative_flags(register_a);
    }
}