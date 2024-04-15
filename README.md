# C Project: 14-bit Assembler
##Brief Description:

The Assembly Language Assembler project aims to develop a program capable of translating assembly language code into machine language, facilitating the conversion process and enabling the execution of assembly programs on target hardware. Assemblers play a crucial role in software development, as they bridge the gap between human-readable assembly instructions and the binary language understood by computers.

Key Features:

Assembly Code Parsing: The assembler will parse assembly language source code, recognizing instructions, labels, operands, and comments.

Symbol Table Management: It will manage a symbol table to keep track of labels and their corresponding memory addresses.

Instruction Translation: Translate assembly instructions into their equivalent machine language representations.

Error Handling: Provide informative error messages for syntax errors, undefined symbols, and other issues encountered during assembly.

Output Generation: Generate output files containing the translated machine code, ready for execution on the target hardware.

Memory Size and Encoding: The size of the memory is defined as 4096 cells, at addresses 0-4095 (in decimal base), with each cell being 14 bits in size. In the output file containing the machine code that the assembler builds, each word will be encoded in "encrypted" base 4, where 0 is represented by "*", 1 by "#", 2 by "%", and 3 by "!".

## Source Structure (Modules)
'main_program': This module manages the complete assembly process for one or more input assembly files. It oversees preprocessing, parsing, analysis, and output file generation, orchestrating the entire assembly workflow.

'parsing_phase': Responsible for the initial processing of assembly code, this module parses instructions, builds code words, and updates data structures to prepare for the subsequent phases of assembly.

'analysis_phase': This module processes assembly code during the second pass. It handles directives, labels, and operands to refine the code for further processing or execution, ensuring accuracy and completeness.

'global_variables': The header file global_variables.h contains macro definitions, enumerations, and structure declarations for global variables utilized throughout the assembler program, ensuring consistency and accessibility.

'code_functions': Part of the assembly process, this module parses assembly instructions, validates operands, and constructs code and data words based on parsed information. It plays a crucial role in assembling or compiling assembly language programs.

'data_tables': Managing a symbol table, this module stores information about symbols such as variable names or labels, along with their associated values and types. It provides essential data management functionality for the assembler.

'instructions_set': Handling instructions related to strings and data within assembly code lines, this module interprets or compiles assembly language instructions, contributing to the accurate translation of assembly code into machine code.

'utility_functions': This module comprises various utility functions essential for assembler or compiler functionality. These functions handle strings, errors, symbol tables, and specific instructions like .define, enhancing the efficiency and robustness of the assembly process.

'preprocessor': Responsible for macro processing within assembly files, this module extracts, processes, and substitutes macros, ensuring modular and reusable code organization while simplifying assembly file management.

'file_writing': As part of the assembly process, this module translates assembly code into machine code, generating output files in formats such as .ob, .ext, .ent, and .am. It completes the assembly workflow by producing executable machine code files ready for deployment or further processing.





## Usage
`./assembler file1 file2 file3 ...`
Will start the processing of each file (specify WITHOUT extension!). No support for double-quoting file names.

## Build
You may build with several tools:
1. Using CMake: run `cmake --build .` (you might have to run `cmake --cofngiure .` first) - Provides support for CLion IDE
2. Using `make` command (Makefile)
3. Manually (Not recommended): compile each `*.c` file to `*.o` and then compile all to a single executable using `CC`/`GCC`/`CLang` with `-g a.o b.o .... -o exe`
