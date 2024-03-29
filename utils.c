#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "utils.h"
#include "code.h"
#include "table.h"

#define ERR_OUTPUT_FILE stderr

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

/* Chckes if the lable is a mdefine lable */
bool check_mdefine(line_info line, table *symbol_table, char *symbol) {
    char lable[MAX_LINE_LENGTH];
    char number[MAX_LINE_LENGTH];
    long long_number;
    long index_l = 0; /* The index of line, used to track the position within a line of the input file*/
    long temp_index = 0; /* Index for copy of the lable and copy of the number*/
    table_entry *item;  /* Item of the symbol table */
    
    index_l = skip_spaces(line.content, index_l);   /*Skips all the spaces or tabs*/

    if (line.content[index_l] == '.') {
        /*if it's mdefind we add it to the symbol table*/
        if (strncmp(".define", line.content, 6) == 0) {
            index_l += 7;
            index_l = skip_spaces(line.content, index_l);   /*Skips all the spaces or tabs*/
            temp_index = 0;

            /* Copys the lable name */
            while (line.content[index_l] && line.content[index_l] != ' ' && line.content[index_l] != '=' && line.content[index_l] != '\n' && line.content[index_l] != EOF && index_l <= MAX_LINE_LENGTH){
                lable[temp_index] = line.content[index_l];
                symbol[temp_index] = line.content[index_l];
                index_l++;
                temp_index++;
            }
            lable[temp_index] = '\0';
            symbol[temp_index] = '\0';

            /* Checks if the label is already marked as mdefine */
            if (lable == NULL) {
                printf("You have to specify a label name for .define");
                return FALSE;
            }

            /* Checks if the lable name is valid */
            if (!check_label_name(lable)) {
                printf("Iligal name");
                return FALSE;
            }

            index_l = skip_spaces(line.content, index_l);   /*Skips all the spaces or tabs*/
            if (line.content[index_l] != '=') {
                printf("Error: '=' not found in the input string.\n");
                return FALSE;
            }
            index_l++;

            index_l = skip_spaces(line.content, index_l);   /*Skips all the spaces or tabs*/
            temp_index = 0;
            /* Copys the number */
            while (line.content[index_l] && line.content[index_l] != ' ' && line.content[index_l] != '\n' && line.content[index_l] != EOF && index_l <= MAX_LINE_LENGTH){
                number[temp_index] = line.content[index_l];
                index_l++;
                temp_index++;
            }    
            number[temp_index] = '\0';

            /* Check if there is any number */
            if (number == NULL) {
                printf("Error: Number not found after '='.\n");
                return FALSE;
            }

            /* Checks if the number is valid */
            if (!check_if_digit(number)) {
                printf("It isn't a digit");
                return FALSE;
            }
            index_l = skip_spaces(line.content, index_l);   /*Skips all the spaces or tabs*/
            
            if (!line.content[index_l]) {
                printf("Error: Unexpected characters after the number.\n");
                return FALSE;
            }
              
            item = find_by_types(*symbol_table, lable);
            
            if (item) {
                /* If the label was alrady defined as mdefine */
                if (item->type == MDEFINE_SYMBOL) {
                printf("You cannot define a mdefine type more than once with the same name.");
                return FALSE;
                }
                /* If the labble was alrady defined as any instruction */
                else {
                    printf("You cannot define type define and instruction with the same name.");
                    return FALSE;
                }
            }
            /* Convert the number into long number*/
            long_number = atoi(number);

            /* Adds new item to the symbol table */
            add_table_item(symbol_table, lable, long_number, MDEFINE_SYMBOL);
            return TRUE;     
        }
        return TRUE;
    }
    return TRUE;
}

/*Checks if the new lable if is valid*/
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
            print_error(line, "Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
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


void free_code_image(machine_word **code_image, long fic) {
	long i;
	/* for each not-null cell (we might have some "holes", so we won't stop on first null) */
	for (i = 0; i < fic; i++) {
		machine_word *curr_word = code_image[i];
		if (curr_word != NULL) {
			/* free code/data word */
			if (curr_word->length > 0) {
				free(curr_word->word.code);
			} else {
				free(curr_word->word.data);
			}
			/* free the pointer to the union */
			free(curr_word);
			code_image[i] = NULL;
		}
	}
}


 /* Prints a detailed error message, including file name and line number by the specified message */
bool print_error(line_info line, char *massage, ...) {
    bool result;
	va_list args; /* for formatting */
	/* Print file+line */
	fprintf(ERR_OUTPUT_FILE,"Error In %s:%ld: ", line.file_name, line.line_number);

	/* use vprintf to call printf from variable argument function (from stdio.h) with message + format */
	va_start(args, massage);
	result = vfprintf(ERR_OUTPUT_FILE, massage, args);
	va_end(args);

	fprintf(ERR_OUTPUT_FILE, "\n");
	return result;
}


