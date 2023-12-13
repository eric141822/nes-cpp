# CMake minimum version required
cmake_minimum_required(VERSION 3.12)

# Enable testing
enable_testing()

# Project name
project(NES-CPP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set compiler flags
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Werror -O3")

# main sources variable
set(SOURCES src/cpu.cpp src/opcode.cpp)

# Add executable target
add_executable(main.out ${SOURCES} src/main.cpp)

set(TEST_NAMES lda_immediate_load_data lda_immediate_zero_flag tax_move_a_to_x inx_overflow 5_ops_together lda_from_memory)

foreach(test_name IN LISTS TEST_NAMES)
    add_executable(${test_name} tests/${test_name}.cpp ${SOURCES})
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()