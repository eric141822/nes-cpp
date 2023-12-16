#include "opcode.h"

std::vector<OpCode> CPU_OP_CODES = {
    //BRK, TAX, INX
    OpCode(0x00, "BRK", 1, 7, AddressingMode::NoneAddressing),
    OpCode(0xAA, "TAX", 1, 2, AddressingMode::NoneAddressing),
    OpCode(0xE8, "INX", 1, 2, AddressingMode::NoneAddressing),

    //LDA
    OpCode(0xA9, "LDA", 2, 2, AddressingMode::Immediate),
    OpCode(0xA5, "LDA", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xB5, "LDA", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0xAD, "LDA", 3, 4, AddressingMode::Absolute),
    OpCode(0xBD, "LDA", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0xB9, "LDA", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0xA1, "LDA", 2, 6, AddressingMode::IndirectX),
    OpCode(0xB1, "LDA", 2, 5, AddressingMode::IndirectY),

    //STA
    OpCode(0x85, "STA", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x95, "STA", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x8D, "STA", 3, 4, AddressingMode::Absolute),
    OpCode(0x9D, "STA", 3, 5, AddressingMode::AbsoluteX),
    OpCode(0x99, "STA", 3, 5, AddressingMode::AbsoluteY),
    OpCode(0x81, "STA", 2, 6, AddressingMode::IndirectX),
    OpCode(0x91, "STA", 2, 6, AddressingMode::IndirectY),

    //ADC
    OpCode(0x69, "ADC", 2, 2, AddressingMode::Immediate),
    OpCode(0x65, "ADC", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x75, "ADC", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x6D, "ADC", 3, 4, AddressingMode::Absolute),
    OpCode(0x7D, "ADC", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0x79, "ADC", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0x61, "ADC", 2, 6, AddressingMode::IndirectX),
    OpCode(0x71, "ADC", 2, 5, AddressingMode::IndirectY),

    //SBC
    OpCode(0xE9, "SBC", 2, 2, AddressingMode::Immediate),
    OpCode(0xE5, "SBC", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xF5, "SBC", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0xED, "SBC", 3, 4, AddressingMode::Absolute),
    OpCode(0xFD, "SBC", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0xF9, "SBC", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0xE1, "SBC", 2, 6, AddressingMode::IndirectX),
    OpCode(0xF1, "SBC", 2, 5, AddressingMode::IndirectY),

    //AND
    OpCode(0x29, "AND", 2, 2, AddressingMode::Immediate),
    OpCode(0x25, "AND", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x35, "AND", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x2D, "AND", 3, 4, AddressingMode::Absolute),
    OpCode(0x3D, "AND", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0x39, "AND", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0x21, "AND", 2, 6, AddressingMode::IndirectX),
    OpCode(0x31, "AND", 2, 5, AddressingMode::IndirectY),

    //ASL
    OpCode(0x0A, "ASL", 1, 2, AddressingMode::NoneAddressing),
    OpCode(0x06, "ASL", 2, 5, AddressingMode::ZeroPage),
    OpCode(0x16, "ASL", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0x0E, "ASL", 3, 6, AddressingMode::Absolute),
    OpCode(0x1E, "ASL", 3, 7, AddressingMode::AbsoluteX),
    
};

std::map<uint8_t, OpCode> OP_CODES_MAP;

void init_op_codes_map()
{
    for (const auto &op_code : CPU_OP_CODES)
    {
        OP_CODES_MAP[op_code.opcode] = op_code;
    }
}