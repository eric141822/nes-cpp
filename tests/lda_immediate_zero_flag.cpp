#include "test.h"

int main() {
    CPU cpu;
    cpu.load_and_run({0xA9, 0x00, 0x00});
    assert(cpu.register_a == 0x00 && "Register A should be 0x00");
    assert((cpu.status & 0b0000'0010) == 0b0000'0010 && "Zero flag should be set");
    assert((cpu.status & 0b1000'0000) == 0 && "Negative flag should be clear");
    return 0;
}