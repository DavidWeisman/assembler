#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

/* Returns the instruction from the given index, if not found returns NONE_INST*/
instruction find_instruction_from_index(line_info line, int *index_l) {
    char the_instruction[MAX_LINE_LENGTH];          /*--------------NEENS A NEW NAME  ---------*/
    int index_i = 0;    /* Indes of the instruction*/
    instruction result;

    *index_l = skip_spaces(line.content, *index_l); /*Skips all the spaces or tabs*/
    
    /*If there is no instruction */
    if(line.content[*index_l] != '.'){
        return NONE_INST;
    }

    /* Copying the instruction */
    while (line.content[*index_l] && line.content[*index_l] != '\t' && line.content[*index_l] != ' ')
    {
        the_instruction[index_i] = line.content[*index_l];
        index_i++;
        (*index_l)++;
    }
    the_instruction[index_i] = '\0'; /* End of string*/
    
    result = find_instruction_by_name(the_instruction + 1);
    if (result != NONE_INST){
        return result;
    }

    /* If the name of the instruction isn't currect*/
    printf("Invalid instruction name: %s", the_instruction);
    return ERROR_INST;
}

/* Process string Instruction */
bool process_string_instruction(line_info line, int index_l, long *data_img, long *dc) {
    char string[MAX_LINE_LENGTH];               /*----------- NEEDS NEW NAME ----------------*/

    char *last_quote_location = strrchr(line.content, '"'); /* The last quote location in the input line*/
    index_l = skip_spaces(line.content, index_l);  /*Skips all the spaces or tabs*/
    
    if (line.content[index_l] != '"') {
        /* If the instruction string isn't starts with quote*/
        printf("Missing opening quote of string");
        return FALSE;
    }
    else if (&line.content[index_l] == last_quote_location) { /* If the last quote is in the same location as the first one*/
        printf("Missing closing quote of string");
        return FALSE;
    }
    else {
        int index_s = 0; /* Indes of the instruction string*/
        /* Copying the string with the quotes */
        while (line.content[index_l] && line.content[index_l] != '\n' && line.content[index_l] != EOF){
            string[index_s] = line.content[index_l];
            index_l++;
            index_s++;
        }

        /* Puts the end of line sign insted of a quote */
        string[last_quote_location - line.content] = '\0';

        /*puts the inter of the string in the data image*/
        for (index_s = 1; string[index_s] && string[index_s] != '"'; index_s++){
            data_img[*dc] = string[index_s];
            (*dc)++;
        }
    }
    return TRUE; /* End of the process*/
}




bool process_data_instruction(line_info line, int index_l, long *data_img, long *dc){
    char number[MAX_LINE_LENGTH];
    char *temp_pointer;
    long number_value;
    int index_n;
    
    index_l = skip_spaces(line.content, index_l);

    if (line.content[index_l] == ',') {
        printf("Unexpected comma after .data instruction");
    }

    while (line.content[index_l] != '\n' && line.content[index_l] != EOF) {
        index_n = 0;
        while (check_char(line.content[index_l])) {
            number[index_n] = line.content[index_l];
            index_l++;
            index_n++;
        }

        number[index_n] = '\0';

        if (!check_if_digit(number)) {
            printf("Expected integer for .data instruction");
            return FALSE;
        }

        number_value = strtol(number, &temp_pointer, 10);
        data_img[*dc] = number_value;

        (*dc)++;
        index_l = skip_spaces(line.content, index_l);
        if (line.content[index_l] == ',') {
            index_l++;
        }
        else if (!line.content[index_l] || line.content[index_l] == '\n' || line.content[index_l] == EOF){
            break;
        }

        index_l = skip_spaces(line.content, index_l);
        if (line.content[index_l] == ',') {
            printf("Multiple consecutive commas.");
            return FALSE;
        }
        else if (line.content[index_l] == EOF || line.content[index_l] == '\n' || !line.content[index_l]) {
            printf("Missing data after comma");
            return FALSE;
        }
    }
    return TRUE;
}