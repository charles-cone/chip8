cmake_minimum_required(VERSION 3.17)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

add_executable(chip8-dis disassembler/main.c common/opcode.h
        disassembler/disassembler.c disassembler/disassembler.h
        common/rom_io.h common/rom_io.c
        common/opcode.c disassembler/stack.c disassembler/stack.h)

FIND_PACKAGE(SDL2 REQUIRED)

add_executable(chip8-emu
        common/rom_io.c common/rom_io.h
        common/opcode.c common/opcode.h
        interpreter/cpu.c interpreter/cpu.h
        interpreter/main.c interpreter/main.c
        interpreter/debugging.c interpreter/debugging.h
        interpreter/ui.c interpreter/ui.h
        )

include_directories(chip8-emu ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8-emu ${SDL2_LIBRARIES})