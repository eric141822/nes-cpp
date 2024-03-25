#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>
#include "opcode.h"
#include "global.h"
#include "bus.h"
#include <functional>

namespace NES
{

    // STACK in 6502 CPU is 256 bytes long, and it starts at 0x0100, ends at 0x01FF
    // Pointer initially points to 0x01FF.
    const u16 STACK_START = 0x0100;

    // documented stack pointer start state.
    const u8 STACK_RESET = 0xFD;

    const u8 STATUS_RESET = 0b100100;
    namespace cpu_flags
    {
        static constexpr u8 CARRY = 0b00000001;
        static constexpr u8 ZERO = 0b00000010;
        static constexpr u8 INTERRUPT = 0b00000100;
        static constexpr u8 DECIMAL_UNUSED = 0b00001000;
        static constexpr u8 BREAK = 0b00010000;
        static constexpr u8 UNUSED = 0b00100000; // should always be 1.
        static constexpr u8 OVERFLW = 0b01000000;
        static constexpr u8 NEGATIVE = 0b10000000;
    };
    struct CPU : public Mem
    {
        u8 register_a;
        u8 register_x;
        u8 register_y;
        u8 status;
        u16 pc;
        u8 stack_pointer;
        Bus bus;

        CPU() : register_a(0), register_x(0), register_y(0), status(STATUS_RESET), pc(0), stack_pointer(STACK_RESET){};
        explicit CPU(Bus bus) : register_a(0), register_x(0), register_y(0), status(STATUS_RESET), pc(0), stack_pointer(STACK_RESET), bus(bus){};

        u8 mem_read(u16 address) override;
        void mem_write(u16 address, u8 value) override;
        u16 mem_read_u16(u16 address) override;
        void mem_write_u16(u16 address, u16 value) override;

        void reset();
        void load_and_run(std::vector<u8> program);
        void load(std::vector<u8> program);
        void run();
        void run_with_callback(std::function<void(CPU &)> callback);

        /* ------ HELPERS ------ */
        void set_zero_and_negative_flags(u8 register_value);
        bool check_status_flag();
        void add_to_register_a(u8 val);
        void set_register_a(u8 val);
        void set_register_x(u8 val);
        void set_register_y(u8 val);
        void stack_push(u8 val);
        u8 stack_pop();
        void stack_push_u16(u16 val);
        u16 stack_pop_u16();

        /* --------------------- */
        void lda(AddressingMode mode);
        void tax();
        void inx();
        void sta(AddressingMode mode);

        void adc(AddressingMode mode);
        void and_op(AddressingMode mode);
        void asl_acc();
        u8 asl(AddressingMode mode);

        // All jump related instructions.
        void branch(bool cond); // branch if cond is true
        void bit(AddressingMode mode);

        // CMP, CPX, CPY
        void cmp_op(AddressingMode mode, u8 reg);

        u8 dec(AddressingMode mode);
        void dex(); // might not be needed, just implied op
        void dey(); // might not be needed, just implied op
        void eor(AddressingMode mode);
        u8 inc(AddressingMode mode);
        void iny();

        void jmp();
        void jmp_abs();
        void jsr();
        void ldx(AddressingMode mode);
        void ldy(AddressingMode mode);
        void lsr_acc();
        u8 lsr(AddressingMode mode);
        void ora(AddressingMode mode);
        void pha();
        void php();
        void pla();
        void plp();
        void rol_acc();
        u8 rol(AddressingMode mode);
        void ror_acc();
        u8 ror(AddressingMode mode);
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

        u16 get_operand_address(AddressingMode mode);
        u16 get_abs_address(AddressingMode mode, u16 addr);
    };
}

#endif // !CPU_H