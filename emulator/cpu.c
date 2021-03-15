//
// Created by Charlie Cone on 1/24/21.
//

#include "cpu.h"

struct chip8* createChip8() {
    srand(time(0));
    struct chip8* c = (struct chip8*)calloc(1, sizeof(struct chip8));

    c->reg.pc = ROM_START;
    c->reg.sp = STACK_START;
    loadFont(c);

    return c;
}

void loadFont(struct chip8 *c) {
    static const unsigned char font[BITS_PER_CHAR*0x10] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,
        0x20, 0x60, 0x20, 0x20, 0x70,
        0xF0, 0x10, 0xF0, 0x80, 0xF0,
        0xF0, 0x10, 0xF0, 0x10, 0xF0,
        0x90, 0x90, 0xF0, 0x10, 0x10,
        0xF0, 0x80, 0xF0, 0x10, 0xF0,
        0xF0, 0x80, 0xF0, 0x90, 0xF0,
        0xF0, 0x10, 0x20, 0x40, 0x40,
        0xF0, 0x90, 0xF0, 0x90, 0xF0,
        0xF0, 0x90, 0xF0, 0x10, 0xF0,
        0xF0, 0x90, 0xF0, 0x90, 0x90,
        0xE0, 0x90, 0xE0, 0x90, 0xE0,
        0xF0, 0x80, 0x80, 0x80, 0xF0,
        0xE0, 0x90, 0x90, 0x90, 0xE0,
        0xF0, 0x80, 0xF0, 0x80, 0xF0,
        0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    memcpy(c->mem+FONT_START, font, BITS_PER_CHAR*0x10);
}

int loadRomIntoMem(struct chip8* c, char* file_name){
    FILE* rom = NULL;
    int f_len = openRom(&rom, file_name);
    if(f_len == -1) {
        printf("ERROR: unable to open: %s\n", file_name);
        return -1;
    }

    fread(c->mem+ROM_START, sizeof(unsigned char), f_len, rom);
    fclose(rom);
    return 0;
}

void tick(struct chip8 *c) {
    static const opFuncPtr i_table[NUMBER_OF_OPCODES] = {
            cls, ret, sys, jp, call, se_byte, sne_byte, se_reg, ld_byte, add_byte, ld_reg, or_reg,
            and_reg, xor_reg, add_reg, sub_reg, shr, subn_reg, shl, sne, ld_addr, jp_addr,
            rnd_byte, drw, skp_key, snkp_key, ld_dt, ld_key, set_dt, set_st, add_i_reg,
            ld_font_reg, ld_bin, str_v_reg, ld_v_reg
    };

    c->jmp_addr = 0;

    union opcode_data next_op = parseNextOpcode(c);
    int func_offset = getOpcodeOffset(next_op);
    (*i_table[func_offset])(c, next_op);

    // check if there was a jump
    if(c->jmp_addr != 0) {
        c->reg.pc = c->jmp_addr;
    } else {
        c->reg.pc += OPCODE_SIZE;
    }
}

void decreaseTimers(struct chip8 *c) {
    if(c->reg.dt) {
        c->reg.dt -= 1;
    }
    if(c->reg.st) {
        c->reg.st -=1;
    }
}

void printDisplay(uint32_t* d){
    for(int i = 0; i < DISP_Y; i++) {
        printf("line: %02d |", i);
        for(int b_i = 0; b_i < BITS_PER_LINE*2; b_i++ ) {
            if(d[i*2 + (b_i/BITS_PER_LINE)] & 1 << b_i%BITS_PER_LINE) {
                printf("X");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

union opcode_data parseNextOpcode(struct chip8* c) {
    union opcode_data d;
    unsigned short opcode = c->mem[c->reg.pc] << 8 | c->mem[c->reg.pc + 1];
    d.val = opcode;
    return d;
}

void sys(struct chip8*c, union opcode_data d) {
    return; // Ignored by all modern interpreters
}

// Clear display
void cls(struct chip8*c, union opcode_data d) {
    for(int i = 0; i < DISP_Y; i++) {
        c->display[i] = 0;
    }
}

// return; pop stack and set pc
void ret(struct chip8*c, union opcode_data d) {
    c->jmp_addr = c->mem[c->reg.sp-2] | (c->mem[c->reg.sp-1] << 8);
    c->reg.sp -= 2;
}

// jump to opcode addr_op
void jp(struct chip8*c, union opcode_data d) {
    c->jmp_addr = d.addr_op.addr;
}

// jump to opcode addr_op, but save current pc on stack
void call(struct chip8*c, union opcode_data d) {
   c->jmp_addr = d.addr_op.addr;

   c->reg.pc += OPCODE_SIZE;
    if (c->reg.sp < STACK_MAX) {
        c->mem[c->reg.sp++] = (c->reg.pc & 0x00FF);
        c->mem[c->reg.sp++] = (c->reg.pc & 0xFF00) >> 8;
    }
}

// skip ahead if v[x] matches byte_op
void se_byte(struct chip8*c, union opcode_data d) {
    if(c->reg.v[d.byte_op.x] == d.byte_op.byte) {
        c->jmp_addr = c->reg.pc + 4;
    }
}

// skip ahead if v[x] does not match byte_op
void sne_byte(struct chip8*c, union opcode_data d) {
    if(c->reg.v[d.byte_op.x] != d.byte_op.byte) {
        c->jmp_addr = c->reg.pc + 4;
    }
}

// skip ahead if v[x] == v[y]
void se_reg(struct chip8*c, union opcode_data d) {
    if(c->reg.v[d.reg_op.x] == c->reg.v[d.reg_op.y]) {
        c->jmp_addr = c->reg.pc + 4;
    }
}

// load_op byte_op into v[x]
void ld_byte(struct chip8*c, union opcode_data d){
    c->reg.v[d.byte_op.x] = d.byte_op.byte;
}

void add_byte(struct chip8*c, union opcode_data d) {
    c->reg.v[d.byte_op.x] += d.byte_op.byte;
};

void ld_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[d.reg_op.x] = c->reg.v[d.reg_op.y];
}

void or_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[d.reg_op.x] |= c->reg.v[d.reg_op.y];
}

void and_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[d.reg_op.x] &= c->reg.v[d.reg_op.y];
}

void xor_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[d.reg_op.x] ^= c->reg.v[d.reg_op.y];
}

void add_reg(struct chip8*c, union opcode_data d) {
    unsigned short result = c->reg.v[d.reg_op.x] + c->reg.v[d.reg_op.y];
    if(result > 0xFF) c->reg.v[0xF] = 1;
    c->reg.v[d.reg_op.x] = result & 0x00FF;
}

void sub_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[0xF] = c->reg.v[d.reg_op.x] > c->reg.v[d.reg_op.y];
    c->reg.v[d.reg_op.x] -= c->reg.v[d.reg_op.y];
}

void shr(struct chip8*c, union opcode_data d) {
    c->reg.v[0xF] = (c->reg.v[d.reg_op.x] & 0x1); // get LSB
    c->reg.v[d.reg_op.x] /= 2;
}

void subn_reg(struct chip8*c, union opcode_data d) {
    c->reg.v[0xF] = c->reg.v[d.reg_op.x] < c->reg.v[d.reg_op.y];
    c->reg.v[d.reg_op.x] -= c->reg.v[d.reg_op.y];
}

void shl(struct chip8*c, union opcode_data d) {
    c->reg.v[0xF] = (c->reg.v[d.reg_op.x] >> 7); // get MSB
    c->reg.v[d.reg_op.x] *= 2;
}

void sne(struct chip8*c, union opcode_data d) {
   if(c->reg.v[d.reg_op.x] != c->reg.v[d.reg_op.y]) {
       c->jmp_addr = c->reg.pc + 4;
   }
}

void ld_addr(struct chip8*c, union opcode_data d) {
    c->reg.I = d.addr_op.addr;
}

void jp_addr(struct chip8*c, union opcode_data d) {
    c->jmp_addr = d.addr_op.addr + c->reg.v[0];
}

void rnd_byte(struct chip8*c, union opcode_data d) {
    c->reg.v[d.byte_op.x] = rand() & d.byte_op.byte;
}

void drw(struct chip8*c, union opcode_data d) {
    unsigned char sprite_x = c->reg.v[d.reg_op.x];
    unsigned char sprite_y = c->reg.v[d.reg_op.y];
    // draw each sprite
    for(int i = 0; i < d.reg_op.end; i++ ) {
        // load sprite from memory
        unsigned char sprite = c->mem[c->reg.I + i];
        // offset by row of sprite, wrap if necessary, each display row has two values
        int row_index = ((sprite_y+i) % DISP_Y) * 2;
        // draw row of the sprite to the screen
        for(int b = 1; b <= BITS_PER_BYTE; b++) {
            // offset if writing to first or second display line
            int bit_offset = (sprite_x+b) % DISP_X;
            int line_index = row_index + (bit_offset / BITS_PER_LINE);
            // xor position if bit is high, account for sprites being flipped
            if(sprite & (1 << (BITS_PER_BYTE - b))) {
                int sprite_mask = 1 << bit_offset % BITS_PER_LINE;
                c->reg.v[0xF] = c->display[line_index] & sprite_mask;
                c->display[line_index] ^= sprite_mask;
            }
        }
    }

}

void skp_key(struct chip8*c, union opcode_data d) {
    if(c->keypad & (1 << c->reg.v[d.reg_op.x]))  {
        c->jmp_addr = c->reg.pc += 4;
    }
}

void snkp_key(struct chip8*c, union opcode_data d) {
    if(!(c->keypad & (1 << c->reg.v[d.reg_op.x]))) {
        c->jmp_addr = c->reg.pc += 4;
    }
}

void ld_dt(struct chip8*c, union opcode_data d) {
    c->reg.v[d.reg_op.x] = c->reg.dt;
}

void ld_key(struct chip8*c, union opcode_data d) {
    if((c->keypad | c->old_keypad) == c->old_keypad) {
        // block execution
        c->jmp_addr = c->reg.pc;
    } else {
        uint16_t key = (c->old_keypad ^ c->keypad);
        uint8_t key_i = 0;

        while ( key >>= 1) {
            key_i++;
        }

        c->reg.v[d.reg_op.x] = key_i;
        c->old_keypad = c->keypad;
    }
}

void set_dt(struct chip8*c, union opcode_data d) {
    c->reg.dt = c->reg.v[d.reg_op.x];
}

void set_st(struct chip8*c, union opcode_data d) {
    c->reg.st = c->reg.v[d.reg_op.x];
}

void add_i_reg(struct chip8*c, union opcode_data d) {
    c->reg.I += c->reg.v[d.reg_op.x];
}

void ld_font_reg(struct chip8*c, union opcode_data d) {
    c->reg.I = FONT_START + (c->reg.v[d.reg_op.x] * BITS_PER_CHAR);
}

void ld_bin(struct chip8*c, union opcode_data d) {
    c->mem[c->reg.I] = c->reg.v[d.reg_op.x] / 100; // hundreds
    c->mem[c->reg.I + 1] = (c->reg.v[d.reg_op.x] % 100) / 10; // tens
    c->mem[c->reg.I + 2] = c->reg.v[d.reg_op.x] % 10; // ones
}

void str_v_reg(struct chip8*c, union opcode_data d) {
    for(int i = 0; i <= d.reg_op.x; i++) {
         c->mem[c->reg.I + i] = c->reg.v[i];
    }
}

void ld_v_reg(struct chip8*c, union opcode_data d) {
    for(int i = 0; i <= d.reg_op.x; i++) {
        c->reg.v[i] = c->mem[c->reg.I + i];
    }
}
