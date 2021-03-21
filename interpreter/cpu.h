//
// Created by Charlie Cone on 1/24/21.
//

#ifndef CHIP8_CPU_H
#define CHIP8_CPU_H

#include "stdint.h"
#include "string.h"
#include "time.h"

#include "../common/opcode.h"
#include "../common/rom_io.h"

#include "debugging.h"

#define REG_MAX 16
#define MEM_MAX 4096
#define STACK_START 0
#define STACK_MAX 32
#define ROM_START 512
#define FONT_START 32
#define BITS_PER_CHAR 5

#define DISP_Y 32
#define DISP_X 64
#define BITS_PER_LINE 32

typedef struct chip8 chip8;
typedef void (*opFuncPtr) (struct chip8*, union opcode_data );

struct registers {
    unsigned short pc;
    unsigned short I;

    unsigned char dt;
    unsigned char st;
    unsigned char sp;

    unsigned char v[REG_MAX];
};

struct chip8 {
    struct registers reg;

    unsigned short jmp_addr;
    unsigned char mem[MEM_MAX];

    uint32_t display[DISP_Y * (DISP_X / BITS_PER_LINE)];
    uint16_t keypad;
    uint16_t old_keypad;
};


struct chip8* createChip8();
void loadFont(struct chip8 *c);

int loadRomIntoMem(struct chip8 *c, char* file_name);
union opcode_data parseNextOpcode(struct chip8* c);

void tick(struct chip8 *c);
void decreaseTimers(struct chip8 *c);
void printDisplay(uint32_t* d);

// OPCODE FUNCTIONS
void sys(struct chip8*c, union opcode_data d);
void cls(struct chip8*c, union opcode_data d);
void ret(struct chip8*c, union opcode_data d);
void jp(struct chip8*c, union opcode_data d);
void call(struct chip8*c, union opcode_data d);
void se_byte(struct chip8*c, union opcode_data d);
void sne_byte(struct chip8*c, union opcode_data d);
void se_reg(struct chip8*c, union opcode_data d);
void ld_byte(struct chip8*c, union opcode_data d);
void add_byte(struct chip8*c, union opcode_data d);
void ld_reg(struct chip8*c, union opcode_data d);
void or_reg(struct chip8*c, union opcode_data d);
void and_reg(struct chip8*c, union opcode_data d);
void xor_reg(struct chip8*c, union opcode_data d);
void add_reg(struct chip8*c, union opcode_data d);
void sub_reg(struct chip8*c, union opcode_data d);
void shr(struct chip8*c, union opcode_data d);
void subn_reg(struct chip8*c, union opcode_data d);
void shl(struct chip8*c, union opcode_data d);
void sne(struct chip8*c, union opcode_data d);
void ld_addr(struct chip8*c, union opcode_data d);
void jp_addr(struct chip8*c, union opcode_data d);
void rnd_byte(struct chip8*c, union opcode_data d);
void drw(struct chip8*c, union opcode_data d);
void skp_key(struct chip8*c, union opcode_data d);
void snkp_key(struct chip8*c, union opcode_data d);
void ld_dt(struct chip8*c, union opcode_data d);
void ld_key(struct chip8*c, union opcode_data d);
void set_dt(struct chip8*c, union opcode_data d);
void set_st(struct chip8*c, union opcode_data d);
void add_i_reg(struct chip8*c, union opcode_data d);
void ld_font_reg(struct chip8*c, union opcode_data d);
void ld_bin(struct chip8*c, union opcode_data d);
void str_v_reg(struct chip8*c, union opcode_data d);
void ld_v_reg(struct chip8*c, union opcode_data d);

#endif //CHIP8_CPU_H
