//
// Created by Charlie Cone on 1/29/21.
//

#include "rom_io.h"

int openRom(FILE** rom, char * file_name) {
    FILE* p_rom = fopen(file_name, "r");
    if(p_rom == NULL) {
        return -1;
    }

    fseek(p_rom, 0, SEEK_END);
    int len = ftell(p_rom);
    fseek(p_rom, 0, SEEK_SET);

    *rom = p_rom;
    return len;
}

unsigned short read_opcode(FILE* rom, int offset) {
    fseek(rom, offset, SEEK_SET);
    return fgetc(rom) << 8 | fgetc(rom);
}



