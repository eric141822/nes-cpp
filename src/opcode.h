#ifndef OPCODE_H
#define OPCODE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "cpu.h"
#include "global.h"
struct OpCode
{
    uint8_t opcode;
    std::string code_name;
    uint8_t len;
    uint8_t cycles;
    AddressingMode mode;
    OpCode() {};
    OpCode(uint8_t opcode, std::string code_name, uint8_t len, uint8_t cycles, AddressingMode mode) : opcode(opcode), code_name(code_name), len(len), cycles(cycles), mode(mode){};
};

void init_op_codes_map();
extern std::map<uint8_t, OpCode> OP_CODES_MAP;

#endif // !OPCODE_H