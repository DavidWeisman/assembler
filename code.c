#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code.h"
#include "utils.h"




/**
 * Checks the operands addressing types, prints error if needed
 * @param op_addressing The current addressing of the operand
 * @param number_of_addrs Number of the valids addresing for the operand
 * @param valid_addr1 The first valid addresing type for the operand 
 * @param valid_addr2 The second valid addresing type for the operand 
 * @param valid_addr3 The third  valid addresing type for the operand 
 * @param valid_addr4 The fourth  valid addresing type for the operand 
 * @return True or false depens if the addresing type is valid or not
*/
static bool validate_op_addr(addressing_type op_addressing, int number_of_addrs, addressing_type valid_addr1, addressing_type valid_addr2, addressing_type valid_addr3, addressing_type valid_addr4);



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
    else if (strchr(operand, '[') && strchr(operand, ']')) {
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

void convert_defind(char *string, table symbol_table){
    int index_s = 0;
    int index_d = 0;
    char temp[MAX_LINE_LENGTH];
    table_entry *item;
    index_s = skip_spaces(string, index_s);
    index_s++;
    while (string[index_s]) {
        temp[index_d] = string[index_s];
        index_d++;
        index_s++;
    }
    temp[index_d] = '\0';
   
    
    item = find_by_types(symbol_table, temp);
    if (item && item->type == MDEFINE_SYMBOL) {
        
        string[0] = '#';
        
        sprintf(string + 1, "%ld",  item->value);
        
        index_s = 1;
        if (string[index_s] == '-' || string[index_s] == '+') {
            index_s++;
        }
        while (isdigit(string[index_s])){
            index_s++;
        }
        string[index_s] = '\0';
        
    }
    
}

/* Analyze the given operands*/
bool analyze_operands(line_info line, int index_l, char **destination, int *operand_count, char *operation, table symbol_table) {
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
        index_o = 0; 
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
    
    if (*operand_count == 1) {
        if (destination[0][0] == '#'){
            
            convert_defind(destination[0], symbol_table);
        }
        
    }
    if (*operand_count == 2) {
        if (destination[1][0] == '#'){
            convert_defind(destination[1], symbol_table);
        }
    }
     
    return TRUE;
}

/**
 * Checks the operands's addressing types by the opcode of the operation
 * @param line The current line we word on
 * @param first_addresing The addresing of the first operand
 * @param second_addressing The addresing of the second operand
 * @param curr_opcode The opcode of the currnt operation 
 * @param op_count The cunter of the operands
 * @return True or false if the addressing are valid or not
*/
bool validate_operand_by_opcode(line_info line, addressing_type first_addresing, addressing_type second_addressing, opcode curr_opcode, int op_count) {
    if (curr_opcode == LEA_OP || (curr_opcode >= MOV_OP && curr_opcode <= SUB_OP)) {
        /* Only two operands required */
        if (op_count != 2) {
            printf( "Operation requires 2 operands");
            return FALSE;
        }

        /* Checks if the addressing are valid */
        if (curr_opcode == MOV_OP || curr_opcode == ADD_OP || curr_opcode == SUB_OP) {
            if (!validate_op_addr(first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR)) {
                return FALSE;
            }
            return validate_op_addr(second_addressing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
        else if (curr_opcode == CMP_OP) {
            if (!validate_op_addr(first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR)) {
                return FALSE;
            }
            return  validate_op_addr(second_addressing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR);
        }
        else if (curr_opcode == LEA_OP) {
            if (!validate_op_addr(first_addresing, 2, DIRECT_ADDR, INDEX_FIXED_ADDR, NONE_ADDR, NONE_ADDR)) {
                return FALSE;
            }
            return validate_op_addr(second_addressing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
    }
    else if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || (curr_opcode >= INC_OP && curr_opcode <= JSR_OP)) {
        if (op_count != 1) {
            /* Only one operand required */
		    if (op_count < 1) {
                printf("Operation requires 1 operand ");
			    return FALSE;
            } 
		}

        /* Checks if the addressing are valid */
        if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || curr_opcode == INC_OP || curr_opcode == DEC_OP || curr_opcode == RED_OP) {
            return validate_op_addr(first_addresing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
        else if (curr_opcode == JMP_OP || curr_opcode == BNE_OP || curr_opcode == JSR_OP) {
            return validate_op_addr(first_addresing, 2, DIRECT_ADDR, REGISTER_ADDR, NONE_ADDR, NONE_ADDR);
        }
        else { /* It's PRN*/
            return validate_op_addr(first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR);
        }
    }
    else if (curr_opcode == RTS_OP || curr_opcode == HLT_OP) {
        /* Zero oprerands exactly */
        if (op_count > 0) {
			printf("Operation requires no operands");
			return FALSE;
		}
    }
    return TRUE;
}

/* Gets the code word*/
code_word *get_code_word(line_info line, opcode curr_opcode, int op_count, char *operands[2]) {
    code_word *codeword;
   
    /* Gets the addressing types */
    addressing_type first_addr = op_count >= 1 ? get_addressing_type(operands[0]) : NONE_ADDR;
    addressing_type second_addr = op_count == 2 ? get_addressing_type(operands[1]) : NONE_ADDR;

  
    if (!validate_operand_by_opcode(line, first_addr, second_addr, curr_opcode, op_count)) {
        return NULL;
    }
   
    /* Allocate memory for the codeword*/
    codeword = (code_word *)malloc(sizeof(code_word));
    if (codeword == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    /* Sets the opcode and the ARE of the codeword*/
    codeword->opcode = curr_opcode;
    codeword->ARE = 0;

    /* Default values for the addressign */
    codeword->dest_addressing = 0;
    codeword->src_addressing = 0;
   
    /* Sets the addressing types */
    if (curr_opcode == LEA_OP || (curr_opcode >= MOV_OP && curr_opcode <= SUB_OP)) {
        codeword->src_addressing = first_addr;
        codeword->dest_addressing = second_addr;
    }
    else if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || (curr_opcode >= INC_OP && curr_opcode <= JSR_OP)) {
        codeword->dest_addressing = first_addr;
    }
    return codeword;
}

static bool validate_op_addr(addressing_type op_addressing, int number_of_addrs, addressing_type valid_addr1, addressing_type valid_addr2, addressing_type valid_addr3, addressing_type valid_addr4) {
    bool is_valid = FALSE; 
    int index;
    addressing_type op_valids[4]; /* Array of the valid addresing*/
    op_valids[0] = valid_addr1;
    op_valids[1] = valid_addr2;
    op_valids[2] = valid_addr3;
    op_valids[3] = valid_addr4;

    /* Checks if the operand addressing type has match to any of the valid ones */
    for (index = 0; index < number_of_addrs; index++) {
        if (op_valids[index] == op_addressing) {
            is_valid = TRUE;
        }
    }
    if (!is_valid) {
        printf("Invalid addressing mode for first operand.\n");
        return FALSE;
    }
    return TRUE;
}


data_word *build_data_word(addressing_type addressing, long data, bool is_extern_symbol, bool is_src_operand) {
    signed long mask; /* For bitwise operations for data conversion */
    unsigned long ARE = 0;
    unsigned long mask_un;

    data_word *dataword = (data_word *)malloc(sizeof(data_word));
    if (dataword == NULL) {
        printf("Memory allocation failed");
        return NULL;
    }

    if (addressing == DIRECT_ADDR) {
        if(is_extern_symbol) {
            ARE = 1;
        }
        else {
            ARE = 2;
        }
    }
    if (addressing == REGISTER_ADDR) {
        if (is_src_operand) {
            data <<= 3;
        }
    }
    dataword->ARE = ARE; /* Set ARE field value */
    /* Now all left is to encode the data */
	mask = -1;
	mask_un = mask; /* both hold 11...11 */
	mask_un >>= 20; /* Now mask_un holds 0..001111....111, 11 zeros and 21 ones */
	dataword->data = mask_un & data; /* Now just the 21 lsb bits area left and assigned to data field. */
	return dataword;
}