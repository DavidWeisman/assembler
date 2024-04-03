#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_variables.h"
#include "code_functions.h"
#include "utility_functions.h"
#include "instructions_set.h"
#include "parsing_phase.h"


/**
 *  Processes a single code instruction during the first pass of assembly.
 *
 * This function processes a single code instruction during the first pass of the assembly process.
 * It extracts the operation and operands, determines the opcode, analyzes operands, builds the code word,
 * and updates the instruction counter and code image accordingly.
 *
 * @param line The line_info structure containing information about the current line.
 * @param index_l The index within the line content to start processing the code instruction.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param code_img Array to store machine words representing code instructions.
 * @param symbol_table The symbol table containing defined symbols.
 * @return Returns TRUE if the code instruction is successfully processed, otherwise returns FALSE.
 */
static bool process_code(line_info line, int index_l, long *ic, machine_word **code_img, table symbol_table);

/* Processes a single line of code during the first pass of assembly */
bool process_line_fpass(line_info line, long *IC, long *DC, machine_word **code_img, long *data_img, table *symbol_table) {
    int index_line = 0;     /* Index within the line content */
    int index_symbol = 0;   /* Index within the symbol */
    char symbol[MAX_LINE_LENGTH];   /* Temporary buffer to store symbol */
    instruction currentInstruction;  /* Current instruction */

    /* Skip leading spaces */
    index_line = skip_spaces(line.content, index_line);

    /* Check if the line is empty or a comment */
    if (!line.content[index_line] || line.content[index_line] == '\n' || line.content[index_line] == EOF || line.content[index_line] == ';') {
        return TRUE;
    }

    /* Check for macro definition */
    if (!check_mdefine(line, symbol_table, symbol)) {
        return FALSE;
    } else if (find_by_types(*symbol_table, symbol) && find_by_types(*symbol_table, symbol)->type == MDEFINE_SYMBOL) {
        return TRUE;
    }

    /* Extract label */
    if (extract_label(line, symbol)) {
        return FALSE;
    }

    /* Check label name validity */
    if (symbol[0] && !check_label_name(symbol)) {
        print_error(line, "Illegal label name: %s", symbol);
        return FALSE;
    }

    /* Process label if present */
    if (symbol[0] != '\0') {
        while (line.content[index_line] != ':') {
            index_line++;
        }
        index_line++;
    }

    /* Skip leading spaces */
    index_line = skip_spaces(line.content, index_line);

    /* Check for end of line */
    if (line.content[index_line] == '\n') {
        return TRUE;
    }

    /* Check if symbol is already defined */
    if (find_by_types(*symbol_table, symbol)) {
        print_error(line, "Symbol %s is already defined.", symbol);
        return FALSE;
    }

    /* Find the instruction */
    currentInstruction = find_instruction_from_index(line, &index_line);

    /* Error handling */
    if (currentInstruction == ERROR_INST) {
        return FALSE;
    }

    /* Skip leading spaces */
    index_line = skip_spaces(line.content, index_line);

    /* Process instruction */
    if (currentInstruction != NONE_INST) {
        /* Process .data and .string instructions */
        if ((currentInstruction == DATA_INST || currentInstruction == STRING_INST) && symbol[0] != '\0') {
            if (currentInstruction == STRING_INST) {
                add_table_item(symbol_table, symbol, *DC - 1, DATA_SYMBOL);
            }
            add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);
        }

        /* Process .string instruction */
        if (currentInstruction == STRING_INST) {
            return process_string_instruction(line, index_line, data_img, DC);
        }
        /* Process .data instruction */
        else if (currentInstruction == DATA_INST) {
            return process_data_instruction(line, index_line, data_img, DC, *symbol_table);
        }
        /* Process .extern instruction */
        else if (currentInstruction == EXTERN_INST) {
            /* Extract external symbol */
            index_line = skip_spaces(line.content, index_line);
            while (line.content[index_line] && line.content[index_line] != '\n' && line.content[index_line] != '\t' && line.content[index_line] != ' ' && line.content[index_line] != EOF) {
                symbol[index_symbol] = line.content[index_line];
                index_line++;
                index_symbol++;
            }
            symbol[index_symbol] = 0;
            /* Check label name validity */
            if (!check_label_name(symbol)) {
                print_error(line, "Invalid external label name: %s", symbol);
                return TRUE;
            }
            /* Add the external symbol to the symbol table */
            add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL);
        }
        /* Process .entry instruction */
        else if (currentInstruction == ENTRY_INST && symbol[0] != '\0') {
            print_error(line, "Can't define a label to an entry instruction.");
            return FALSE;
        }
    } else {
        /* Process regular code instruction */
        if (symbol[0] != '\0') {
            add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
        }
        return process_code(line, index_line, IC, code_img, *symbol_table);
    }

    return TRUE;
}

/**
 * Builds an extra code word for an operand during the first pass.
 *
 * This function constructs an extra code word for an operand during the first pass of assembly.
 * It determines the addressing mode of the operand, extracts its value, builds the data word, and updates the code image accordingly.
 *
 * @param code_img Array to store machine words representing code instructions.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param operand The operand for which to build the extra code word.
 * @param is_src_operand Flag indicating whether the operand is a source operand.
 * @param symbol_table The symbol table containing defined symbols.
 */
static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand, bool is_src_operand, table symbol_table);

/**
 *  Builds an extra code word for a register addressing mode instruction during the first pass.
 *
 * This function constructs an extra code word for a register addressing mode instruction during the first pass.
 * It extracts the register values from the operands, builds the data word, and updates the code image accordingly.
 *
 * @param code_img Array to store machine words representing code instructions.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param operands Array containing operands extracted from the instruction.
 * @param symbol_table The symbol table containing defined symbols.
 */
static void build_extra_codeword_fpass_reg(machine_word **code_img, long *ic, char **operands, table symbol_table);

/* Processes a single code instruction during the first pass of assembly. */
static bool process_code(line_info line, int index_l, long *ic, machine_word **code_img, table symbol_table) {
    char operation[8];      /* Buffer to store the operation */
    char *operands[2];      /* Array to store operands */
    opcode curr_opcode;     /* Current opcode */
    code_word *codeword;    /* Code word */
    long ic_before;         /* Instruction counter before processing */
    int index_o = 0;        /* Index within the operation */
    int operand_count;      /* Number of operands */
    machine_word *word_to_write;  /* Machine word to write */

    /* Skip leading spaces */
    index_l = skip_spaces(line.content, index_l); 

    /* Extract operation */
    while (line.content[index_l] && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != '\n' && line.content[index_l] != EOF && index_o < 6) {
        operation[index_o] = line.content[index_l];
        index_l++;
        index_o++;
    }
    operation[index_o] = '\0';

    /* Get opcode */
    get_opcode(operation, &curr_opcode);

    /* Check if opcode is valid */
    if (curr_opcode == NONE_OP) {
        print_error(line, "Unrecognized instruction: %s.", operation);
        return FALSE;
    }

    /* Analyze operands */
    if (!analyze_operands(line, index_l, operands, &operand_count, operation, symbol_table)) {
        return FALSE;
    }

    /* Get code word */
    codeword = get_code_word(line, curr_opcode, operand_count, operands, symbol_table);
    if (codeword == NULL) {
        if (operands[0]) {
            free(operands[0]);
            if (operands[1]) {
                free(operands[1]);
            }
        }
        return FALSE;
    }

    /* Store instruction counter */
    ic_before = *ic;

    /* Allocate memory for machine word */
    word_to_write = (machine_word *)malloc(sizeof(machine_word));
    if (word_to_write == NULL) {
        printf("Memory allocation failed");
        return FALSE;
    }
    word_to_write->word.code = codeword;
    code_img[(*ic) - IC_INIT_VALUE] = word_to_write;

    /* Handle special cases for operand count */
    if (operand_count == 2) {
        if (get_addressing_type(operands[0], symbol_table) == REGISTER_ADDR) {
            if (get_addressing_type(operands[1], symbol_table) == REGISTER_ADDR) {
                build_extra_codeword_fpass_reg(code_img, ic, operands, symbol_table);
                free(operands[0]);
                free(operands[1]);
                operand_count = 0;
            }   
        }
    }

    /* Process operands */
    if (operand_count--) {
        build_extra_codeword_fpass(code_img, ic, operands[0], TRUE, symbol_table);
        free(operands[0]);
        if (operand_count) {
            build_extra_codeword_fpass(code_img, ic, operands[1], FALSE, symbol_table);
            free(operands[1]);
        }
    }

    /* Increment instruction counter */
    (*ic)++; 

    /* Update instruction length */
    code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;
    return TRUE; 
}

/* Builds an extra code word for a register addressing mode instruction during the first pass */
static void build_extra_codeword_fpass_reg(machine_word **code_img, long *ic, char **operands, table symbol_table) {
    machine_word *word_to_write;   /* Machine word to write */
    long first_value;               /* Value of the first register */
    long second_value;              /* Value of the second register */

    /* Increment instruction counter */
    (*ic)++;

    /* Extract register values */
    first_value = strtol(operands[0] + 1, NULL, 10);
    second_value = strtol(operands[1] + 1, NULL, 10);

    /* Allocate memory for machine word */
    word_to_write = (machine_word *)malloc(sizeof(machine_word));
    if (word_to_write == NULL) {
        printf("Memory allocation failed");
        exit(1);
    } else {
        /* Initialize machine word */
        word_to_write->length = 0; 
        word_to_write->word.data = build_data_word_reg(first_value, second_value);
        code_img[(*ic) - IC_INIT_VALUE] = word_to_write; 
    }
}

/* Builds an extra code word for an operand during the first pass */
static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand, bool is_src_operand, table symbol_table) {
    addressing_type operand_addr = get_addressing_type(operand, symbol_table); /* Addressing mode of the operand */

    /* Check if the operand has a valid addressing mode */
    if (operand_addr != NONE_ADDR) {
        (*ic)++; /* Increment instruction counter */

        /* Process immediate and register addressing modes */
        if (operand_addr == IMMEDIATE_ADDR || operand_addr == REGISTER_ADDR) {
            machine_word *word_to_write; /* Machine word to write */
            long value; /* Value of the operand */

            /* Extract value based on addressing mode */
            if (operand_addr == IMMEDIATE_ADDR) {
                value = strtol(operand + 1, NULL, 10); /* Extract immediate value */
            } else {
                value = get_register_by_name(operand); /* Extract register value */
            }
            
            /* Allocate memory for machine word */
            word_to_write = (machine_word *)malloc(sizeof(machine_word));
            if (word_to_write == NULL) {
                printf("Memory allocation failed");
                exit(1);
            } else {
                /* Initialize machine word */
                word_to_write->length = 0; 
                word_to_write->word.data = build_data_word(operand_addr, value, FALSE, is_src_operand);
                code_img[(*ic) - IC_INIT_VALUE] = word_to_write; 
            }
        }

        /* Increment instruction counter for index fixed addressing mode */
        if (operand_addr == INDEX_FIXED_ADDR){
            (*ic)++;
        }
    }
}