#ifndef GLOBALS_H
#define GLOBALS_H

/* Maximum length of a line in the source code */
#define MAX_LINE_LENGTH 80

/* Initial value for the instruction counter */
#define IC_INIT_VALUE 100

/* Length of the code and data arrays in the image file */
#define CODE_ARR_IMG_LENGTH 1200

/* Structure to hold information about a line in the source code */
typedef struct line_info {
    long line_number;	/* Line number */
    char *file_name;	/* File name */
    char *content;		/* Content of the line */
} line_info;


/* Boolean values */
typedef enum booleans {
    FALSE = 0,	/* False boolean value */
	TRUE = 1	/* True boolean value */
} bool;

/* Operation codes */
typedef enum opcodes {
    MOV_OP = 0,     /* Move operation */
    CMP_OP = 1,     /* Compare operation */
    ADD_OP = 2,     /* Add operation */
    SUB_OP = 3,     /* Subtract operation */
    NOT_OP = 4,     /* Logical NOT operation */
    CLR_OP = 5,     /* Clear operation */
    LEA_OP = 6,     /* Load Effective Address operation */
    INC_OP = 7,     /* Increment operation */
    DEC_OP = 8,     /* Decrement operation */
    JMP_OP = 9,     /* Jump operation */
    BNE_OP = 10,    /* Branch if Not Equal operation */
    RED_OP = 11,    /* Read operation */
    PRN_OP = 12,    /* Print operation */
    JSR_OP = 13,    /* Jump to Subroutine operation */
    RTS_OP = 14,    /* Return from Subroutine operation */
    HLT_OP = 15,    /* Halt operation */
    NONE_OP = -1    /* No operation */
} opcode;

/* Registers */
typedef enum registers {
    R0 = 0,		/* Register 0 */
    R1,			/* Register 0 */
    R2,			/* Register 0 */
    R3,			/* Register 0 */
    R4,			/* Register 0 */
    R5,			/* Register 0 */
    R6,			/* Register 0 */
    R7,			/* Register 0 */
    NONE_REG = -1 	/* No register */
} reg;

/* Types of instructions */
typedef enum instruction {
    DATA_INST,		/* Data instruction */
    EXTERN_INST,	/* Extern  instruction */
    ENTRY_INST,		/* Entry  instruction */
    STRING_INST,	/* String  instruction */
    NONE_INST,		/* No instruction */
    ERROR_INST		/* Error  instruction */
} instruction;		

/* Structure to represent a machine code word for instructions */
typedef struct code_word {
    unsigned int ARE: 2;				/* Addressing Reallocation Encoding */
    unsigned int dest_addressing: 2;	/* Destination addressing mode */
    unsigned int src_addressing: 2;		/* Source addressing mode */
    unsigned int opcode: 4;				/* Operation code */
} code_word;

/* Structure to represent a machine code word for data */
typedef struct data_word {
    unsigned int ARE: 2;	/* Addressing Reallocation Encoding */
    unsigned long data;		/* Data value */
} data_word;

/* Structure to represent a machine code word */
typedef struct machine_word {
    short length;			/* Length of the word */
    union word {
        data_word *data;	/* Data word */
        code_word *code;	/* Code word */
    } word;
} machine_word;

/* Types of addressing modes */
typedef enum addressing_types {
    IMMEDIATE_ADDR = 0,		/* Immediate addressing */
    DIRECT_ADDR = 1,		/* Direct  addressing */
    INDEX_FIXED_ADDR = 2,	/* Indexed fixed addressing */
    REGISTER_ADDR = 3,		/* Register  addressing */
    NONE_ADDR = -1			/* No addressing mode */
} addressing_type;

#endif