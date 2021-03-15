//
// Created by Charlie Cone on 1/29/21.
//

#ifndef CHIP8_ROM_IO_H
#define CHIP8_ROM_IO_H

#include "stdio.h"

int openRom(FILE** rom, char * file_name);
unsigned short read_opcode(FILE* rom, int offset);

#endif //CHIP8_ROM_IO_H
