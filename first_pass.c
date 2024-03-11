#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"
#include "utils.h"
#include "instructions.h"
#include "first_pass.h"


/*Processes a single line in the first pass*/
bool process_line_fpass(line_info line, table *symbol_table, long *IC, long *DC, long *data_img, machine_word **code_img){
    int index_l = 0;
    int index_s = 0;
    char symbol[MAX_LINE_LENGTH];
    instruction the_instruction;                  /*-----------------NEEDS NEW NAME----------------*/

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

    /*Cheaks if the line is empty or a comment line*/
    /*If it does return 1 - no error*/
    if (!line.content[index_l] || line.content[index_l] == '\n' || line.content[index_l] == EOF || line.content[index_l] == ';'){
        return TRUE;
    } 

    /* Cheks the label*/
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
    the_instruction = find_instruction_from_index(line, &index_l);

    if (the_instruction == ERROR_INST){  /* If the instruction have an erroe */
        return FALSE;
    }

    index_l = skip_spaces(line.content, index_l);  /*Skips all the spaces or tabs*/

    /* If it's is an instruction */
    if (the_instruction != NONE_INST) {
        
        /* If it's .string or .data, and the symmbal defined, puts it into hte symbol table*/
        if ((the_instruction == DATA_INST || the_instruction == STRING_INST) && symbol[0] != '\0'){
            
            /* If it's data or string, add DC with the symbol to the table as data*/
            add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);
        }

        /* If it is a string, encode into data image and increase dc as needed*/
        if(the_instruction == STRING_INST){
            return process_string_instruction(line, index_l, data_img, DC);
        }
        else if(the_instruction == DATA_INST){
            return process_data_instruction(line, index_l, data_img, DC); 
        }
        else if (the_instruction == EXTERN_INST) {
            index_l = skip_spaces(line.content, index_l);
            
            while (line.content[index_l] && line.content[index_l] != '\n' && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != EOF) {
                symbol[index_s] = line.content[index_l];
                index_l++;
                index_s++;
            }
            symbol[index_s] = 0;
            if (!check_label_name(symbol)) {
                printf("Invalid external label name");
                return TRUE;
            }
            add_table_item(symbol_table, symbol, 0, EXTERNAL_SYMBOL);
        }
        else if (the_instruction == ENTRY_INST && symbol[0] != '\0') {
            printf("Can't define a label to an entry instruction.");
            return FALSE;
        }
    }
    else {
        if (symbol[0] != '\0'){
            add_table_item(symbol_table, symbol, *IC, CODE_SYMBOL);
        }
        /* return process_code(line, i, IC, code_img);*/
    }
    return TRUE;
}

static bool process_code(line_info line, int index_l, long *ic, machine_word **code_img) {
    char operation[8]; 
	char *operands[2]; 
	opcode curr_opcode;
    code_word *codeword; 
	long ic_before;
    int index_o = 0;
    int operand_count;
    machine_word *word_to_write;

    index_l = skip_spaces(line.content, index_l);
    while (line.content[index_l] && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != '\n' && line.content[index_l] != EOF && index_o < 6) {
        operation[index_o] = line.content[index_l];
    }
    operation[index_o] = '\0';

    get_opcode(operation, &curr_opcode);

    if (curr_opcode == NONE_OP) {
        printf("Unrecognized instruction");
        return FALSE;
    }

    if (!analyze_operands(line, index_l, operands, &operand_count, operation))  {
		return FALSE;
	}

}