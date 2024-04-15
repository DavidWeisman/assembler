#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "code_functions.h"
#include "utility_functions.h"

/**
 * @brief Validates the addressing mode for an operand.
 * 
 * This function validates whether the addressing mode for an operand is one of the valid
 * addressing modes specified. It compares the provided addressing mode with a list of valid
 * addressing modes and returns true if a match is found, indicating a valid addressing mode.
 * 
 * @param line The line of assembly code containing the instruction.
 * @param op_addressing The addressing mode to be validated.
 * @param number_of_addrs The number of valid addressing modes to check against.
 * @param valid_addr1 The first valid addressing mode.
 * @param valid_addr2 The second valid addressing mode.
 * @param valid_addr3 The third valid addressing mode.
 * @param valid_addr4 The fourth valid addressing mode.
 * @return Returns true if the addressing mode is valid, otherwise returns false.
 */
static bool validate_op_addr(line_info line, addressing_type op_addressing, int number_of_addrs, addressing_type valid_addr1, addressing_type valid_addr2, addressing_type valid_addr3, addressing_type valid_addr4);

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

/* Retrieves the opcode corresponding to the given instruction name */
void get_opcode(char *name, opcode *opcode_out){
    struct command_element *element;
    *opcode_out = NONE_OP;          /* Initialize opcode_out to NONE_OP by default */
    /* Iterate through the lookup table */
    for (element = lookup_table; element->name != NULL; element++) {
        /* Compare the current instruction name with the given name */
		if (strcmp(element->name, name) == 0) {
            /* If a match is found, store the corresponding opcode */
			*opcode_out = element->op;
			return;
		}
	}
}

/* Retrieves the register index corresponding to the given register name */
reg get_register_by_name(char *name) {
    /* Check if the register name is in the format 'rX' where X is a digit between 0 and 7 */
	if (name[0] == 'r' && isdigit(name[1]) && name[2] == '\0') {
		int digit = name[1] - '0';  /* Convert the character representing the digit to an integer */
        /* Check if the digit is within the valid range (0 to 7) */
		if (digit >= 0 && digit <= 7){
            return digit;   /* Return the digit as the register index */
        } 
	}
	return NONE_REG;    /* Return NONE_REG if the format is incorrect or the digit is out of range */
}

/* Determines the addressing type of the given operand */
addressing_type get_addressing_type(char *operand, table symbol_table) {
    char label_name[31];        /* Buffer to store label name extracted from operand */
    char *open_bracket;         /* Pointer to the opening square bracket '[' in the operand */
    char *closing_bracket;      /* Pointer to the closing square bracket ']' in the operand */
    char *number;               /* Buffer to store the index number in indexed addressing */
    int index_length;           /* Length of the index number in indexed addressing */
    
    /* Check if operand represents a register (r0-r7) */
    if (operand[0] == 'r' && operand[1] >= '0' && operand[1] <= '7' && operand[2] == '\0'){
        return REGISTER_ADDR;
    }
    
    /* Check if operand is an immediate value (#number) */
    else if (operand[0] == '#' && check_if_digit(operand + 1)) {
        return IMMEDIATE_ADDR;
    }

    /* Check if operand is a valid label name */
    else if (check_label_name(operand)) {
        return DIRECT_ADDR;
    }
    /* Check if operand represents indexed addressing with fixed addressing */
    else if (strchr(operand, '[') && strchr(operand, ']')) {
        open_bracket = strchr(operand, '[');
        if (!open_bracket) {
            return NONE_ADDR;
        }
        /* Extract label name before '[' */
        strncpy(label_name, operand, open_bracket - operand);
        label_name[open_bracket - operand] = '\0';

        if (!check_label_name(label_name)) {
            return NONE_ADDR;
        }

        closing_bracket = strchr(operand, ']');
        if (!closing_bracket) {
            return NONE_ADDR;
        }

        /* Check if there's anything after ']' */
        if (closing_bracket[1] != '\0') {
            return NONE_ADDR;
        }
        /* Calculate the length of the index number */
        index_length = closing_bracket - open_bracket - 1; 
        /* Allocate memory for index number */
        number = (char *)malloc((index_length + 1) * sizeof(char));
        strncpy(number, open_bracket + 1, index_length); 
        number[index_length] = '\0'; 
        /* Check if index number is a digit or a label name */
        if (!check_if_digit(number)) {
            if (find_by_types(symbol_table, number, 1, MDEFINE_SYMBOL) == NULL) {
                return NONE_ADDR;
            }
        }
        return INDEX_FIXED_ADDR;
    }
    else {
        return NONE_ADDR;   /* Operand does not match any addressing mode */
    }
}

/* Converts a defined label in the input string to its corresponding value */
void convert_defind(char *string, table symbol_table, bool is_immediate_addr){
    int string_index = 0;                   /* Index for string traversal */
    int copy_index = 0;                     /* Index for constructing the extracted label string */
    char extracted_label[MAX_LINE_LENGTH];  /* Buffer to store the label extracted from the string */
    table_entry *symbol_entry;              /* Pointer to the symbol table entry */

    string_index = skip_spaces(string, string_index); /* Skip leading spaces in the string */

    /* If the string starts with '#', move to the next character */
    if (string[0] == '#') {
        string_index++;
    }
    
    /* Extract the label from the string */
    while (string[string_index]) {
        extracted_label[copy_index] = string[string_index];
        copy_index++;
        string_index++;
    }
    extracted_label[copy_index] = '\0'; /* Ensure proper termination of extracted_label */
    
    /* Find the symbol table entry corresponding to the extracted label */
    symbol_entry = find_by_types(symbol_table, extracted_label, 1, MDEFINE_SYMBOL);

    /* If the label is found and it is of type MDEFINE_SYMBOL */
    if (symbol_entry != NULL) {
        string_index = 0; /*  Reset string_index to start modifying the string */

        /* If the label is used in immediate addressing mode, add '#' prefix before the value */
        if (is_immediate_addr) {
            string[0] = '#';
            string_index = 1;
            /* Replace the label in the string with its value */
            sprintf(string + 1, "%ld",  symbol_entry->value);
        }
        else { 
            /* Replace the label in the string with its value */
            sprintf(string, "%ld",  symbol_entry->value);
        }
        
        /* If the value is negative or positive, skip the sign character */
        if (string[string_index] == '-' || string[string_index] == '+') {
            string_index++;
        }
        /* Move string_index to the end of the replaced value */
        while (isdigit(string[string_index])){
            string_index++;
        }
        string[string_index] = '\0';    /* Ensure proper termination of the modified string */
    }
}

/* Analyzes operands in a line of assembly code */
bool analyze_operands(line_info line, int line_index, char **destination, int *operand_count, char *operation, table symbol_table) {
    int operand_index  = 0;        /* Index for operand string construction */  
    *operand_count = 0;     /* Initialize operand count */
    destination[0] = NULL;  /* Initialize destination array */
    destination[1] = NULL;  /* Initialize destination array */
     
     /*  Skip leading spaces in the line */
    line_index = skip_spaces(line.content, line_index); 

    /* Check for unexpected comma after the command */
    if (line.content[line_index] == ',') {
        print_error(line, "Unexpected comma after command.");
        return FALSE; 
    }

    /* Loop through the line to parse operands */
    for (*operand_count = 0; line.content[line_index] != EOF && line.content[line_index] != '\n' && line.content[line_index];) {
        /* Check if there are too many operands for the operation */
		if (*operand_count == 2) {
			print_error(line, "Too many operands for operation (got >%d)", *operand_count);
			free(destination[0]);
			free(destination[1]);
			return FALSE;   
		}

        /* Allocate memory for the operand string */
        destination[*operand_count] = malloc(MAX_LINE_LENGTH);
        if (destination[*operand_count] == NULL) {
            printf("Memory not allocated");
            return FALSE;
        }
        operand_index = 0; /* Reset index for operand string construction */
        
        /* Parse the operand until encountering whitespace, newline, EOF, or comma */
        while (line.content[line_index] && line.content[line_index] != '\t' && line.content[line_index] != ' ' && line.content[line_index] != '\n' && line.content[line_index] != EOF && line.content[line_index] != ',') {
            destination[*operand_count][operand_index] = line.content[line_index];
            line_index++;
            operand_index++;
        }
        destination[*operand_count][operand_index] = '\0';    /* Ensure proper termination of operand string */
        (*operand_count)++;                             /* Increment operand count */

        /* Skip spaces in the line */
        line_index = skip_spaces(line.content, line_index); 
        
        /* Check for end of line or end of file */
        if (line.content[line_index] == '\n' || line.content[line_index] == EOF || !line.content[line_index]) {
            break;
        }
        /* Check for a comma separating operands */
        else if (line.content[line_index] != ',') {
            print_error(line, "Expecting ',' between operands");

            free(destination[0]);
            if (*operand_count > 1) {
                free(destination[1]);
            }
            return FALSE;
        }
        line_index++;  /* Move past the comma */
        
        /* Skip spaces in the line */
        line_index = skip_spaces(line.content, line_index); 

        /* Check for syntax errors related to commas or missing operands */
        if (line.content[line_index] == '\n' || line.content[line_index] == EOF || !line.content[line_index]) {
            print_error(line, "Missing operand after comma.");
        }
        else if (line.content[line_index] == ',') {
            print_error(line, "Multiple consecutive commas.");
        }
        else continue;  /* Continue parsing operands */

        /* Free memory and return false if any syntax errors are encountered */
        {
            free(destination[0]);
            if (*operand_count > 1) {
				free(destination[1]);
			}
			return FALSE;
        }
    }
    /* Convert defined labels to their corresponding values if present */
    if (*operand_count == 1) {
        if (destination[0][0] == '#'){
            
            convert_defind(destination[0], symbol_table, TRUE);
        }
        
    }
    if (*operand_count == 2) {
        if (destination[1][0] == '#'){
            convert_defind(destination[1], symbol_table, TRUE);
        }
    }
     
    return TRUE;    /* Operand analysis successful */
}

/* Validates operands based on the opcode and addressing modes */
bool validate_operand_by_opcode(line_info line, addressing_type first_addresing, addressing_type second_addressing, opcode curr_opcode, int op_count) {
    /* Validate operands based on the opcode */
    if (curr_opcode == LEA_OP || (curr_opcode >= MOV_OP && curr_opcode <= SUB_OP)) {
        /* Operations requiring two operands */
        if (op_count != 2) {
            print_error(line, "Operation requires 2 operands (got %d)", op_count);
            return FALSE;
        }

        /* Validating addressing modes for specific operations */
        if (curr_opcode == MOV_OP || curr_opcode == ADD_OP || curr_opcode == SUB_OP) {
            if (!validate_op_addr(line, first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR)) {
                return FALSE;
            }
            return validate_op_addr(line, second_addressing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
        else if (curr_opcode == CMP_OP) {
            if (!validate_op_addr(line, first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR)) {
                return FALSE;
            }
            return  validate_op_addr(line, second_addressing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR);
        }
        else if (curr_opcode == LEA_OP) {
            if (!validate_op_addr(line, first_addresing, 2, DIRECT_ADDR, INDEX_FIXED_ADDR, NONE_ADDR, NONE_ADDR)) {
                return FALSE;
            }
            return validate_op_addr(line, second_addressing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
    }
    else if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || (curr_opcode >= INC_OP && curr_opcode <= JSR_OP)) {
        /* Operations requiring one operand */
        if (op_count != 1) {
		    if (op_count < 1) {
                print_error(line, "Operation requires 1 operand (got %d)", op_count);
			    return FALSE;
            } 
		}

        /* Validating addressing modes for specific operations */
        if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || curr_opcode == INC_OP || curr_opcode == DEC_OP || curr_opcode == RED_OP) {
            return validate_op_addr(line, first_addresing, 3, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR, NONE_ADDR);
        }
        else if (curr_opcode == JMP_OP || curr_opcode == BNE_OP || curr_opcode == JSR_OP) {
            return validate_op_addr(line, first_addresing, 2, DIRECT_ADDR, REGISTER_ADDR, NONE_ADDR, NONE_ADDR);
        }
        else { 
            return validate_op_addr(line, first_addresing, 4, IMMEDIATE_ADDR, DIRECT_ADDR, INDEX_FIXED_ADDR, REGISTER_ADDR);
        }
    }
    else if (curr_opcode == RTS_OP || curr_opcode == HLT_OP) {
        /*  Operations requiring no operands */
        if (op_count > 0) {
			print_error(line, "Operation requires no operands (got %d)", op_count);
			return FALSE;
		}
    }
    return TRUE;    /* Operands are valid for the given opcode */
}

/* Retrieves the code word for the given assembly instruction */
code_word *get_code_word(line_info line, opcode curr_opcode, int op_count, char *operands[2], table symbol_table) {
    code_word *codeword;    /* Pointer to the constructed code word */
    
    /* Determine addressing types for operands */
    addressing_type first_addr = op_count >= 1 ? get_addressing_type(operands[0], symbol_table) : NONE_ADDR;
    addressing_type second_addr = op_count == 2 ? get_addressing_type(operands[1], symbol_table) : NONE_ADDR;

    /* Validate operands based on the opcode and addressing modes */
    if (!validate_operand_by_opcode(line, first_addr, second_addr, curr_opcode, op_count)) {
        return NULL;
    }
    
    /* Allocate memory for the code word */
    codeword = (code_word *)malloc(sizeof(code_word));
    if (codeword == NULL) {
        print_error(line, "Memory allocation failed\n");  
        return NULL;
    }

    /*  Initialize fields of the code word */
    codeword->opcode = curr_opcode;
    codeword->ARE = 0;
    codeword->dest_addressing = 0;
    codeword->src_addressing = 0;
    
    /* Set addressing modes for source and destination operands */
    if (curr_opcode == LEA_OP || (curr_opcode >= MOV_OP && curr_opcode <= SUB_OP)) {
        codeword->src_addressing = first_addr;
        codeword->dest_addressing = second_addr;
    }
    else if (curr_opcode == NOT_OP || curr_opcode == CLR_OP || (curr_opcode >= INC_OP && curr_opcode <= JSR_OP)) {
        codeword->dest_addressing = first_addr;
    }
    return codeword;    /* Return pointer to the constructed code word */
}

/* Validates the addressing mode for an operand */
static bool validate_op_addr(line_info line, addressing_type op_addressing, int number_of_addrs, addressing_type valid_addr1, addressing_type valid_addr2, addressing_type valid_addr3, addressing_type valid_addr4) {
    bool is_valid = FALSE;          /* Flag indicating whether the addressing mode is valid */
    int index;
    addressing_type op_valids[4];   /*  Array to store valid addressing modes */

    /* Assign valid addressing modes to the array */
    op_valids[0] = valid_addr1;
    op_valids[1] = valid_addr2;
    op_valids[2] = valid_addr3;
    op_valids[3] = valid_addr4;

    /* Check if the provided addressing mode matches any of the valid addressing modes */
    for (index = 0; index < number_of_addrs; index++) {
        if (op_valids[index] == op_addressing) {
            is_valid = TRUE;    /* Set flag to true if a match is found */
        }
    }
    /* If the addressing mode is not valid, print an error message */
    if (!is_valid) {
        print_error(line, "Invalid addressing mode for operand.");
        return FALSE;
    }
    return TRUE;    /* Return true if addressing mode is valid */
}

/* Constructs a data word based on the given parameters. */
data_word *build_data_word(addressing_type addressing, long data, bool is_extern_symbol, bool is_src_operand) {
    signed long mask;       /* Mask for extracting data */
    unsigned long ARE = 0;  /* Addressing Reallocation Entry value */
    unsigned long mask_un;  /* Unsigned version of the mask */

    /* Allocate memory for the data word */
    data_word *dataword = (data_word *)malloc(sizeof(data_word));
    if (dataword == NULL) {
        printf("Memory allocation failed");
        return NULL;
    }

    /* Set ARE field based on addressing mode and external symbol flag */
    if (addressing == DIRECT_ADDR) {
        if(is_extern_symbol) {
            ARE = 1;    /* External addressing mode */
        }
        else {
            ARE = 2;    /* Relative addressing mode */
        }
    }
    /* Adjust data value for source operand in register addressing mode */
    if (addressing == REGISTER_ADDR) {
        if (is_src_operand) {
            data <<= 3;     /* Shift data value by 3 bits for source operand */
        }
    }
     
     /*  Calculate data field using masking */
	mask = -1;
	mask_un = mask;
	mask_un >>= 20;                     /* Shift mask to extract 20 bits */
    dataword->ARE = ARE;                /* Set ARE field of the data word */
	dataword->data = mask_un & data;    /* Set data field of the data word */ 
	return dataword;                    /* Return pointer to the constructed data word */
}

/* Constructs a data word using two data values as registers */
data_word *build_data_word_reg(long first_data, long second_data) {
    signed long mask; /* Mask for extracting data */
    unsigned long ARE = 0; /* Addressing Reallocation Entry value */
    unsigned long mask_un; /* Unsigned version of the mask */
    long third_data; /* Combined data value */

    /* Allocate memory for the data word */
    data_word *dataword = (data_word *)malloc(sizeof(data_word));
    if (dataword == NULL) {
        printf("Memory allocation failed");
        return NULL;
    }

    /* Combine two data values as registers */
    third_data = (first_data << 3) | second_data;

    /* Set ARE field */
    dataword->ARE = ARE; 

    /* Calculate data field using masking */
    mask = -1;
    mask_un = mask; 
    mask_un >>= 20; /* Shift mask to extract 20 bits */
    dataword->data = mask_un & third_data; /* Set data field of the data word */

    return dataword; /* Return pointer to the constructed data word */
}
