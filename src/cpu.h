#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>
#include "opcode.h"
enum AddressingMode
{
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    IndirectX,
    IndirectY,
    NoneAddressing,
};

const extern uint16_t STACK_T = 0x0100;
const extern uint16_t STACK_RESET = 0xfd;
struct CPU_FLAGS
{
    static constexpr uint8_t CARRY = 0b00000001;
    static constexpr uint8_t ZERO = 0b00000010;
    static constexpr uint8_t INTERRUPT = 0b00000100;
    static constexpr uint8_t DECIMAL_UNUSED = 0b00001000;
    static constexpr uint8_t BREAK = 0b00010000;
    static constexpr uint8_t BREAK2 = 0b00100000;
    static constexpr uint8_t OVERFLOW = 0b01000000;
    static constexpr uint8_t NEGATIVE = 0b10000000;
};
struct CPU
{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    uint8_t status;
    uint16_t pc;
    uint8_t memory[0xFFFF];

    CPU() : register_a(0), register_x(0), register_y(0), status(0), pc(0){};
    uint8_t mem_read(uint16_t address);
    void mem_write(uint16_t address, uint8_t value);
    uint16_t mem_read_u16(uint16_t address);
    void mem_write_u16(uint16_t address, uint16_t value);
    void reset();
    void load_and_run(std::vector<uint8_t> program);
    void load(std::vector<uint8_t> program);
    void run();

    /* ------ HELPERS ------ */
    void set_zero_and_negative_flags(uint8_t register_value);
    bool check_status_flag();
    void add_to_register_a(uint8_t val);
    void set_register_a(uint8_t val);
    /* --------------------- */
    void lda(AddressingMode mode);
    void tax();
    void inx();
    void sta(AddressingMode mode);

    /* ------ TODO ------ */
    void adc(AddressingMode mode);
    void sbc(AddressingMode mode);
    void and_op(AddressingMode mode);
    uint8_t asl(AddressingMode mode);
    void branch(bool cond); // branch if cond is true
    void clear(uint8_t flag_to_clear); // clear flag
    void cmp_op(AddressingMode mode);
    void cpx(AddressingMode mode);
    void cpy(AddressingMode mode);
    void dec(AddressingMode mode);
    void dex(); // might not be needed, just implied op
    void dey(); // might not be needed, just implied op
    void eor(AddressingMode mode);
    void inc(AddressingMode mode);
    void iny();
    void jmp(AddressingMode mode);
    void jsr();
    void ldx(AddressingMode mode);
    void ldy(AddressingMode mode);
    void lsr(AddressingMode mode);
    void ora(AddressingMode mode);
    void pha();
    void php();
    void pla();
    void plp();
    void rol(AddressingMode mode);
    void ror(AddressingMode mode);
    void rti();
    void rts();
    void sbc(AddressingMode mode);
    void sec(); // implied
    void sed(); // implied
    void sei(); // implied
    void stx(AddressingMode mode);
    void sty(AddressingMode mode);
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();



    uint16_t get_operand_address(AddressingMode mode);
};

#endif // !CPU_H