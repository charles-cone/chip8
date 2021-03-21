# chip8
Chip8 disassembler and interpreter written in C99

## Disassembler
The disassembler uses a recursive traveral algorithim to find all opcodes in the program, and labels all found branch loactions.

### Compilation
The disassembler has no dependencies and should compile as is on Linux / MacOS (Tested).
```bash
mkdir build
cd build
cmake ../disassembler && make
```
### Usage
#### Launching
```bash
./chip8-dis /path/to/rom
```

#### Options
```
-o  print opcodes in hex
-a  print the address of each opcode
```


## Interpreter
The interpreter uses opcode hashing with a table of function pointers for performance and is entirely functional with the exception of sound.

### Compilation
Install SDL2 through the package manager of your choice and then
```bash
mkdir build
cd build
cmake ../interpreter && make
```
Tested on MacOS but should work fine on Linux as well.

### Usage
#### Launching
```bash
./chip8-emu /path/to/rom
```

#### Keys
Keys 0-f are mapped to each row of the first four keys on the left of a QWERTY keyboard. (0 -> 1, 4 -> q, 8-> a, etc.)

#### Options
```
-c DESIRED_CYCLE_COUNT the number of opcodes to execute per second (default is 500)
```
