//
// Created by Charlie Cone on 2/10/21.
//

#include "debugging.h"

void printRegisters(struct chip8 *c) {
    printf("pc: 0x%04x I: 0x%04x dt: 0x%02x st: 0x%02x sp: 0x%02x\n",
           c->reg.pc, c->reg.I, c->reg.dt, c->reg.st, c->reg.sp);

    for(int i = 0; i < REG_MAX; i++){
        printf("V%x: 0x%02x ", i, c->reg.v[i]);
        if(i == (REG_MAX-1) / 2) printf("\n");
    }
    printf("\n");
}

void printHex(unsigned char* data, int len) {
    for(int i = 0; i < len; i++) {
        if(i % 16 == 0 && i != 0) printf("\n");
        printf("0x%02x ", data[i]);
    }
    printf("\n");
}

void printStack(struct chip8 *c) {
    for(int i = 0; i < c->reg.sp; i += OPCODE_SIZE) {
        printf("%d: 0x%04x\n", i, c->mem[STACK_START + i]);
    }
}

void printOpcodeData(union opcode_data d, enum op_type mode) {
    switch (mode) {
        case REGISTER:
            printf("0x%04x :  id: 0x%x x: 0x%x y: 0x%x end: 0x%x\n", d.val,
                   d.reg_op.index, d.reg_op.x, d.reg_op.y, d.reg_op.end );
            break;
        case ADDRESS:
            printf("0x%04x :  id: 0x%x addr_op: 0x%03x\n", d.val,
                   d.addr_op.index, d.addr_op.addr );
            break;
        case BYTE:
            printf("0x%04x :  id: 0x%x x: 0x%x byte_op: 0x%02x\n", d.val,
                   d.byte_op.index, d.byte_op.x, d.byte_op.byte );
            break;
        case LOAD:
            printf("0x%04x :  id: 0x%x x: 0x%x type: 0x%02x\n", d.val,
                   d.load_op.index, d.load_op.x, d.load_op.type );
            break;
        default:
            break;
    }
}

void printBinary(void *val, int len) {
    char* byte_arr = (char*)val;
//    printf("0b");
    for(int i = 0; i < len; i++) {
        char byte = byte_arr[i];
        for (int j = 0; j < BITS_PER_BYTE; ++j) {
            char val_at_pos = byte & (1 << j) ?  'X' : '_';
            printf("%c", val_at_pos);
        }
    }
    printf("\n");
}
