#include "test.h"

int main() {
    CPU cpu;
    cpu.load_and_run({0xA9, 0x0A, 0xAA, 0x00});
    assert(cpu.register_x == 10 && "Register X should be 10");
    return 0;
}