//
// Created by Charlie Cone on 2/10/21.
//

#ifndef CHIP8_DEBUGGING_H
#define CHIP8_DEBUGGING_H

#define BITS_PER_BYTE 8

#include "stdio.h"

#include "cpu.h"

void printStack(struct chip8 *c);
void printRegisters(struct chip8 *c);
void printHex(unsigned char* data, int len);
void printOpcodeData( union opcode_data d, enum op_type mode);
void printBinary(void *val, int len);

#endif //CHIP8_DEBUGGING_H
