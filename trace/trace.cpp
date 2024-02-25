#include "trace.h"
std::string trace(CPU &cpu)
{
    uint8_t code = cpu.mem_read(cpu.pc);
    OpCode ops = OP_CODES_MAP[code];

    uint16_t begin = cpu.pc;
    std::vector<uint8_t> dump;
    dump.push_back(code);

    uint16_t addr = (ops.mode == AddressingMode::Immediate || ops.mode == AddressingMode::NoneAddressing) ? 0 : cpu.get_abs_address(ops.mode, begin + 1);
    uint8_t stored_value = (ops.mode == AddressingMode::Immediate || ops.mode == AddressingMode::NoneAddressing) ? 0 : cpu.mem_read(addr);

    std::string tmp = "";
    switch (ops.len)
    {
    case 1:
    {
        switch (ops.opcode)
        {
        case 0x0a:
        case 0x4a:
        case 0x2a:
        case 0x6a:
        {
            tmp = "A ";
            break;
        }
        default:
            break;
        }
        break;
    }
    case 2:
    {
        uint8_t address = cpu.mem_read(begin + 1); // get address of operand.
        dump.push_back(address);

        switch (ops.mode)
        {
        case AddressingMode::Immediate:
        {
            tmp = fmt::format("#${:02X}", address);
            break;
        }
        case AddressingMode::ZeroPage:
        {
            tmp = fmt::format("${:02X} = {:02X}", addr, stored_value);
            break;
        }
        case AddressingMode::ZeroPageX:
        {
            tmp = fmt::format("${:02X},X @ {:02X} = {:02X}", address, addr, stored_value);
            break;
        }
        case AddressingMode::ZeroPageY:
        {
            tmp = fmt::format("${:02X},Y @ {:02X} = {:02X}", address, addr, stored_value);
            break;
        }
        case AddressingMode::IndirectX:
        {
            tmp = fmt::format("(${:02X},X) @ {:02X} = {:04X} = {:02X}", address, address + static_cast<uint16_t>(cpu.register_x), addr, stored_value);
            break;
        }
        case AddressingMode::IndirectY:
        {
            tmp = fmt::format("(${:02X}),Y = {:04X} @ {:04X} = {:02X}", address, addr - static_cast<uint16_t>(cpu.register_y), addr, stored_value);
            break;
        }
        case AddressingMode::NoneAddressing:
        {
            uint16_t jmp_addr = begin + 2 + static_cast<uint8_t>(address);
            tmp = fmt::format("${:04X}", jmp_addr);
            break;
        }
        default:
        {
            std::cout << "Unknown addressing mode: " << ops.mode << std::endl;
            break;
        }
        }
        break;
    }
    case 3:
    {
        uint8_t address_lo = cpu.mem_read(begin + 1);
        uint8_t address_hi = cpu.mem_read(begin + 2);
        dump.push_back(address_lo);
        dump.push_back(address_hi);
        uint16_t address = cpu.mem_read_u16(begin + 1);
        switch (ops.mode)
        {
        case AddressingMode::NoneAddressing:
        {
            if (ops.opcode == 0x6c)
            {
                // indirect jmp;
                uint16_t jmp_addr = ((address & 0x00ff) == 0x00ff) ? static_cast<uint16_t>(cpu.mem_read(address & 0xff00)) << 8 | static_cast<uint16_t>(cpu.mem_read(address)) : cpu.mem_read_u16(address);
                tmp = fmt::format("(${:04X}) = {:04X}", address, jmp_addr);
            }
            else
            {
                tmp = fmt::format("${:04X}", address);
            }
            break;
        }
        case AddressingMode::Absolute:
        {
            tmp = fmt::format("${:04X} = {:02X}", address, stored_value);
            break;
        }
        case AddressingMode::AbsoluteX:
        {
            tmp = fmt::format("${:04X},X @ {:04X} = {:02X}", address, addr, stored_value);
            break;
        }
        case AddressingMode::AbsoluteY:
        {
            tmp = fmt::format("${:04X},Y @ {:04X} = {:02X}", address, addr, stored_value);
            break;
        }
        default:
        {
            std::cout << "Unknown addressing mode: " << ops.mode << std::endl;
            break;
        }
        }
        break;
    }
    default:
    {
        exit(1);
        break;
    }
    }
    std::string hex_str = "";
    for (auto &i : dump)
    {
        hex_str += fmt::format("{:02X} ", i);
    }

    std::string asm_str = fmt::format("{:04X}  {:9}{:>4} {}", begin, hex_str, ops.code_name, tmp);

    return fmt::format("{:47} A:{:02X} X:{:02X} Y:{:02X} P:{:02X} SP:{:02X}",
                       asm_str, cpu.register_a, cpu.register_x, cpu.register_y, cpu.status, cpu.stack_pointer);
}