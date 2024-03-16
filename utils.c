#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "code.h"

/*Adds an extension for a file name*/
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

/* Skippes all the the spaces or tabs, returns a new index of the next char in the line*/
int skip_spaces(char *string, int index){
    while(string[index] && (string[index] == ' ' || string[index] == '\t')){
        index++;
    }
    return index;
}

/*Checks if the new lable if is OK*/
/*checks the length of the label*/
/*checks if there is any ather label or words with the same name*/
bool extract_label(line_info line, char *symbol){

    int index_l = 0; /* Index for the line*/
    int index_s = 0; /* Index for the symbol*/

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

    /* Saves the new lable name in to symbol*/
    while (line.content[index_l] && line.content[index_l] != ':' && line.content[index_l] != EOF && index_l <= MAX_LINE_LENGTH){
        symbol[index_s] = line.content[index_l];
        index_l++;
        index_s++;
    }

    symbol[index_s] = '\0'; /* End of string*/

    /* Checks the label name*/
    if(line.content[index_l] == ':'){
        if(!check_label_name(symbol)){
            printf("Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
            symbol[0] = '\0';
            return TRUE; /* The lable is not valid*/
        }
        return FALSE; 
    }

    symbol[0] = '\0';
    return FALSE; 
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


/* Returns the instruction by the input name*/
/* If there is no instruction by this name the function returns none_inst*/
instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;
		}
	}
	return NONE_INST;
}


/* Checks if the word is a special word*/
/* Special word - opcode, register, instruction*/
bool is_special_word(char *word){
    int opc;
    get_opcode(word, &opc);
    if(opc != NONE_OP || get_register_by_name(word) != NONE_REG || find_instruction_by_name(word) != NONE_INST){
        return TRUE;
    }
    return FALSE;
}

/* Checks if the string is alpha or digits*/
bool chec_all_str_alpha(char *string){
    int index;
    for (index = 0; string[index]; index++){
        if (!isalpha(string[index]) && !isdigit(string[index])){
            return FALSE;
        }
    }
    return TRUE;
}

/* Checks if the first char is alpha and all the others are alphe or digits, and if it's a special word*/
bool check_label_name(char *label_name){
    return label_name[0] && strlen(label_name) < 32 && isalpha(label_name[0]) && chec_all_str_alpha(label_name + 1) && !is_special_word(label_name);
}


/* check if all the chars in the givan string are digits*/
bool check_if_digit(char *string) {
    int index_s = 0;
    if (string[0] == '-' || string[0] == '+') { /*If the string statrts with "+" or "-" it's OK*/
        string++;
    }
    while (string[index_s] != '\0') {   /*Check if every char in the string is a digit*/
        if (!isdigit(string[index_s])) {
            return FALSE;
        }
        index_s++;
    }
    if (index_s == 0) {  /* In case it was an empy string*/
        return FALSE;
    }
    return TRUE;
}





