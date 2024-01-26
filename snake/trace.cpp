#include "trace.h"
std::string trace(CPU &cpu) {
    // test
    int a = 42;
    return fmt::format("Hello {}", a);
}