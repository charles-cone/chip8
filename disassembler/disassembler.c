//
// Created by Charlie Cone on 1/29/21.
//
#include "disassembler.h"

struct opcode_disassembly {
    char* name;
    enum op_type type;
};

struct dis_val {
    char label_val;
    unsigned char is_unaligned;
    union opcode_data o_d;
};

struct opcode_disassembly d_table[NUMBER_OF_OPCODES] = {
    { "SYS", ADDRESS},      { "CLS", NONE},         { "RET", NONE},
    { "JP", JUMP },      { "CALL", JUMP },          { "SE", BYTE},
    { "SNE", BYTE },        {"SE", REGISTER},       { "LD", BYTE },         { "ADD", BYTE  },
    { "LD", REGISTER },     { "OR", REGISTER },     { "AND", REGISTER },
    { "XOR", REGISTER },    { "ADD", REGISTER },    { "SUB", REGISTER },
    { "SHR", REGISTER },    { "SUBN", REGISTER },   { "SHL", REGISTER },
    { "SNE", REGISTER },    { "LD I,", ADDRESS },   { "JP I,", JUMP },
    { "RND", BYTE },        { "DRW", DRAW},         { "SKP", REGISTER },
    { "SKNP", REGISTER },   { "LD DT", LOAD},       { "LD K", LOAD },
    { "STR DT", LOAD},      { "STR ST", LOAD},      { "ADD I", LOAD },
    { "LD F", LOAD },       { "LD BIN", LOAD },     { "STR REG", LOAD},
    { "LD REG", LOAD }
};

static inline struct opcode_disassembly lookupOpcode(union opcode_data o_d) {
    return d_table[getOpcodeOffset(o_d)];
}

void printOpcodeDisassembly(union opcode_data o_dat, char l_num) {
    struct opcode_disassembly o_dis = lookupOpcode(o_dat);
    switch(o_dis.type) {
        case REGISTER:
            printf("%s V%x, V%x\n", o_dis.name, o_dat.reg_op.x, o_dat.reg_op.y );
            break;
        case ADDRESS:
            printf("%s 0x%03x\n", o_dis.name, o_dat.addr_op.addr );
            break;
        case JUMP:
            if(l_num == 0) {
                printf("%s .start (0x%03x)\n", o_dis.name, o_dat.addr_op.addr);
            } else {
                printf("%s .L%d (0x%03x)\n", o_dis.name, l_num, o_dat.addr_op.addr );
            }
            break;
        case BYTE:
            printf("%s V%x, 0x%02x\n", o_dis.name, o_dat.byte_op.x, o_dat.byte_op.byte );
            break;
        case LOAD:
            printf("%s V%x\n", o_dis.name, o_dat.byte_op.x);
            break;
        case DRAW:
            printf("%s V%x, V%x, n: %d\n", o_dis.name, o_dat.reg_op.x,o_dat.reg_op.y, o_dat.reg_op.end );
            break;
        default:
            printf("%s\n", o_dis.name );
            break;
    }
}

static inline int isOptionSelected(unsigned int options, unsigned int dis_opt ){
    if(options & dis_opt) return 1;
    else return 0;
}

static inline char getOffsetLabelValue(int offset, struct dis_val* buf) {
    int buf_offset = (offset-ROM_START) / OPCODE_SIZE;
    return buf[buf_offset].label_val;
}

void printDisassembly(struct dis_val* buf, int buf_len, unsigned int options) {
    printf(".start:\n");
    for(int i = 0; i < buf_len; i++) {
        struct dis_val t = buf[i];
        if(buf[i].label_val != IS_DATA) {
            if(buf[i].label_val > IS_DEFAULT_LABEL ) {
                printf(".L%d:\n", + buf[i].label_val);
            }
            printf("\t");

            if (isOptionSelected(options, DIS_SHOW_OFFSET)) {
                unsigned short offset = (i*OPCODE_SIZE) + buf[i].is_unaligned;
                printf("0x%04x: ", offset);
            }
            if (isOptionSelected(options, DIS_SHOW_OPCODE)) {
                printf("0x%04x ", buf[i].o_d.val);
            }

            //TODO add lookup for label index
            int jump_addr = buf[i].o_d.addr_op.addr;
            printOpcodeDisassembly(buf[i].o_d, getOffsetLabelValue(jump_addr, buf));
        }
    }
}

static inline int isJumpOpcode(unsigned short opcode) {
    return (opcode >> 12) == 0xB || (opcode >> 12) == 0x1 || (opcode >> 12) == 0x2;
}

static inline int isCallOpcode(unsigned short opcode) {
    return (opcode >> 12) == 0x2;
}

static inline int isSkipOpcode(unsigned short opcode) {
    return (opcode >> 12) > 0x2 && (opcode >> 12) < 0x6 ;
}

static inline int isSysOpcode(unsigned short opcode) {
    return opcode == 0;
}

static inline int fileToBufferOffset( int f_offset ) {
    return f_offset / OPCODE_SIZE;
}

static inline int isCode(struct dis_val *buf, int buf_len, int f_offset ) {
    if((f_offset / OPCODE_SIZE) > buf_len) {
        return 0; // OOB
    }
    return buf[fileToBufferOffset(f_offset)].label_val > IS_DATA;
}

static inline int offsetIsInBounds(int f_offset, int buf_len) {
    return fileToBufferOffset(f_offset) < buf_len;
}

void traceExecution(FILE *rom, struct dis_val buf[], int buf_len) {
    // do rough pass and discover all conditional jump addresses
    struct stack* label_addresses = initStack(10);
    insertVal(label_addresses, 0);
    struct stack* jump_addresses = initStack(10);
    insertVal(jump_addresses, 0);
    int block_index = 0;

    while(block_index < jump_addresses->len) {
        // get offset of the next block to disassemble
        int file_offset = jump_addresses->buf[block_index];
        unsigned char is_conditional = 0;
        unsigned char did_branch = 0;

        while(!isCode(buf, buf_len, file_offset) && offsetIsInBounds(file_offset, buf_len) ) {
            union opcode_data o_data = (union opcode_data)read_opcode(rom, file_offset);

            int b_offset = fileToBufferOffset(file_offset);
            buf[b_offset].label_val = did_branch ? IS_DEFAULT_LABEL : IS_CODE;
            buf[b_offset].is_unaligned = file_offset % 2;
            buf[b_offset].o_d = o_data;

            if(isSysOpcode(o_data.val)) {
                // some roms i have come across appear to be using 0x0000 as a quit opcode
                break;
            }

            if(isJumpOpcode(o_data.val)) {
                int jump_offset = o_data.addr_op.addr-ROM_START;
                b_offset = fileToBufferOffset(jump_offset);

                // check if the jump location is new
                if(buf[b_offset].label_val != IS_DEFAULT_LABEL) {
                    insertVal(label_addresses, jump_offset);
                    // check if the area has already been disassembled
                    if(buf[b_offset].label_val == IS_CODE) buf[b_offset].label_val = IS_DEFAULT_LABEL;

                    // Handle new disassembly code
                    if (is_conditional || isCallOpcode(o_data.val)) {
                        // Keep disassembling, but add the jump area to the blocks to investigate
                        insertVal(jump_addresses, jump_offset);
                        file_offset += OPCODE_SIZE;
                        did_branch = 0;
                    } else {
                        // unconditional jump so go to the new area
                        file_offset = jump_offset;
                        did_branch = 1;
                    }
                }
            } else {
                file_offset += OPCODE_SIZE;
                did_branch = 0;
            }

            is_conditional = isSkipOpcode(o_data.val);
        }
        block_index++;
    }
    deleteStack(jump_addresses);

    // Update code labels so disassembly is labeled sequentially
    sortContents(label_addresses);
    for(int i = 0; i < label_addresses->len; i++) {
        buf[fileToBufferOffset(label_addresses->buf[i])].label_val = i;
    }
    deleteStack(label_addresses);
}

int disassembleRom(char* rom_name, unsigned int options) {
    // load in rom and allocate buffer
    FILE* rom = fopen(rom_name, "r");
    if(rom == NULL) {
        printf("ERROR: unable to open rom: %s\n", rom_name);
        return -1;
    }

    fseek(rom, 0, SEEK_END);
    int rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    int buf_len = rom_size / OPCODE_SIZE;
    struct dis_val* d_buffer = malloc(buf_len * sizeof(struct dis_val));
    for(int i = 0; i < buf_len; i++) {
        // by default all code is labeled as data
        d_buffer[i].label_val = IS_DATA;
    }

    // trace execution and buffer opcodes
    traceExecution(rom, d_buffer, buf_len);
    // go through the traced buffer and print all opcodes and labels
    printDisassembly(d_buffer, buf_len, options);
    // cleanup
    fclose(rom);
    free(d_buffer);
    return 0;
};
