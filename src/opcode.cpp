#include "opcode.h"

std::vector<OpCode> CPU_OP_CODES = {
    // BRK
    OpCode(0x00, "BRK", 1, 7, AddressingMode::NoneAddressing),

    // TAX
    OpCode(0xAA, "TAX", 1, 2, AddressingMode::NoneAddressing),

    // INX
    OpCode(0xE8, "INX", 1, 2, AddressingMode::NoneAddressing),

    // LDA
    OpCode(0xA9, "LDA", 2, 2, AddressingMode::Immediate),
    OpCode(0xA5, "LDA", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xB5, "LDA", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0xAD, "LDA", 3, 4, AddressingMode::Absolute),
    OpCode(0xBD, "LDA", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0xB9, "LDA", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0xA1, "LDA", 2, 6, AddressingMode::IndirectX),
    OpCode(0xB1, "LDA", 2, 5, AddressingMode::IndirectY),

    // STA
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

    // Branching
    OpCode(0x90, "BCC", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0xB0, "BCS", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0xF0, "BEQ", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0x30, "BMI", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0xD0, "BNE", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0x10, "BPL", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0x50, "BVC", 2, 2, AddressingMode::NoneAddressing),
    OpCode(0x70, "BVS", 2, 2, AddressingMode::NoneAddressing),

    // BIT
    OpCode(0x24, "BIT", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x2C, "BIT", 3, 4, AddressingMode::Absolute),

    // CLC
    OpCode(0x18, "CLC", 1, 2, AddressingMode::NoneAddressing),

    // CLD
    OpCode(0xD8, "CLD", 1, 2, AddressingMode::NoneAddressing),

    // CLI
    OpCode(0x58, "CLI", 1, 2, AddressingMode::NoneAddressing),

    // CLV
    OpCode(0xB8, "CLV", 1, 2, AddressingMode::NoneAddressing),

    // CMP
    OpCode(0xC9, "CMP", 2, 2, AddressingMode::Immediate),
    OpCode(0xC5, "CMP", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xD5, "CMP", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0xCD, "CMP", 3, 4, AddressingMode::Absolute),
    OpCode(0xDD, "CMP", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0xD9, "CMP", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0xC1, "CMP", 2, 6, AddressingMode::IndirectX),
    OpCode(0xD1, "CMP", 2, 5, AddressingMode::IndirectY),

    // CPX
    OpCode(0xE0, "CPX", 2, 2, AddressingMode::Immediate),
    OpCode(0xE4, "CPX", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xEC, "CPX", 3, 4, AddressingMode::Absolute),

    // CPY
    OpCode(0xC0, "CPY", 2, 2, AddressingMode::Immediate),
    OpCode(0xC4, "CPY", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xCC, "CPY", 3, 4, AddressingMode::Absolute),

    // DEC
    OpCode(0xC6, "DEC", 2, 5, AddressingMode::ZeroPage),
    OpCode(0xD6, "DEC", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0xCE, "DEC", 3, 6, AddressingMode::Absolute),
    OpCode(0xDE, "DEC", 3, 7, AddressingMode::AbsoluteX),

    // DEX
    OpCode(0xCA, "DEX", 1, 2, AddressingMode::NoneAddressing),

    // DEY
    OpCode(0x88, "DEY", 1, 2, AddressingMode::NoneAddressing),

    // EOR
    OpCode(0x49, "EOR", 2, 2, AddressingMode::Immediate),
    OpCode(0x45, "EOR", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x55, "EOR", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x4D, "EOR", 3, 4, AddressingMode::Absolute),
    OpCode(0x5D, "EOR", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0x59, "EOR", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0x41, "EOR", 2, 6, AddressingMode::IndirectX),
    OpCode(0x51, "EOR", 2, 5, AddressingMode::IndirectY),

    // INC
    OpCode(0xE6, "INC", 2, 5, AddressingMode::ZeroPage),
    OpCode(0xF6, "INC", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0xEE, "INC", 3, 6, AddressingMode::Absolute),
    OpCode(0xFE, "INC", 3, 7, AddressingMode::AbsoluteX),

    // INY
    OpCode(0xC8, "INY", 1, 2, AddressingMode::NoneAddressing),

    // JMP
    OpCode(0x4C, "JMP", 3, 3, AddressingMode::Absolute),
    OpCode(0x6C, "JMP", 3, 5, AddressingMode::NoneAddressing), // bugged indiret jump.

    // JSR
    OpCode(0x20, "JSR", 3, 6, AddressingMode::Absolute),

    // LDX
    OpCode(0xA2, "LDX", 2, 2, AddressingMode::Immediate),
    OpCode(0xA6, "LDX", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xB6, "LDX", 2, 4, AddressingMode::ZeroPageY),
    OpCode(0xAE, "LDX", 3, 4, AddressingMode::Absolute),
    OpCode(0xBE, "LDX", 3, 4, AddressingMode::AbsoluteY),
    
    // LDY
    OpCode(0xA0, "LDY", 2, 2, AddressingMode::Immediate),
    OpCode(0xA4, "LDY", 2, 3, AddressingMode::ZeroPage),
    OpCode(0xB4, "LDY", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0xAC, "LDY", 3, 4, AddressingMode::Absolute),
    OpCode(0xBC, "LDY", 3, 4, AddressingMode::AbsoluteX),

    // LSR
    OpCode(0x4A, "LSR", 1, 2, AddressingMode::NoneAddressing), // accumulator
    OpCode(0x46, "LSR", 2, 5, AddressingMode::ZeroPage),
    OpCode(0x56, "LSR", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0x4E, "LSR", 3, 6, AddressingMode::Absolute),
    OpCode(0x5E, "LSR", 3, 7, AddressingMode::AbsoluteX),

    // NOP
    OpCode(0xEA, "NOP", 1, 2, AddressingMode::NoneAddressing),

    // ORA
    OpCode(0x09, "ORA", 2, 2, AddressingMode::Immediate),
    OpCode(0x05, "ORA", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x15, "ORA", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x0D, "ORA", 3, 4, AddressingMode::Absolute),
    OpCode(0x1D, "ORA", 3, 4, AddressingMode::AbsoluteX),
    OpCode(0x19, "ORA", 3, 4, AddressingMode::AbsoluteY),
    OpCode(0x01, "ORA", 2, 6, AddressingMode::IndirectX),
    OpCode(0x11, "ORA", 2, 5, AddressingMode::IndirectY),

    // PHA
    OpCode(0x48, "PHA", 1, 3, AddressingMode::NoneAddressing),

    // PHP
    OpCode(0x08, "PHP", 1, 3, AddressingMode::NoneAddressing),

    // PLA
    OpCode(0x68, "PLA", 1, 4, AddressingMode::NoneAddressing),

    // PLP
    OpCode(0x28, "PLP", 1, 4, AddressingMode::NoneAddressing),

    // ROL
    OpCode(0x2A, "ROL", 1, 2, AddressingMode::NoneAddressing), // accumulator
    OpCode(0x26, "ROL", 2, 5, AddressingMode::ZeroPage),
    OpCode(0x36, "ROL", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0x2E, "ROL", 3, 6, AddressingMode::Absolute),
    OpCode(0x3E, "ROL", 3, 7, AddressingMode::AbsoluteX),
    
    // ROR
    OpCode(0x6A, "ROR", 1, 2, AddressingMode::NoneAddressing), // accumulator
    OpCode(0x66, "ROR", 2, 5, AddressingMode::ZeroPage),
    OpCode(0x76, "ROR", 2, 6, AddressingMode::ZeroPageX),
    OpCode(0x6E, "ROR", 3, 6, AddressingMode::Absolute),
    OpCode(0x7E, "ROR", 3, 7, AddressingMode::AbsoluteX),

    // RTI
    OpCode(0x40, "RTI", 1, 6, AddressingMode::NoneAddressing),

    // RTS
    OpCode(0x60, "RTS", 1, 6, AddressingMode::NoneAddressing),

    // SEC
    OpCode(0x38, "SEC", 1, 2, AddressingMode::NoneAddressing),

    // SED
    OpCode(0xF8, "SED", 1, 2, AddressingMode::NoneAddressing),

    // SEI
    OpCode(0x78, "SEI", 1, 2, AddressingMode::NoneAddressing),

    // STX
    OpCode(0x86, "STX", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x96, "STX", 2, 4, AddressingMode::ZeroPageY),
    OpCode(0x8E, "STX", 3, 4, AddressingMode::Absolute),

    // STY
    OpCode(0x84, "STY", 2, 3, AddressingMode::ZeroPage),
    OpCode(0x94, "STY", 2, 4, AddressingMode::ZeroPageX),
    OpCode(0x8C, "STY", 3, 4, AddressingMode::Absolute),

    // TAY
    OpCode(0xA8, "TAY", 1, 2, AddressingMode::NoneAddressing),

    // TSX
    OpCode(0xBA, "TSX", 1, 2, AddressingMode::NoneAddressing),

    // TXA
    OpCode(0x8A, "TXA", 1, 2, AddressingMode::NoneAddressing),

    // TXS
    OpCode(0x9A, "TXS", 1, 2, AddressingMode::NoneAddressing),

    // TYA
    OpCode(0x98, "TYA", 1, 2, AddressingMode::NoneAddressing),    



    
};

std::map<uint8_t, OpCode> OP_CODES_MAP;

void init_op_codes_map()
{
    for (const auto &op_code : CPU_OP_CODES)
    {
        OP_CODES_MAP[op_code.opcode] = op_code;
    }
}