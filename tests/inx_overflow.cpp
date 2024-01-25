#include "test.h"

int main() {
    CPU cpu;
    cpu.load_and_run({0xA9, 0xFF, 0xAA, 0xE8, 0xE8, 0x00});
    assert(cpu.register_x == 0x01 && "Register X should be 0x01");
    return 0;
}