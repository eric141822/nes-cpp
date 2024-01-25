#include "test.h"

int main() {
    CPU cpu;
    cpu.load_and_run({0xA9, 0x05, 0x00});
    assert(cpu.register_a == 0x05 && "Register A should be 0x05");
    assert((cpu.status & 0b0000'0010) == 0 && "Zero flag should be clear");
    assert((cpu.status & 0b1000'0000) == 0 && "Negative flag should be clear");
    return 0;
}