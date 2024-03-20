#include <stdio.h>
#include <stdlib.h>
#include "second_pass.h"
#include "code.h"
#include "utils.h"
#include "string.h"

bool process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img, table *symbol_table);

/**
 * processes a single line in the second pass
 * @param line The line we work on
 * @param ic A pointer to instruction counter
 * @param code_img The code image
 * @param symbol_table The simbol table
 * @return True of false if the process succeeded or not
*/
bool process_line_spass(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
    char *index_of_colon;
    char *token;
    long index_l = 0;
    
    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

    /* Cheaks if the line is empty or a comment line */
    /* If it does return 1 - no error */
    if (line.content[index_l] == ';' || line.content[index_l] == '\n') {
        return TRUE;
    }

    index_of_colon = strchr(line.content, ':');

    /*checks if there is any labels*/
    if (index_of_colon != NULL) {
        index_l = index_of_colon - line.content;
        index_l++;
    }

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

    if (line.content[index_l] == '.') {
        /* If it's an entry adds it to the symbol table */
        if (strncmp(".entry", line.content, 6) == 0) {
            index_l += 6;
            index_l = skip_spaces(line.content, index_l);
            token = strtok(line.content + index_l, "\n\t");

            /* If teh label is alrady defindes as entry, ignore*/
            if (token == NULL) {
                printf("You have to specify a label name for .entry instruction.");
                return FALSE;
            }

            if (find_by_types(*symbol_table, token)->type != ENTRY_SYMBOL) {
                table_entry *item;
                token = strtok(line.content + index_l, "\n"); /*Gets the name of teh label*/

                /* If the symbol isn't defined as data or code */
                item = find_by_types(*symbol_table, token);
                if (item->type != DATA_SYMBOL && item->type != CODE_SYMBOL) {
                    
                    /* If defined as external */
                    item = find_by_types(*symbol_table, token);
                    if (item->type == EXTERNAL_SYMBOL) {
                        printf("The symbol can be either external or entry, but not both.");
                        return FALSE;
                    }
                    printf("The symbol for .entry is undefined.\n");
                    return FALSE;
                }
                /* otherwise print more general error */
                add_table_item(symbol_table, token, item->value, ENTRY_SYMBOL);
            }
        }
        return TRUE;
    }
    return add_symbols_to_code(line, ic, code_img, symbol_table);
}

/**
 * Find the symbol that need replacment in a code line, and replacing them by the address in the symbol table.
 * @param line The current code line that is being processed
 * @param ic A pointer to the current instruction counter
 * @param code_img The machine code image array
 * @param data_table The data symbol table
 * @param code_table The code symbol table
 * @param ext_table The externals symbol table
 * @param ext_references A pointer to the external symbols references table
 * @return True of false if succeeded or not
 */
bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table) {
    char temp_string[MAX_LINE_LENGTH];
    char *operands[2];
    int index_l = 0;
    int operand_count;
    bool isvalid = TRUE;
    long curr_ic = *ic; /* Using curr_ic as temp index inside the code image, in the current line code+data words */
    /* Get the total word length of current code text line in code binary image */
    int length = code_img[(*ic) - IC_INIT_VALUE]->length;

    /* if the length is 1, then there's only the code word, no data. */
    if (length > 1) {
        index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/
        extract_label(line, temp_string);
        if (temp_string[0] != '\0') {  /* if symbol is defined */
            while (line.content[index_l] && line.content[index_l] != '\n' && line.content[index_l] != EOF && line.content[index_l] != ' ' && line.content[index_l] != '\t') {
                index_l++;
            }
			index_l++;
        }
        index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

        /* Skip command */
        while (line.content[index_l] && line.content[index_l] != ' ' && line.content[index_l] != '\t' && line.content[index_l] != '\n' && line.content[index_l] != EOF) {
            index_l++;
        }
        index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

        /* now analyze operands We send NULL as string of command because no error will be printed, and that's the only usage for it there. */
        analyze_operands(line, index_l, operands, &operand_count, NULL, *symbol_table);

        /* Process operands, if needed. if failed return failure. otherwise continue */
        if (operand_count--) {
            isvalid = process_spass_operand(line, &curr_ic, ic, operands[0], code_img, symbol_table);
            free(operands[0]);
            if (!isvalid) {
                return FALSE;
            }
            if (operand_count) {
                isvalid = process_spass_operand(line, &curr_ic, ic, operands[1], code_img, symbol_table);
                free(operands[0]);
                if (!isvalid) {
                    return FALSE;
                }
            }
        }
    } 
    /* Make the current pass IC as the next line ic */
    (*ic) = (*ic) + length;
    return TRUE;
}



/**
 * Builds the additional data word for operand in the second pass, if needed.
 * @param curr_ic Current instruction pointer of source code line
 * @param ic Current instruction pointer of source code line start
 * @param operand The operand string
 * @param code_img The code image array
 * @param symbol_table The symbol table
 * @return True or false if succeeded or not
 */
bool process_spass_operand(line_info line, long *curr_ic, long *ic, char *operand, machine_word **code_img, table *symbol_table) {
    addressing_type addr = get_addressing_type(operand);
    machine_word *word_to_write;

    /* if the word on *IC has the immediately addressed value (done in first pass), go to next cell (increase ic) */
    if (addr == IMMEDIATE_ADDR || addr == REGISTER_ADDR) {
        (*curr_ic)++;
    }
    
    if (DIRECT_ADDR == addr || INDEX_FIXED_ADDR == addr) {
        long data_to_add;
        table_entry *item = find_by_types(*symbol_table, operand);
        if (item == NULL) {
            printf("The symbol not found %s, %d, %d\n", operand, line.line_number, addr);
            return FALSE;
        }
        /* The symbol found*/
        data_to_add = item->value;

        if (addr == INDEX_FIXED_ADDR) {
            if (item->type != CODE_SYMBOL) {
                printf("The symbol cannot be addressed relatively because it's not a code symbol.");
                return FALSE;
            }
            data_to_add = data_to_add -*ic;
        }

        /* Add to externals reference table if it's an external. increase ic because it's the next data word */
        if (item->type == EXTERNAL_SYMBOL) {
            add_table_item(symbol_table, operand, (*curr_ic) + 1, EXTERNAL_REFERENCE);
        }

        /* The symbol found*/        
        word_to_write = (machine_word *)malloc(sizeof(machine_word));
        if (word_to_write == NULL){
            printf("Memory allocation failed\n");  
            return FALSE;
        }
        word_to_write->length = 0;
        word_to_write->word.data = build_data_word(addr, data_to_add, item->type == EXTERNAL_SYMBOL, FALSE);
        code_img[(++(*curr_ic)) - IC_INIT_VALUE] = word_to_write;
    }
    return TRUE;
}