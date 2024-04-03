#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "analysis_phase.h"
#include "code_functions.h"
#include "utility_functions.h"

/**
 *  Processes a single operand in SPASS assembly code.
 *
 * This function processes a single operand extracted from the line content.
 * It determines the addressing type of the operand, updates the current instruction counter accordingly,
 * and writes the operand to the code image.
 *
 * @param line The line_info structure containing information about the current line.
 * @param curr_ic Pointer to the current instruction counter.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param operand The operand to be processed.
 * @param code_img Array to store machine words representing code instructions.
 * @param symbol_table Pointer to the symbol table containing defined symbols.
 * @return Returns TRUE if the operand is successfully processed, otherwise returns FALSE.
 */
bool process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img, table *symbol_table);

/* Processes a line in the second pass of the assembly process */
bool process_line_spass(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
    char *index_of_colon;   /* Pointer to the location of a colon in the line content (if present) */
    char *token;            /* Token extracted from the line content */
    long index_instruction = 0; /* Index within the line content indicating the start of the instruction */

    /* Skip leading spaces */
    index_instruction = skip_spaces(line.content, index_instruction); 

    /* Check for comments or empty lines */
    if (line.content[index_instruction] == ';' || line.content[index_instruction] == '\n') {
        return TRUE;
    }

    /* Check for label and update index accordingly */
    index_of_colon = strchr(line.content, ':');
    if (index_of_colon != NULL) {
        index_instruction = index_of_colon - line.content + 1;
    }

    /* Skip leading spaces after the label */
    index_instruction = skip_spaces(line.content, index_instruction); 

    /* Check if the line contains a directive */
    if (line.content[index_instruction] == '.') {
        /* Process .entry directive */
        if (strncmp(".entry", line.content, 6) == 0) {
            index_instruction += 6; /* Move index past ".entry" */
            index_instruction = skip_spaces(line.content, index_instruction); /* Skip spaces */
            token = strtok(line.content + index_instruction, "\n\t"); /* Extract token */

            if (token == NULL) {
                print_error(line, "You have to specify a label name for .entry instruction.");
                return FALSE;
            }
            if (find_by_types(*symbol_table, token)->type != ENTRY_SYMBOL) {
                table_entry *item;
                token = strtok(line.content + index_instruction, "\n"); /* Extract token */

                item = find_by_types(*symbol_table, token);
                if (item->type != DATA_SYMBOL && item->type != CODE_SYMBOL) {
                    item = find_by_types(*symbol_table, token);
                    if (item->type == EXTERNAL_SYMBOL) {
                        print_error(line, "The symbol %s can be either external or entry, but not both.", item->type);
                        return FALSE;
                    }
                    print_error(line, "The symbol %s for .entry is undefined.", token);
                    return FALSE;
                }
                /* Add the symbol to the symbol table as an entry */
                add_table_item(symbol_table, token, item->value, ENTRY_SYMBOL);
            }
        }
        return TRUE;
    }
    /* Process code instructions and add symbols to the code image */
    return add_symbols_to_code(line, ic, code_img, symbol_table);
}


bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
    char temp_string[MAX_LINE_LENGTH];  /* Temporary string buffer */
    char *operands[2];                  /* Array to store operands extracted from the line content */
    int index_line = 0;                  /* Index within the line content */
    int operand_count;                   /* Number of operands */
    bool is_valid = TRUE;                /* Flag to indicate if the operation is valid */
    long current_ic = *ic;               /* Current instruction counter */
    int length = code_img[(*ic) - IC_INIT_VALUE]->length; /* Length of the current instruction */

    /* Check if the instruction has operands */
    if (length > 1) {
        /* Skip leading spaces */
        index_line = skip_spaces(line.content, index_line); 

        /* Extract label and move index if present */
        extract_label(line, temp_string);
        if (temp_string[0] != '\0') {  
            while (line.content[index_line] && line.content[index_line] != '\n' && line.content[index_line] != EOF && line.content[index_line] != ' ' && line.content[index_line] != '\t') {
                index_line++;
            }
			index_line++;
        }
        index_line = skip_spaces(line.content, index_line); 
        while (line.content[index_line] && line.content[index_line] != ' ' && line.content[index_line] != '\t' && line.content[index_line] != '\n' && line.content[index_line] != EOF) {
            index_line++;
        }
       
        /* Skip leading spaces */
        index_line = skip_spaces(line.content, index_line); 

        /* Extract and analyze operands */
        analyze_operands(line, index_line, operands, &operand_count, NULL, *symbol_table);

        /* Process operands */
        if (operand_count--) {
            is_valid = process_spass_operand(line, &current_ic, ic, operands[0], code_img, symbol_table);
            free(operands[0]);
            if (!is_valid) {
                return FALSE;
            }
            if (operand_count) {
                is_valid = process_spass_operand(line, &current_ic, ic, operands[1], code_img, symbol_table);
                free(operands[0]);
                if (!is_valid) {
                    return FALSE;
                }
            }
        }
    }
    /* Update instruction counter */ 
    (*ic) = (*ic) + length;
    return TRUE;
}



/* Processes a single operand in SPASS assembly code */
bool process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img, table *symbol_table) {
    addressing_type addr = get_addressing_type(operand, *symbol_table); /* Get the addressing type of the operand */
    machine_word *word_to_write;  /* Machine word to store the operand */

    /* Increment current instruction counter for immediate and register addressing modes */
    if (addr == IMMEDIATE_ADDR || addr == REGISTER_ADDR) {
        (*curr_ic)++;
    }

    /* Process operand based on addressing type */
    if (DIRECT_ADDR == addr || INDEX_FIXED_ADDR == addr) {
        long data_to_add;
        table_entry *item;

        /* Handle indexed addressing mode */
        if (INDEX_FIXED_ADDR == addr) {
            int index_offset = 0;  /* Index within the operand string */
            int temp_index = 0;    /* Temporary index */
            char label[MAX_LINE_LENGTH];  /* Label extracted from the operand */
            char number[MAX_LINE_LENGTH]; /* Index number extracted from the operand */

            /* Extract label */
            index_offset = skip_spaces(operand, index_offset);
            while (operand[index_offset] != '[') {
                label[temp_index] = operand[index_offset];
                index_offset++;
                temp_index++;
            }
            label[temp_index] = '\0';
            index_offset++;
            temp_index = 0;

            /* Extract index number */
            while (operand[index_offset] != ']') {
                number[temp_index] = operand[index_offset];
                index_offset++;
                temp_index++;
            }
            number[temp_index] = '\0';

            /* Convert index number */
            convert_defind(number, *symbol_table, FALSE);

            /* Find the symbol in the symbol table */
            item = find_by_types(*symbol_table, label);
            if (item == NULL) {
                print_error(line, "The symbol %s not found", operand);
                return FALSE;
            }

            /* Set data to add */
            data_to_add = item->value;

            /* Handle external symbols */
            if (item->type == EXTERNAL_SYMBOL) {
                add_table_item(symbol_table, operand, (*curr_ic) + 1, EXTERNAL_REFERENCE);
            }

            /* Write data word for label */
            word_to_write = (machine_word *)malloc(sizeof(machine_word));
            if (word_to_write == NULL){
                printf("Memory allocation failed\n");  
                return FALSE;
            }
            word_to_write->length = 0;
            word_to_write->word.data = build_data_word(DIRECT_ADDR, data_to_add, item->type == EXTERNAL_SYMBOL, FALSE);
            code_img[(++(*curr_ic)) - IC_INIT_VALUE] = word_to_write;

            /* Write data word for index */
            word_to_write = (machine_word *)malloc(sizeof(machine_word));
            if (word_to_write == NULL){
                printf("Memory allocation failed\n");  
                return FALSE;
            }
            word_to_write->length = 0;
            word_to_write->word.data = build_data_word(IMMEDIATE_ADDR, strtol(number, NULL, 10), FALSE, FALSE);
            code_img[(++(*curr_ic)) - IC_INIT_VALUE] = word_to_write;
        }
        else {
            /* Handle direct addressing mode */
            item = find_by_types(*symbol_table, operand);
            if (item == NULL) {
                print_error(line, "The symbol %s not found", operand);
                return FALSE;
            }

            /* Set data to add */
            data_to_add = item->value;

            /* Handle external symbols */
            if (item->type == EXTERNAL_SYMBOL) {
                add_table_item(symbol_table, operand, (*curr_ic) + 1, EXTERNAL_REFERENCE);
            }

            /* Write data word */
            word_to_write = (machine_word *)malloc(sizeof(machine_word));
            if (word_to_write == NULL){
                printf("Memory allocation failed\n");  
                return FALSE;
            }
            word_to_write->length = 0;
            word_to_write->word.data = build_data_word(addr, data_to_add, item->type == EXTERNAL_SYMBOL, FALSE);
            code_img[(++(*curr_ic)) - IC_INIT_VALUE] = word_to_write;
        }
    }
    return TRUE;
}