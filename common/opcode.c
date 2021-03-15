//
// Created by Charlie Cone on 1/29/21.
//

#include "opcode.h"

typedef int (*opHashFuncPtr) (union opcode_data);

struct table_node {
    int offset;
    opHashFuncPtr hash_func;
};

int get_zero_offset(union opcode_data d) {
    return (d.reg_op.y == 0xE) + (d.reg_op.index == 0xE);
}

int get_eight_offset(union opcode_data d) {
    int location = d.reg_op.end;
    if(d.reg_op.end == 0xE) {
        location -= 6; // SHL is abnormality
    }
    return location;
}

int get_e_offset(union opcode_data d) {
    return d.reg_op.end == 1;
}

int get_f_offset(union opcode_data d) {
    unsigned char i = 0x9 / 2;
    unsigned char top = 0x9;
    unsigned char bot = 0;
    unsigned char val = d.load_op.type;

    static const unsigned char ld_lookup[0x9] = {0x7, 0xa, 0x15, 0x18, 0x1e, 0x29, 0x33, 0x55, 0x65 };

    while(ld_lookup[i] != val && top > bot) {
        if(ld_lookup[i] > val) {
            top = i-1;
        } else {
            bot = i+1;
        }
        i = (top + bot) / 2;
    }
    return i;
}



int getOpcodeOffset(union opcode_data d) {
    static const struct table_node d_lookup[0x10] = {
            {0, get_zero_offset}, // 0x0
            {3, NULL}, // 0x1
            {4, NULL}, // 0x2
            {5, NULL}, // 0x3
            {6, NULL}, // 0x4
            {7, NULL}, // 0x5
            {8, NULL}, // 0x6
            {9, NULL}, // 0x7
            {10, get_eight_offset}, // 0x8
            {19, NULL}, // 0x9
            {20, NULL}, // 0xA
            {21, NULL}, // 0xB
            {22, NULL}, // 0xC
            {23, NULL}, // 0xD
            {24, get_e_offset}, // 0xE
            {26, get_f_offset} // 0xF
    };

    unsigned int op_val = d.val / 0x1000;
    struct table_node t_data = d_lookup[op_val];
    if(t_data.hash_func == NULL) {
        return t_data.offset;
    } else {
        return t_data.offset + (*t_data.hash_func)(d);
    }
}
