#include <unistd.h>

#include "disassembler.h"
#include "stack.h"

int main(int argc, char* argv[]) {
    char* usage = "usage: chip8-dis [options] file_name\n"
                  "  options:\n"
                  "\t-o print opcodes\n"
                  "\t-a print file addresses\n";

    int c;
    unsigned int options = 0;
    while((c = getopt(argc, argv, "oa")) != -1) {
        switch (c) {
            case 'o':
                options |= DIS_SHOW_OPCODE;
                break;
            case 'a':
                options |= DIS_SHOW_OFFSET;
                break;

            default:
                printf("%s", usage);
                return 1;
        }
    }
    if(optind == argc) {
        printf("Missing filename\n %s", usage);
    }

    // will throw error if non-valid filename
    disassembleRom( argv[optind], options );
}
