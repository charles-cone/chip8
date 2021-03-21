cmake_minimum_required(VERSION 3.17)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

add_executable(chip8-dis main.c ../common/opcode.h
        disassembler.c disassembler.h
        ../common/rom_io.h ../common/rom_io.c
        ../common/opcode.c stack.c stack.h)
