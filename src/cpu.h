#ifndef CPU_H
#define CPU_H
#include <cstdint>
#include <vector>

struct CPU
{
    uint8_t register_a;
    uint8_t register_x;
    uint8_t status;
    uint16_t pc;
    uint8_t memory[0xFFFF];

    CPU() : register_a(0), register_x(0), status(0), pc(0){};
    uint8_t mem_read(uint16_t address);
    void mem_write(uint16_t address, uint8_t value);
    uint16_t mem_read_u16(uint16_t address);
    void mem_write_u16(uint16_t address, uint16_t value);
    void reset();
    void load_and_run(std::vector<uint8_t> program);
    void load(std::vector<uint8_t> program);
    void run();
    void set_zero_and_negative_flags(uint8_t register_value);
    void lda(uint8_t param);
    void tax();
    void inx();
};

#endif // !CPU_H