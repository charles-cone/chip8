cmake_minimum_required(VERSION 3.17)
project(chip8 C)

set(CMAKE_C_STANDARD 99)

FIND_PACKAGE(SDL2 REQUIRED)

add_executable(chip8-emu
        common/rom_io.c common/rom_io.h
        common/opcode.c common/opcode.h
        emulator/cpu.c emulator/cpu.h
        emulator/main.c emulator/main.c
        emulator/debugging.c emulator/debugging.h
        emulator/ui.c emulator/ui.h
        )

include_directories(chip8-emu ${SDL2_INCLUDE_DIRS})
target_link_libraries(chip8-emu ${SDL2_LIBRARIES})