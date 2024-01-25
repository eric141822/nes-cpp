#include "test.h"

int main() {
    CPU cpu;
    cpu.mem_write(0x10, 0x55);

    cpu.load_and_run({0xA5, 0x10, 0x00});

    assert(cpu.register_a == 0x55 && "Register A should be 0x55");
    return 0;
}