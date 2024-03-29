# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = global_variables.h # Dependencies for everything
EXE_DEPS = main_program.o code_functions.o parsing_phase.o analysis_phase.o instructions_set.o data_tables.o utility_functions.o file_writeing.o # Deps for exe

## Executable
main_program: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
main_program.o: main_program.c $(GLOBAL_DEPS)
	$(CC) -c main_program.c $(CFLAGS) -o $@

## Code helper functions:
code_functions.o: code_functions.c code_functions.h $(GLOBAL_DEPS)
	$(CC) -c code_functions.c $(CFLAGS) -o $@

## First Pass:
parsing_phase.o: parsing_phase.c parsing_phase.h $(GLOBAL_DEPS)
	$(CC) -c parsing_phase.c $(CFLAGS) -o $@

## Second Pass:
analysis_phase.o: analysis_phase.c analysis_phase.h $(GLOBAL_DEPS)
	$(CC) -c analysis_phase.c $(CFLAGS) -o $@

instructions_set.o: instructions_set.c instructions_set.h $(GLOBAL_DEPS)
	$(CC) -c instructions_set.c $(CFLAGS) -o $@

## Table:
data_tables.o: data_tables.c data_tables.h $(GLOBAL_DEPS)
	$(CC) -c data_tables.c $(CFLAGS) -o $@

## Useful functions:
utility_functions.o: utility_functions.c instructions_set.h $(GLOBAL_DEPS)
	$(CC) -c utility_functions.c $(CFLAGS) -o $@

## Output Files:
file_writeing.o: file_writeing.c file_writeing.h $(GLOBAL_DEPS)
	$(CC) -c file_writeing.c $(CFLAGS) -o $@
	
# Clean Target (remove leftovers)
clean:
	rm -rf *.o