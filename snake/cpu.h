#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>
#include "opcode.h"
#include "global.h"
#include "bus.h"
#include <functional>

// STACK in 6502 CPU is 256 bytes long, and it starts at 0x0100, ends at 0x01FF
// Pointer initially points to 0x01FF.
const uint16_t STACK_START = 0x0100;

// documented stack pointer start state.
const uint8_t STACK_RESET = 0xFD;

const uint8_t STATUS_RESET = 0b00100100;
namespace cpu_flags
{
    static constexpr uint8_t CARRY = 0b00000001;
    static constexpr uint8_t ZERO = 0b00000010;
    static constexpr uint8_t INTERRUPT = 0b00000100;
    static constexpr uint8_t DECIMAL_UNUSED = 0b00001000;
    static constexpr uint8_t BREAK = 0b00010000;
    static constexpr uint8_t UNUSED = 0b00100000; // should always be 1.
    static constexpr uint8_t OVERFLW = 0b01000000;
    static constexpr uint8_t NEGATIVE = 0b10000000;
};
struct CPU : public Mem
{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    uint8_t status;
    uint16_t pc;
    uint8_t stack_pointer;
    Bus bus;

    CPU() : register_a(0), register_x(0), register_y(0), status(STATUS_RESET), pc(0), stack_pointer(STACK_RESET){};
    explicit CPU(Bus bus) : register_a(0), register_x(0), register_y(0), status(STATUS_RESET), pc(0), stack_pointer(STACK_RESET), bus(bus){};
    
    uint8_t mem_read(uint16_t address) override;
    void mem_write(uint16_t address, uint8_t value) override;
    uint16_t mem_read_u16(uint16_t address) override;
    void mem_write_u16(uint16_t address, uint16_t value) override;

    void reset();
    void load_and_run(std::vector<uint8_t> program);
    void load(std::vector<uint8_t> program);
    void run();
    void run_with_callback(std::function<void(CPU &)> callback);

    /* ------ HELPERS ------ */
    void set_zero_and_negative_flags(uint8_t register_value);
    bool check_status_flag();
    void add_to_register_a(uint8_t val);
    void set_register_a(uint8_t val);
    void set_register_x(uint8_t val);
    void set_register_y(uint8_t val);
    void stack_push(uint8_t val);
    uint8_t stack_pop();
    void stack_push_u16(uint16_t val);
    uint16_t stack_pop_u16();

    /* --------------------- */
    void lda(AddressingMode mode);
    void tax();
    void inx();
    void sta(AddressingMode mode);

    void adc(AddressingMode mode);
    void and_op(AddressingMode mode);
    void asl_acc();
    uint8_t asl(AddressingMode mode);

    // All jump related instructions.
    void branch(bool cond); // branch if cond is true
    void bit(AddressingMode mode);

    // CMP, CPX, CPY
    void cmp_op(AddressingMode mode, uint8_t reg);

    uint8_t dec(AddressingMode mode);
    void dex(); // might not be needed, just implied op
    void dey(); // might not be needed, just implied op
    void eor(AddressingMode mode);
    uint8_t inc(AddressingMode mode);
    void iny();

    void jmp();
    void jmp_abs();
    void jsr();
    void ldx(AddressingMode mode);
    void ldy(AddressingMode mode);
    void lsr_acc();
    uint8_t lsr(AddressingMode mode);
    void ora(AddressingMode mode);
    void pha();
    void php();
    void pla();
    void plp();
    void rol_acc();
    uint8_t rol(AddressingMode mode);
    void ror_acc();
    uint8_t ror(AddressingMode mode);
    void rti();
    void rts();
    void sbc(AddressingMode mode);
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