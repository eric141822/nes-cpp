#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "cpu.h"
#include "global.h"

namespace NES
{

    struct OpCode
    {
        u8 opcode;
        std::string code_name;
        u8 len;
        u8 cycles;
        AddressingMode mode;
        OpCode(){};
        OpCode(u8 opcode, std::string code_name, u8 len, u8 cycles, AddressingMode mode) : opcode(opcode), code_name(code_name), len(len), cycles(cycles), mode(mode){};
    };

    void init_op_codes_map();
    extern std::map<u8, OpCode> OP_CODES_MAP;
}

#endif // !OPCODE_H