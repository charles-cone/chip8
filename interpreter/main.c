//
// Created by Charlie Cone on 2/9/21.
//

#include <stdio.h>
#include <unistd.h>

#include "cpu.h"
#include "ui.h"

#define DEFAULT_CYCLES_PER_SEC 500
#define FPS 60

int main( int argc, char* argv[] ) {
    char* usage =   "usage: chip8-emu [options] file_name\n"
                    "  options:\n"
                    "\t-c (#) Number of cycles per second, default is 500\n";

    if(argc == 1) {
        printf("%s", usage);
        return -1;
    }

    int c;
    int cycles_per_sec = DEFAULT_CYCLES_PER_SEC;
    while((c = getopt(argc, argv, "c:")) != -1) {
        switch (c) {
            case 'c':
                cycles_per_sec = atoi(optarg);
                break;
            default:
                printf("%s", usage);
        }
    }


    struct chip8* emu = createChip8();
    if(loadRomIntoMem(emu, argv[optind]) == -1) {
        return -1;
    };

    struct emu_ui* ui = initUI(emu->display, &emu->keypad);

    if(ui != NULL) {
        while(ui->is_open) {
            drawDisplayUI(ui);
            decreaseTimers(emu);

            for(int i = 0; i < cycles_per_sec / FPS; i++) {
                tick(emu);
                tickUI(ui);
            }
        }
    }

    closeUI(ui);
    free(emu);

    return 0;
}