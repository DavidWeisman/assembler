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

bool analyze_operands(line_info line, int index_l, char **destination, int *operand_count, char *operation) {
    int index_o = 0;
    *operand_count = 0;
    destination[0] = NULL;
    destination[1] = NULL;
    index_l = skip_spaces(line.content, index_l);
    if (line.content[index_l] == ',') {
        printf("Unexpected comma after command.");
        return FALSE;
    }

    for (*operand_count = 0; line.content[index_l] != EOF && line.content[index_l] != '\n' && line.content[index_l];) {
		if (*operand_count == 2) {
			printf("Too many operands for operation");
			free(destination[0]);
			free(destination[1]);
			return FALSE; 
		}

        destination[*operand_count] = malloc(MAX_LINE_LENGTH);
        if (destination[*operand_count] == NULL) {
            printf("Memory not allocated");
            return FALSE;
        }

        while (check_char(line.content[index_l])) {
            destination[*operand_count][index_o] = line.content[index_l];
        }
        destination[*operand_count][index_o] = '\0';
        (*operand_count)++;

        index_l = skip_spaces(line.content, index_l);
        
        if (line.content[index_l] == '\n' || line.content[index_l] == EOF || !line.content[index_l]) {
            break;
        }
        else if (line.content[index_l] != ',') {
            printf("Expecting ',' between operands");
            free(destination[0]);
            if (*operand_count > 1) {
                free(destination[1]);
            }
            return FALSE;
        }
        index_l++;
        index_l = skip_spaces(line.content, index_l);
        if (line.content[index_l] == '\n' || line.content[index_l] == EOF || !line.content[index_l]) {
            printf("Missing operand after comma.");
        }
        else if (line.content[index_l] == ',') {
            printf("Multiple consecutive commas.");
        }
        else continue;
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
