#include <iostream>
#include <cassert>
#include "cpu.h"
void test_0xa9_lda_immediate_load_data() {
    struct CPU cpu;
    cpu.load_and_run({ 0xA9, 0x05, 0x00 });
    assert(cpu.register_a == 0x05 && "Register A should be 0x05");
    assert((cpu.status & 0b0000'0010) == 0 && "Zero flag should be clear");
    assert((cpu.status & 0b1000'0000) == 0 && "Negative flag should be clear");
}

void test_0xa9_lda_immediate_zero_flag() {
    struct CPU cpu;
    cpu.load_and_run({ 0xA9, 0x00, 0x00 });
    assert(cpu.register_a == 0x00 && "Register A should be 0x00");
    assert((cpu.status & 0b0000'0010) == 0b0000'0010 && "Zero flag should be set");
    assert((cpu.status & 0b1000'0000) == 0 && "Negative flag should be clear");
}

void test_0xaa_tax_move_a_to_x() {
    struct CPU cpu;
    cpu.register_a = 10;
    cpu.load_and_run({ 0xAA, 0x00 });
    assert(cpu.register_x == 10 && "Register X should be 10");
}

void test_5_ops_together() {
    struct CPU cpu;
    cpu.load_and_run({ 0xA9, 0xC0, 0xAA, 0xE8, 0x00 });
    assert(cpu.register_x == 0xC1 && "Register X should be 0xC1");
}

void test_inx_overflow() {
    struct CPU cpu;
    cpu.register_x = 0xFF;
    cpu.load_and_run({ 0xE8, 0xE8, 0x00 });
    assert(cpu.register_x == 0x01 && "Register X should be 0x01");
}

int main() {
    test_0xa9_lda_immediate_load_data();
    test_0xa9_lda_immediate_zero_flag();
    test_0xaa_tax_move_a_to_x();
    test_5_ops_together();
    test_inx_overflow();
    std::cout << "All tests passed!\n";
    return 0;
}
