//
// Created by Charlie Cone on 1/29/21.
//

#ifndef CHIP8_DISASSEMBLER_H
#define CHIP8_DISASSEMBLER_H

#define DIS_SHOW_OPCODE 0b00000001
#define DIS_SHOW_OFFSET 0b00000010

#define IS_DATA     (-2)
#define IS_CODE     (-1)
#define IS_DEFAULT_LABEL    0

#define ROM_START 0x200

#include "stdio.h"
#include "string.h"

#include "stack.h"

#include "../common/opcode.h"
#include "../common/rom_io.h"


int disassembleRom(char* rom_name, unsigned int options);

unsigned short read_opcode(FILE* rom, int offset);

#endif //CHIP8_DISASSEMBLER_H
