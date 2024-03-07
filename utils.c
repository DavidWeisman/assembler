#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "code.h"

char* add_extension(char *file_name, char *extension ){

    char *new_ex = (char*)malloc((strlen(file_name) + strlen(extension) + 1) * sizeof(char));

    if(new_ex == NULL){
        printf("Memory allocation failed.");
        return NULL;
    }
    
    strcpy(new_ex, file_name);
    strcat(new_ex, extension);

    return new_ex;
}



int skip_spaces(char *string, int index){
    while(string[index] && (string[index] == ' ' || string[index] == '\t')){
        index++;
    }
    return index;
}


bool extract_label(line_info line, char *symbol){

    int index_l = 0;
    int index_s = 0;

    index_l = skip_spaces(line.content, index_l);

    while (line.content[index_l] && line.content[index_l] != ':' && line.content[index_l] != EOF && index_l <= MAX_LINE_LENGTH)
    {
        symbol[index_s] = line.content[index_l];
        index_l++;
        index_s++;
    }

    symbol[index_s] = '\0';

    return TRUE;
    

}



struct instruction_lookup_item {
	char *name;
	instruction value;
};

static struct instruction_lookup_item
		instructions_lookup_table[] = {
		{"string", STRING_INST},
		{"data",   DATA_INST},
		{"entry",  ENTRY_INST},
		{"extern", EXTERN_INST},
		{NULL, NONE_INST}
};



instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}



bool is_special_word(char *word){

    int opc;

    get_opcode(word, &opc);
    if(opc != NONE_OP || get_register_by_name(word) != NONE_REG || find_instruction_by_name(word) != NONE_INST){
        return TRUE;
    }
    return FALSE;
}


bool chec_all_str_alpha(char *string){
    int index;
    for (index = 0; string[index]; index++){
        if (!isalpha(string[index]) && !isdigit(string[index])){
            return FALSE;
        }
    }
    return TRUE;
}

bool check_label_name(char *label_name){
    return label_name[0] && strlen(label_name) < 32 && isalpha(label_name[0]) && chec_all_str_alpha(label_name + 1) && !is_special_word(label_name);
}

