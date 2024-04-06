#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "utility_functions.h"
#include "code_functions.h"
#include "data_tables.h"

#define ERR_OUTPUT_FILE stderr

/* Define instruction lookup item structure */
struct instruction_lookup_item {
	char *name;
	instruction value;
};

/* Define instructions lookup table */
static struct instruction_lookup_item instructions_lookup_table[] = {
		{"string", STRING_INST},
		{"data",   DATA_INST},
		{"entry",  ENTRY_INST},
		{"extern", EXTERN_INST},
		{NULL, NONE_INST}
};


/* Adds an extension to a file name */
char* add_extension(char *original_file_name, char *extension ){
    /* Calculate the length of the new string (original file name + extension + null terminator) */
    size_t new_length = strlen(original_file_name) + strlen(extension) + 1;

    /* Allocate memory for the new string */
    char *new_file_name = (char*)malloc(new_length * sizeof(char));

    /*  Check if memory allocation was successful */
    if(new_file_name  == NULL){
        printf("Memory allocation failed.");
        return NULL;
    }
    
    /* Copy the original file name to the new string */
    strcpy(new_file_name, original_file_name);
    
    /* Concatenate the extension to the new string */
    strcat(new_file_name, extension);

    /* Return the new string with the added extension */
    return new_file_name;
}

/* Skips whitespace characters in a string starting from the given index */
int skip_spaces(char *string, int index){
    /* Iterate through the string starting from the given index */
    while(string[index] && (string[index] == ' ' || string[index] == '\t')){
        index++;    /* Move to the next character if it's a whitespace character */
    }

    /* Return the index of the first non-whitespace character encountered */
    return index;
}

/* Checks if a string represents a valid integer */
bool check_if_digit(char *string) {
    int index = 0;

    /* Skip leading positive or negative sign if present */
    if (string[0] == '-' || string[0] == '+') { 
        string++;
    }

    /* Iterate through the string to check if all characters are digits */
    while (string[index] != '\0') {   
        if (!isdigit(string[index])) {
            return FALSE;   /* Return false if any character is not a digit */
        }
        index++;
    }

    /* If no digits found or only the sign was present, return false */
    if (index == 0) {  
        return FALSE;
    }

    /* Return true if all characters are digits */
    return TRUE;
}

/* Checks if a string contains only alphanumeric characters. */
bool chec_all_str_alpha(char *string){
    int index;

    for (index = 0; string[index]; index++){
        if (!isalpha(string[index]) && !isdigit(string[index])){
            return FALSE;   /* Return false if any character is not alphanumeric */
        }
    }
    return TRUE; /* Return true if all characters are alphanumeric */
}

/* Checks if a word is a special word */
bool is_special_word(char *word){
    int opcode;

    /* Check if the word is an opcode */
    get_opcode(word, &opcode);
    if (opcode != NONE_OP) {
        return TRUE;
    }

    /* Check if the word is a register name */
    if (get_register_by_name(word) != NONE_REG) {
        return TRUE;
    }

    /* Check if the word is an instruction name */
    if (find_instruction_by_name(word) != NONE_INST) {
        return TRUE;
    }

    /* The word is not a special word */
    return FALSE;
}


/* Checks if a label name is valid. */
bool check_label_name(char *label_name) {
    
    /* Check if label_name is not an empty string */
    if (!label_name[0]) {
        return FALSE;
    }
    
    /* Check if length is within the allowed limit */
    if (strlen(label_name) > 32) {
        return FALSE;
    }
    
    /* Check if the first character is alphabetic */
    if (!isalpha(label_name[0])) {
        return FALSE;
    }
    
    /* Check if all subsequent characters are alphabetic */
    if (!chec_all_str_alpha(label_name + 1)) {
        return FALSE;
    }

    /* Check if the label_name is not a special word */
    if (is_special_word(label_name)) {
        return FALSE;
    }
    
    /* All checks passed, label name is valid */
    return TRUE;
}


/* Extracts a label from a line of code */
bool extract_label(line_info line, char *symbol){

    int index_line  = 0;    /* Index for iterating through the line content */
    int index_symbol  = 0;  /* Index for storing characters in the symbol array */

    /* Skip leading spaces in the line */
    index_line  = skip_spaces(line.content, index_line ); 

    /* Extract characters from the line until a colon (':') or the end of the line is encountered */
    while (line.content[index_line ] && line.content[index_line ] != ':' && line.content[index_line ] != EOF && index_line  <= MAX_LINE_LENGTH){
        symbol[index_symbol ] = line.content[index_line ];
        index_line++;
        index_symbol ++;
    }

    /* Null-terminate the symbol array */
    symbol[index_symbol ] = '\0'; 

    /* If a colon is encountered, check if the extracted label is valid */
    if(line.content[index_line ] == ':'){
        if(!check_label_name(symbol)){
            print_error(line, "Invalid label name - cannot be longer than 32 chars, may only start with letter be alphanumeric.");
            symbol[0] = '\0';   /* Set symbol array to an empty string */
            return TRUE;        /* Return true to indicate an error */
        }
        return FALSE;           /* Return false if label extraction is successful and valid */
    }

    /* Return false if no colon is encountered */
    symbol[0] = '\0';   /* Set symbol array to an empty string */
    return FALSE; 
}

/* Finds the instruction corresponding to the given name */
instruction find_instruction_by_name(char *name) {
	struct instruction_lookup_item *curr_item;
    
    /* terate through the instructions lookup table */
	for (curr_item = instructions_lookup_table; curr_item->name != NULL; curr_item++) {
        /* Compare the current instruction name with the given name */
		if (strcmp(curr_item->name, name) == 0) {
			return curr_item->value;    /* Return the instruction value if a match is found */
		}
	}
    /* Return NONE_INST if no instruction is found with the given name */
	return NONE_INST;
}

/* Prints an error message. */
bool print_error(line_info line, char *massage, ...) {
    bool result;
	va_list args; 

    /* Print error location (file name and line number) */
	fprintf(ERR_OUTPUT_FILE,"Error In %s:%ld: ", line.file_name, line.line_number);

    /* Print the formatted error message with additional arguments */
	va_start(args, massage);
	result = vfprintf(ERR_OUTPUT_FILE, massage, args);
	va_end(args);

    /* Print a newline character after the error message */
	fprintf(ERR_OUTPUT_FILE, "\n");
	return result;
}

/* Checks and processes a line for define definition */
bool check_mdefine(line_info line, table *symbol_table, char *symbol) {
    char label[MAX_LINE_LENGTH];    /* Buffer to store the label extracted from the line */
    char number[MAX_LINE_LENGTH];   /* Buffer to store the number extracted from the line */
    long long_number;               /* Variable to store the parsed number as a long integer*/
    long index_line  = 0;               /* Index for iterating through the characters in the line */
    long temp_index = 0;            /* Temporary index for storing characters in the label and number buffers */
    table_entry *item;              /* Pointer to a table_entry struct used for symbol table operations */
    
    /* Skip leading spaces in the line */
    index_line  = skip_spaces(line.content, index_line );   
    
    /* Check if the line starts with '.' */
    if (line.content[index_line ] == '.') {
        /* Check if the line contains ".define" */
        if (strncmp(".define", line.content, 7) == 0) {
            index_line  += 7;   /* Skip ".define" */
            index_line  = skip_spaces(line.content, index_line );   /* Skip spaces */

            temp_index = 0;
            
            /* Extract label name until space, '=', or newline */
            while (line.content[index_line ] && line.content[index_line ] != ' ' && line.content[index_line ] != '=' && line.content[index_line ] != '\n' && line.content[index_line ] != EOF && index_line  <= MAX_LINE_LENGTH){
                label[temp_index] = line.content[index_line ];
                symbol[temp_index] = line.content[index_line ];
                index_line ++;
                temp_index++;
            }
            label[temp_index] = '\0';
            symbol[temp_index] = '\0';

            /* Check if label name is provided */
            if (label[0] == '\0') {
                printf("You have to specify a label name for .define");
                return FALSE;
            }

            /* Validate label name */
            if (!check_label_name(label)) {
                printf("Iligal name");
                return FALSE;
            }

            index_line  = skip_spaces(line.content, index_line );   /* Skip spaces */
            /* Check if '=' is found */
            if (line.content[index_line ] != '=') {
                printf("Error: '=' not found in the input string.\n");
                return FALSE;
            }
            index_line ++;

            index_line  = skip_spaces(line.content, index_line );   /*  Skip spaces */
            temp_index = 0;

            /* Extract number until space, newline, or EOF */
            while (line.content[index_line ] && line.content[index_line ] != ' ' && line.content[index_line ] != '\n' && line.content[index_line ] != EOF && index_line  <= MAX_LINE_LENGTH){
                number[temp_index] = line.content[index_line ];
                index_line ++;
                temp_index++;
            }    
            number[temp_index] = '\0';

            /* Check if number is provided */
            if (number == NULL) {
                printf("Error: Number not found after '='.\n");
                return FALSE;
            }

            /* Validate number */
            if (!check_if_digit(number)) {
                printf("It isn't a digit");
                return FALSE;
            }
            index_line  = skip_spaces(line.content, index_line );   /*  Skip spaces */
            
            /* Check if unexpected characters exist after the number */
            if (line.content[index_line] && line.content[index_line] != '\n') {
                printf("Error: Unexpected characters after the number.\n");
                return FALSE;
            }

            /* Check if label already exists in the symbol table */
            item = find_by_types(*symbol_table, label, 1, MDEFINE_SYMBOL);
            if (item != NULL) {
                if (item != NULL) {
                printf("You cannot define a mdefine type more than once with the same name.");
                return FALSE;
                }
                else {
                    printf("You cannot define type define and instruction with the same name.");
                    return FALSE;
                }
            }

            /* Convert number to long */
            long_number = atoi(number);

            /* Add table item to symbol table */
            add_table_item(symbol_table, label, long_number, MDEFINE_SYMBOL);
            return TRUE;    /* Line processed successfully */   
        }
        return TRUE;    /* Line does not start with .define*/
    }
    return TRUE;    /* Line does not start with .*/
}

/* Frees memory allocated for the code image */
void free_code_image(machine_word **code_image, long total_instructions) {
    long index;

    /* Iterate through each index of the code image array */
    for (index = 0; index < total_instructions; index++) {
        machine_word *curr_word = code_image[index];
        if (curr_word != NULL) {
            /* Check if the current machine word is not NULL */
            if (curr_word->length > 0) {
                /* If the length is positive, it's a code word, free the code memory */
                free(curr_word->word.code);
            } else {
                /* If the length is zero or negative, it's a data word, free the data memory */
                free(curr_word->word.data);
            }
            free(curr_word);
            /* Set the corresponding index in the code image array to NULL */
            code_image[index] = NULL;
        }
    }
}




