#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


instruction find_instruction_from_index(line_info line, int *index_l) {
    char the_instruction[MAX_LINE_LENGTH];
    int index_i = 0;
    instruction result;

    *index_l = skip_spaces(line.content, *index_l);
    if(line.content[*index_l] != '.'){
        return NONE_INST;
    }

    while (line.content[*index_l] && line.content[*index_l] != '\t' && line.content[*index_l] != ' ')
    {
        the_instruction[index_i] = line.content[*index_l];
        index_i++;
        (*index_l)++;
    }
    the_instruction[index_i] = '\0';
    
    result = find_instruction_by_name(the_instruction + 1);
    if (result != NONE_INST){
        return result;
    }

    printf("Invalid instruction name: %s", the_instruction);
    return ERROR_INST;

}

bool process_string_instruction(line_info line, int index_l, long *data_img, long *dc) {
    char string[MAX_LINE_LENGTH];
    char *last_quote_location = strrchr(line.content, '"');
    index_l = skip_spaces(line.content, index_l);
    if (line.content[index_l] != '"') {
        printf("Missing opening quote of string");
        return FALSE;
    }
    else if (&line.content[index_l] == last_quote_location) {
        printf("Missing closing quote of string");
        return FALSE;
    }
    else {
        int index_s = 0;
        while (line.content[index_l] && line.content[index_l] != '\n' && line.content[index_l] != EOF){
            string[index_s] = line.content[index_l];
            index_l++;
            index_s++;
        }

        string[last_quote_location - line.content] = '\0';
        for (index_s = 1; string[index_s] && string[index_s] != '"'; index_s++){
            data_img[*dc] = string[index_s];
            (*dc)++;
        }
    }
    return TRUE;
}