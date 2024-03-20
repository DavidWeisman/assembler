#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"
#include "utils.h"
#include "instructions.h"
#include "first_pass.h"


/**
 *  Processes a single code line in the first pass.
 * Adds the code build binary structure to the code_img,
 * encodes immediately-addresses operands and leaves required data word that use labels NULL.
 * @param line The code line to process
 * @param index_l Where to start processing the line from
 * @param ic A pointer to the current instruction counter
 * @param code_img The code image array
 * @return Whether succeeded or notssss
 */
static bool process_code(line_info line, int index_l, long *ic, machine_word **code_img, table symbol_table);

/*Processes a single line in the first pass*/
bool process_line_fpass(line_info line, long *IC, long *DC, machine_word **code_img, long *data_img, table *symbol_table) {
    int index_l = 0;
    int index_s = 0;
    char symbol[MAX_LINE_LENGTH];
    instruction currentInstruction;                 

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/
    
    /*Cheaks if the line is empty or a comment line*/
    /*If it does return 1 - no error*/
    if (!line.content[index_l] || line.content[index_l] == '\n' || line.content[index_l] == EOF || line.content[index_l] == ';'){
        return TRUE;
    } 
    if (!check_mdefine(line, symbol_table, symbol)){
        return FALSE;
    }
    else if (find_by_types(*symbol_table, symbol) && find_by_types(*symbol_table, symbol)->type == MDEFINE_SYMBOL) {
        return TRUE;
    }
    /* Cheks the label*/
    /* Returns an error in case of tried to defind label, but it's invalid*/
    if(extract_label(line, symbol)){
        return FALSE;
    }

    /*Checks if illegal name*/
    if(symbol[0] && !check_label_name(symbol)){
        printf("Illegal label name: %s", symbol);
        return FALSE;
    }
    
    /*If symbol detected, start analyzing from it's deceleration end*/
    if(symbol[0] != '\0'){
        while (line.content[index_l] != ':'){
            index_l++;
        }
        index_l++;
    }

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

    /*Only label*/
    if (line.content[index_l] == '\n'){
        return TRUE;
    }
    
    /*Checks if the symblol was alrady defined as data/external/cod */ 
    if (find_by_types(*symbol_table, symbol)){
        printf("Symbol %s is already defined.", symbol);
        return FALSE;
    }

    /* Checks if it's an instruction, if it is saves in the varivale */
    currentInstruction = find_instruction_from_index(line, &index_l);
    
    if (currentInstruction == ERROR_INST){  /* If the instruction have an erroe */
        return FALSE;
    }

    index_l = skip_spaces(line.content, index_l);  /*Skips all the spaces or tabs*/
    
    /* If it's is an instruction */
    if (currentInstruction != NONE_INST) {
        
        /* If it's .string or .data, and the symmbal defined, puts it into hte symbol table*/
        if ((currentInstruction == DATA_INST || currentInstruction == STRING_INST) && symbol[0] != '\0'){
            
            /* If it's data or string, add DC with the symbol to the table as data*/
            add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);
        }

        /* If it is a string, encode into data image and increase dc as needed*/
        if(currentInstruction == STRING_INST){
            return process_string_instruction(line, index_l, data_img, DC);
        }
        /*If it is a data, encode into data image and increase dc as needed*/
        else if(currentInstruction == DATA_INST){
            return process_data_instruction(line, index_l, data_img, DC); 
        }
        /* if external symbol detected, start analyzing from it's deceleration end */
        else if (currentInstruction == EXTERN_INST) {
            index_l = skip_spaces(line.content, index_l);
            
            while (line.content[index_l] && line.content[index_l] != '\n' && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != EOF) {
                symbol[index_s] = line.content[index_l];
                index_l++;
                index_s++;
            }
            /* If invalid external label name, it's an error */
            symbol[index_s] = 0;
            if (!check_label_name(symbol)) {
                printf("Invalid external label name");
                return TRUE;
            }
            add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL); /* Extern value is defaulted to 0 */
        }
        /* if entry and symbol defined, print error */
        else if (currentInstruction == ENTRY_INST && symbol[0] != '\0') {
            printf("Can't define a label to an entry instruction.");
            return FALSE;
        }
        /* .entry is handled in second pass! */
    }
    /* not instruction=>it's a command! */
    else {
        /* if symbol defined, add it to the table */
        if (symbol[0] != '\0'){
            add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
        }
        /* Analyze code */
        return process_code(line, index_l, IC, code_img, *symbol_table);
    }
    return TRUE;
}

/** 
 * Allocates and builds the data inside the additional code word by the given operand,
 * Only in the first pass
 * @param code_img The current code image
 * @param ic The current instruction counter
 * @param operand the operand to check
 * @param is_src_operand If the operand is a source operand
*/
static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand, bool is_src_operand);


/**
 *  Processes a single code line in the first pass.
 * Adds the code build binary structure to the code_img,
 * encodes immediately-addresses operands and leaves required data word that use labels NULL.
 * @param line The code line to process
 * @param index_l Where to start processing the line from
 * @param ic A pointer to the current instruction counter
 * @param code_img The code image array
 * @return Whether succeeded or notssss
 */
static bool process_code(line_info line, int index_l, long *ic, machine_word **code_img, table symbol_table) {
    char operation[8]; /* stores the string of the current code instruction */
	char *operands[2]; /* 2 strings, each for operand */
	opcode curr_opcode;
    code_word *codeword; /* The current code word */
	long ic_before;
    int index_o = 0;
    int operand_count;
    machine_word *word_to_write;

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/
    
    /* copyes the operation */
    while (line.content[index_l] && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != '\n' && line.content[index_l] != EOF && index_o < 6) {
        operation[index_o] = line.content[index_l];
        index_l++;
        index_o++;
    }
    operation[index_o] = '\0'; /* End of string */
    
    /* Get opcode into curr_opcode */
    get_opcode(operation, &curr_opcode);

    /* If invalid operation */
    if (curr_opcode == NONE_OP) {
        printf("Unrecognized instruction");
        return FALSE;
    }
    
    /* Separate operands and get their count */
    if (!analyze_operands(line, index_l, operands, &operand_count, operation, symbol_table))  {
		return FALSE;
	}
    
    /* Build the code word */
    codeword = get_code_word(line, curr_opcode, operand_count, operands);
    if (codeword == NULL) {
        /* If there where any erroe in the building release teh allocated memory of the operans*/
        if (operands[0]) {
            free(operands[0]);
            if (operands[1]) {
                free(operands[1]);
            }
        }
        return FALSE;
    }
    /* ic is in position of a new code word*/
    ic_before = *ic;
   
    /* allocate memory for a new word in the code image, and put the code word into it */
    word_to_write = (machine_word *)malloc(sizeof(machine_word));
    if (word_to_write == NULL) {
        printf("Memory allocation failed");
        return FALSE;
    }
    (word_to_write->word).code = codeword;
    code_img[(*ic) - IC_INIT_VALUE] = word_to_write; /* Avoid "spending" cells of the array, by starting from initial value of ic */

    /* Build extra information code word if possible, free pointers with no need */
    if (operand_count--) { /* If there's 1 operand at least */
        build_extra_codeword_fpass(code_img, ic, operands[0], TRUE);
        free(operands[0]);
        if (operand_count) { /* If there are 2 operands */
            build_extra_codeword_fpass(code_img, ic, operands[1], FALSE);
            free(operands[1]);
        }
    }

    (*ic)++; /* increase ic to point the next cell */

    /* Add the final length (of code word + data words) to the code word struct: */
    code_img[ic_before - IC_INIT_VALUE]->length = (*ic) - ic_before;

    return TRUE; /* No errors */
}


static void build_extra_codeword_fpass(machine_word **code_img, long *ic, char *operand, bool is_src_operand) {
    addressing_type operand_addr = get_addressing_type(operand);
    if (operand_addr != NONE_ADDR) {
        (*ic)++;
        if (operand_addr == IMMEDIATE_ADDR || operand_addr == REGISTER_ADDR) {
            char *ptr;
            machine_word *word_to_write;
            long value;
            if (operand_addr == IMMEDIATE_ADDR) {
                value = strtol(operand + 1, &ptr, 10);
            }
            else {
                value = get_register_by_name(operand);
            }
            
            word_to_write = (machine_word *)malloc(sizeof(machine_word));
            if (word_to_write == NULL) {
                printf("Memory allocation failed");
                exit(1);
            }
            
            word_to_write->length = 0; /* Not Code word! */
            (word_to_write->word).data = build_data_word(operand_addr, value, FALSE, is_src_operand);
            code_img[(*ic) - IC_INIT_VALUE] = word_to_write; /* Avoid "spending" cells of the array, by starting from initial value of ic */
        }
    }
}