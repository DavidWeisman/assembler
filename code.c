#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code.h"
#include "utils.h"

/* Table element*/
struct command_element {
	char *name;
	opcode op;
};

/* Table element, opcode and command name*/
static struct command_element lookup_table[] = {
    {"mov", MOV_OP},
    {"cmp", CMP_OP},
    {"add", ADD_OP},
    {"sub", SUB_OP},
    {"not", NOT_OP},
    {"clr", CLR_OP},
    {"lea", LEA_OP},
    {"inc", INC_OP},
    {"dec", DEC_OP},
    {"jmp", JMP_OP},
    {"bne", BNE_OP},
    {"red", RED_OP},
    {"prn", PRN_OP},
    {"jsr", JSR_OP},
    {"rts", RTS_OP},
    {"hlt", HLT_OP},
    {NULL, NONE_OP}
};

/* Gets the opcode of the input element name*/
void get_opcode(char *name, opcode *opcode_out){
    struct command_element *element;
    *opcode_out = NONE_OP; /* No match */

    for (element = lookup_table; element->name != NULL; element++) {
		if (strcmp(element->name, name) == 0) {
			*opcode_out = element->op;
			return;
		}
	}
}

/* Returns the register by the input name*/
reg get_register_by_name(char *name) {
	if (name[0] == 'r' && isdigit(name[1]) && name[2] == '\0') {
		int digit = name[1] - '0'; /* Converts from char digit to a normal digit */
		if (digit >= 0 && digit <= 7){
            return digit;
        } 
	}
	return NONE_REG; /* No match */
}

addressing_type get_addressing_type(char *operand) {
    /* Variables of the Index fixed addressed check*/
    char label_name[31]; /* The place where the label name saved*/
    char *open_bracket; /* Pointer to the open bracket in the operand*/
    char *closeing_bracket; /* Pointer to the closeing bracket in the operand*/
    char *number; /* Pointer to the digit between the brackets*/
    int num_len; /* The length of the number*/

    /* If first char is 'r', second is number in range 0-7 and third is end of string, it's a register */
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0'){
        return REGISTER_ADDR;
    }

    /* If operand starts with # and a number right after that, it's immediately addressed */
    else if (operand[0] == '#' && check_if_digit(operand + 1)) {
        return IMMEDIATE_ADDR;
    }

    /* If operand is a valid label name, it's directly addressed */
    else if (check_label_name(operand)) {
        return DIRECT_ADDR;
    }
    /* If first string is a valid labbel name, then the next char is '[', then the next stirng is a valid digit, then if the next char is '[', and if the last char is end of string, it's Index fixed addressed*/
    else if (operand[0] == '\0') {
        /* Check if in the operand there is '['*/
        open_bracket = strchr(operand, '[');
        if (!open_bracket) {
            return NONE_ADDR;
        }

        /* Copys the lable name*/
        strncpy(label_name, operand, open_bracket - operand);
        label_name[open_bracket - operand] = '\0';

        /* Checks if the name is valid*/
        if (!check_label_name(label_name)) {
            return NONE_ADDR;
        }

        /* Checks if in the operand there is '[' */
        closeing_bracket = strchr(operand, ']');
        if (!closeing_bracket) {
            return NONE_ADDR;
        }

        /* Checks if the last char is end of string*/
        if (closeing_bracket[1] != '\0') {
            return NONE_ADDR;
        }

        num_len = closeing_bracket - open_bracket - 1; /* Gets the length of the number */

        number = (char *)malloc((num_len + 1) * sizeof(char));
        strncpy(number, open_bracket + 1, num_len); /* Copys the number*/

        number[num_len] = '\0'; /* End of string*/
        /*Checks if it's a valid number*/
        if (!check_if_digit(number)) {
            return NONE_ADDR;
        }
        return INDEX_FIXED_ADDR;
    }
    else {
        return NONE_ADDR; /* If the input is empty */
    }
}

/* Analyze the given operands*/
bool analyze_operands(line_info line, int index_l, char **destination, int *operand_count, char *operation) {
    int index_o = 0;
    *operand_count = 0;
    destination[0] = NULL;
    destination[1] = NULL;

    index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/
    if (line.content[index_l] == ',') {
        printf("Unexpected comma after command.");
        return FALSE; /* an error occurred */
    }

    /* Until noy too many operands (max of 2) and it's not the end of the line */
    for (*operand_count = 0; line.content[index_l] != EOF && line.content[index_l] != '\n' && line.content[index_l];) {
		if (*operand_count == 2) { /* =We already got 2 operands in, We're going ot get the third! */
			printf("Too many operands for operation");
			free(destination[0]);
			free(destination[1]);
			return FALSE;   /* an error occurred */
		}

        /* Allocate memory to save the operand */
        destination[*operand_count] = malloc(MAX_LINE_LENGTH);
        if (destination[*operand_count] == NULL) {
            printf("Memory not allocated");
            return FALSE;
        }

        /* as long we're still on same operand */
        while (line.content[index_l] && line.content[index_l] != '\t' && line.content[index_l] != ' ' && line.content[index_l] != '\n' && line.content[index_l] != EOF && line.content[index_l] != ',') {
            destination[*operand_count][index_o] = line.content[index_l];
            index_l++;
            index_o++;
        }
        destination[*operand_count][index_o] = '\0';
        (*operand_count)++;  /* We've just saved another operand! */

        index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/
        
        if (line.content[index_l] == '\n' || line.content[index_l] == EOF || !line.content[index_l]) {
            break;
        }
        else if (line.content[index_l] != ',') {
            /* After operand & after white chars there's something that isn't ',' or end of line.. */
            printf("Expecting ',' between operands");

            /* Release operands dynamically allocated memory */
            free(destination[0]);
            if (*operand_count > 1) {
                free(destination[1]);
            }
            return FALSE;
        }
        index_l++;

        index_l = skip_spaces(line.content, index_l); /*Skips all the spaces or tabs*/

        /* if there was just a comma, then (optionally) white char(s) and then end of line */
        if (line.content[index_l] == '\n' || line.content[index_l] == EOF || !line.content[index_l]) {
            printf("Missing operand after comma.");
        }
        else if (line.content[index_l] == ',') {
            printf("Multiple consecutive commas.");
        }
        else continue;  /* No errors, continue */
        {
            free(destination[0]);
            if (*operand_count > 1) {
				free(destination[1]);
			}
			return FALSE;
        }
    }
    return TRUE;
}


/*
code_word *get_code_word(line_info line, opcode curr_opcode, int op_count, char *operands[2]) {
    code_word *codeword;

}
*/
