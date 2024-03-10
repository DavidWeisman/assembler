#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "code.h"
#include "utils.h"
#include "instructions.h"
#include "first_pass.h"


/*Processes a single line in the first pass*/
void process_line_fpass(line_info line, table *symbol_table, long *IC, long *DC, long *data_img){

    int index = 0;
    char symbol[MAX_LINE_LENGTH];
    instruction the_instruction;                  /*-----------------NEEDS NEW NAME----------------*/

    index = skip_spaces(line.content, index); /*Skips all the spaces or tabs*/

    /*Cheaks if the line is empty or a comment line*/
    /*If it does return 1 - no error*/
    if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF || line.content[index] == ';'){
        return;
    } 

    /* Cheks the label*/
    if(extract_label(line, symbol)){
        return;
    }

    /*Checks if illegal name*/
    if(symbol[0] && !check_label_name(symbol)){
        printf("Illegal label name: %s", symbol);
        return;
    }

    /*If symbol detected, start analyzing from it's deceleration end*/
    if(symbol[0] != '\0'){
        while (line.content[index] != ':'){
            index++;
        }
        index++;
    }

    index = skip_spaces(line.content, index); /*Skips all the spaces or tabs*/

    /*Only label*/
    if (line.content[index] == '\n'){
        return;
    }

    /*Checks if the symblol was alrady defined as data/external/cod */ 
    if (find_by_types(*symbol_table, symbol)){
        printf("Symbol %s is already defined.", symbol);
        return;
    }

    /* Checks if it's an instruction, if it is saves in the varivale */
    the_instruction = find_instruction_from_index(line, &index);

    if (the_instruction == ERROR_INST){  /* If the instruction have an erroe */
        return;
    }

    index = skip_spaces(line.content, index);  /*Skips all the spaces or tabs*/

    /* If it's is an instruction */
    if (the_instruction != NONE_INST) {
        
        /* If it's .string or .data, and the symmbal defined, puts it into hte symbol table*/
        if ((the_instruction == DATA_INST || the_instruction == STRING_INST) && symbol[0] != '\0'){
            
            /* If it's data or string, add DC with the symbol to the table as data*/
            add_table_item(symbol_table, symbol, *DC, DATA_SYMBOL);
        }

        /* If it is a string, encode into data image and increase dc as needed*/
        if(the_instruction == STRING_INST){
            if (process_string_instruction(line, index, data_img, DC)){
                return;
            }
            return;
        }




    }



}
