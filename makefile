# Basic compilation macros
CC = gcc # GCC Compiler
CFLAGS = -ansi -Wall -pedantic # Flags
GLOBAL_DEPS = globals.h # Dependencies for everything
EXE_DEPS = main.o code.o fpass.o spass.o instructions.o table.o utils.o # Deps for exe

## Executable
main: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(EXE_DEPS) $(CFLAGS) -o $@

## Main:
main.o: main.c $(GLOBAL_DEPS)
	$(CC) -c main.c $(CFLAGS) -o $@

## Code helper functions:
code.o: code.c code.h $(GLOBAL_DEPS)
	$(CC) -c code.c $(CFLAGS) -o $@

## First Pass:
fpass.o: first_pass.c first_pass.h $(GLOBAL_DEPS)
	$(CC) -c first_pass.c $(CFLAGS) -o $@

## Second Pass:
spass.o: second_pass.c second_pass.h $(GLOBAL_DEPS)
	$(CC) -c second_pass.c $(CFLAGS) -o $@
	
instructions.o: instructions.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c instructions.c $(CFLAGS) -o $@

## Table:
table.o: table.c table.h $(GLOBAL_DEPS)
	$(CC) -c table.c $(CFLAGS) -o $@

## Useful functions:
utils.o: utils.c instructions.h $(GLOBAL_DEPS)
	$(CC) -c utils.c $(CFLAGS) -o $@

# Clean Target (remove leftovers)
clean:
	rm -rf *.o