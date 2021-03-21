cmake_minimum_required(VERSION 3.17)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

add_executable(chip8-dis disassembler/main.c common/opcode.h
        disassembler/disassembler.c disassembler/disassembler.h
        common/rom_io.h common/rom_io.c
        common/opcode.c disassembler/stack.c disassembler/stack.h)
