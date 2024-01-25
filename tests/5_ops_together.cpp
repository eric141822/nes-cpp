#include "test.h"

int main() {
    CPU cpu;
    cpu.load_and_run({0xA9, 0xC0, 0xAA, 0xE8, 0x00});
    assert(cpu.register_x == 0xC1 && "Register X should be 0xC1");
    return 0;
}