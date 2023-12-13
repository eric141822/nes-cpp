#ifndef TEST_H
#define TEST_H

#include <cstdint>
#include <cassert>
#include <iostream>
#include "../src/cpu.h"

void test_0xa9_lda_immediate_load_data();
void test_0xa9_lda_immediate_zero_flag();
void test_0xaa_tax_move_a_to_x();
void test_5_ops_together();
void test_inx_overflow();

#endif // !TEST_H


