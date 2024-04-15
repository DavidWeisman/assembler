# C Project: 14-bit Assembler
Brief Description:

The Assembly Language Assembler project aims to develop a program capable of translating assembly language code into machine language, facilitating the conversion process and enabling the execution of assembly programs on target hardware. Assemblers play a crucial role in software development, as they bridge the gap between human-readable assembly instructions and the binary language understood by computers.

Key Features:

Assembly Code Parsing: The assembler will parse assembly language source code, recognizing instructions, labels, operands, and comments.

Symbol Table Management: It will manage a symbol table to keep track of labels and their corresponding memory addresses.

Instruction Translation: Translate assembly instructions into their equivalent machine language representations.

Error Handling: Provide informative error messages for syntax errors, undefined symbols, and other issues encountered during assembly.

Output Generation: Generate output files containing the translated machine code, ready for execution on the target hardware.

Memory Size and Encoding: The size of the memory is defined as 4096 cells, at addresses 0-4095 (in decimal base), with each cell being 14 bits in size. In the output file containing the machine code that the assembler builds, each word will be encoded in "encrypted" base 4, where 0 is represented by "*", 1 by "#", 2 by "%", and 3 by "!".

## Source structure (Modules)
1. `assembler` - main function definition, argv & argc processing, single file processing.
2. `first_pass` - contains functions for processing a single line in the first pass and a code line in the first pass.
3. `second_pass` - contains function for processing a single line in second pass and replacing symbol by it's address.
4. `code` - contains useful function for processing code.
5. `instructions` - contains useful function for processing instruction.
6. `globals.h` - contains type and constant definitions.
7. `table` - contains definition of table-like linked list data structure.
8. `utils` - contains general-purposed, project-wide functions.
9. `writefiles` - contains methods for writing output files: `*.ob`, `*.ext` and `*.ent` 

## Usage
`./assembler file1 file2 file3 ...`
Will start the processing of each file (specify WITHOUT extension!). No support for double-quoting file names.

## Build
You may build with several tools:
1. Using CMake: run `cmake --build .` (you might have to run `cmake --cofngiure .` first) - Provides support for CLion IDE
2. Using `make` command (Makefile)
3. Manually (Not recommended): compile each `*.c` file to `*.o` and then compile all to a single executable using `CC`/`GCC`/`CLang` with `-g a.o b.o .... -o exe`
