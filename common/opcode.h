//
// Created by Charlie Cone on 1/24/21.
//

#ifndef CHIP8_OPCODE_H
#define CHIP8_OPCODE_H

#define NUMBER_OF_OPCODES 35
#define OPCODE_SIZE 2

#include "stdlib.h"

enum op_type {
    REGISTER,
    ADDRESS,
    BYTE,
    LOAD,
    DRAW,
    JUMP,
    NONE
};

struct reg_opcode {
    unsigned end:4;
    unsigned y:4;
    unsigned x:4;
    unsigned index:4;
};

struct addr_opcode {
    unsigned addr:12;
    unsigned index:4;
};

struct byte_opcode {
    unsigned byte:8;
    unsigned x:4;
    unsigned index:4;
};

struct load_opcode {
    unsigned type:8;
    unsigned x:4;
    unsigned index:4;
};

union opcode_data {
    struct reg_opcode reg_op;
    struct addr_opcode addr_op;
    struct byte_opcode byte_op;
    struct load_opcode load_op;
    unsigned short val;
};

int getOpcodeOffset(union opcode_data d);

#endif //CHIP8_OPCODE_H
