#ifndef _CODE_H
#define _CODE_H
#include "table.h"
#include "globals.h"

/**
 * Returns the register value by the input name
 * @param *name The name of the register
 * @return The register value of the register that he found by the input name, otherwise returns NOONE_REG
*/
reg get_register_by_name(char *name);

/**
 * Puts into the opcode input pointer the opcode of the command name 
 * @param *name The name of the command
 * @param *opcpde_out The pointer where the function puts the opcode of the command
*/
void get_opcode(char *name, opcode *opcode_out);

/**
 * Returns the addressing type of an operand
 * @param operand The pinter to the string of the operand
 * @return The addressing type of an operand
*/
addressing_type get_addressing_type(char *operand);


/**
 * Checks and builds the code word by the opcode, operand count and operand strings
 * @param line The current line we work on
 * @param curr_opcode The current opcode
 * @param op_count The counter of the operands
 * @param operands An array of two cell of opinters to first and second operands
 * @return A pointer to the codeword, if there is an erroe return NULL
*/
code_word *get_code_word(line_info line, opcode curr_opcode, int op_count, char *operands[2]);

/**
 * Separates the operands from the given index, puts each of the operands into the destination array, and puts the number of the operands into the count argument
 * @param line The input line we work on
 * @param index_l The Index of the start of the looking
 * @param destination The array of at least a 2 cell that saves the strings of the extaracted operand strings
 * @param operand_count The counter of the operands
 * @param operation The current command stirng
 * @return True of false if the analyzing succeded or not
*/
bool analyze_operands(line_info line, int index_l, char **destination, int *operand_count, char *operation, table symbol_table);

/**
 * Builds a data word by the operand's addressing type, value and whether the symbol (if it is one) is external.
 * @param addressing The addressing type of the value
 * @param data The value
 * @param is_extern_symbol If the symbol is a label, and it's external
 * @param is_src_operand If the operand is a source operand
 * @return A pointer to the new data word 
*/
data_word *build_data_word(addressing_type addressing, long data, bool is_extern_symbol, bool is_dest_operand);
#endif