#ifndef GLOBALS_H
#define GLOBALS_H

/* Maximum length of a single line  */
#define MAX_LINE_LENGTH 80

/* A struct that represet the informasion of a line*/
typedef struct line_info {
	/* Line number in file */
	long line_number;
	/* File name */
	char *file_name;
	/* Line content */
	char *content;
} line_info;

/* Boolean definition*/
typedef enum booleans {
	FALSE = 0, TRUE = 1
} bool;

/* Commands opcode */
typedef enum opcodes {
	MOV_OP = 0,
	CMP_OP = 1,
	ADD_OP = 2,
	SUB_OP = 3,
	NOT_OP = 4,
	CLR_OP = 5,
	LEA_OP = 6,
	INC_OP = 7,
	DEC_OP = 8,
	JMP_OP = 9,
	BNE_OP = 10,
	RED_OP = 11,
	PRN_OP = 12,
	JSR_OP = 13,
	RTS_OP = 14,
	HLT_OP = 15,
	NONE_OP = -1 /* In case of error*/
} opcode;

/*Registers*/
typedef enum registers {
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	NONE_REG = -1 /* In case of error*/
} reg;

/* Instructions types*/
typedef enum instruction {
	/** .data instruction */
	DATA_INST,
	/** .extern instruction */
	EXTERN_INST,
	/** .entry instruction */
	ENTRY_INST,
	/** .string instruction */
	STRING_INST,
	/** Not found */
	NONE_INST,
	/** Parsing/syntax error */
	ERROR_INST
} instruction;

typedef struct code_word {
	unsigned int ARE: 2;
	unsigned int dest_addressing: 2;
	unsigned int src_addressing: 2;
	unsigned int opcode: 4;

} code_word;

typedef struct data_word {
	unsigned int ARE: 2;
	unsigned long data;
} data_word;

typedef struct machine_word {
	short length;
	union word {
		data_word *data;
		code_word *code;
	} word;
} machine_word;


typedef enum addressing_types {
	IMMEDIATE_ADDR = 0,
	DIRECT_ADDR = 1,
	INDEX_FIXED_ADDR = 2,
	REGISTER_ADDR = 3,
	NONE_ADDR = -1
}addressing_type;

#endif 