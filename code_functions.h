#ifndef _CODE_FUNCTIONS_H
#define _CODE_FUNCTIONS_H
#include "data_tables.h"
#include "global_variables.h"

/**
 * @brief Retrieves the opcode corresponding to the given instruction name.
 * 
 * This function searches for the instruction name in the lookup table and retrieves
 * the corresponding opcode. If the instruction name is found in the table, the opcode
 * is stored in the variable pointed to by opcode_out. If the instruction name is not
 * found, opcode_out is set to NONE_OP.
 * 
 * @param name The instruction name for which the opcode is to be retrieved.
 * @param opcode_out Pointer to the variable where the retrieved opcode will be stored.
 */
void get_opcode(char *name, opcode *opcode_out);

/**
 * @brief Retrieves the register index corresponding to the given register name.
 * 
 * This function checks if the given register name is in the format 'rX', where X is a digit
 * between 0 and 7. If the format matches, it retrieves the digit and returns it as the register
 * index. If the format does not match, or if the digit is out of the valid range, NONE_REG is returned.
 * 
 * @param name The register name to be checked.
 * @return Returns the register index if the name is in the correct format and within the valid range,
 *         otherwise returns NONE_REG.
 */
reg get_register_by_name(char *name);

/**
 * @brief Determines the addressing type of the given operand.
 * 
 * This function analyzes the operand to determine its addressing type. It supports
 * various addressing modes including register, immediate, direct, and indexed with
 * fixed addressing. It also validates label names and checks for presence of square
 * brackets for indexed addressing.
 * 
 * @param operand The operand to be analyzed.
 * @param symbol_table The symbol table used for label validation.
 * @return Returns the addressing type of the operand.
 */
addressing_type get_addressing_type(char *operand, table symbol_table);

/**
 * @brief Converts a defined label in the input string to its corresponding value.
 * 
 * This function searches for a defined label in the input string and replaces it
 * with its corresponding value from the symbol table. If the label is found and
 * it is of type MDEFINE_SYMBOL, the label in the string is replaced with its value.
 * If the label is used in an immediate addressing mode, the '#' prefix is added
 * before the value. The function then ensures proper termination of the modified string.
 * 
 * @param string The input string possibly containing a defined label.
 * @param symbol_table The symbol table used for label lookup.
 * @param is_immediate_addr Indicates if the defined label is used in immediate addressing mode.
 */
void convert_defind(char *string, table symbol_table, bool is_immediate_addr);

/**
 * @brief Analyzes operands in a line of assembly code.
 * 
 * This function parses the operands in a line of assembly code, separating them
 * and storing them in the destination array. It also checks for syntax errors
 * such as unexpected commas, missing operands, and multiple consecutive commas.
 * Additionally, it converts defined labels to their corresponding values.
 * 
 * @param line The line of assembly code to be analyzed.
 * @param line_index The index indicating the position in the line where operand parsing starts.
 * @param destination An array to store the parsed operands.
 * @param operand_count Pointer to an integer storing the number of operands found.
 * @param operation The operation being performed on the operands.
 * @param symbol_table The symbol table used for label conversion.
 * @return Returns true if operand analysis is successful, otherwise returns false.
 */
bool analyze_operands(line_info line, int line_index, char **destination, int *operand_count, char *operation, table symbol_table);

/**
 * @brief Validates operands based on the opcode and addressing modes.
 * 
 * This function validates the operands based on the opcode and addressing modes
 * specified in the assembly instruction. It checks if the number of operands is
 * correct for the given operation and validates each operand's addressing mode.
 * 
 * @param line The line of assembly code containing the instruction.
 * @param first_addresing The addressing mode of the first operand.
 * @param second_addressing The addressing mode of the second operand.
 * @param curr_opcode The opcode of the current instruction.
 * @param op_count The number of operands found in the instruction.
 * @return Returns true if the operands are valid for the given opcode, otherwise returns false.
 */
bool validate_operand_by_opcode(line_info line, addressing_type first_addresing, addressing_type second_addressing, opcode curr_opcode, int op_count);

/**
 * @brief Retrieves the code word for the given assembly instruction.
 * 
 * This function constructs the code word for the given assembly instruction,
 * including the opcode and addressing modes for operands. It validates the
 * operands based on the opcode and addressing modes before constructing
 * the code word.
 * 
 * @param line The line of assembly code containing the instruction.
 * @param curr_opcode The opcode of the current instruction.
 * @param op_count The number of operands in the instruction.
 * @param operands An array containing the operands of the instruction.
 * @param symbol_table The symbol table used for operand validation.
 * @return Returns a pointer to the constructed code word if successful, otherwise returns NULL.
 */
code_word *get_code_word(line_info line, opcode curr_opcode, int op_count, char *operands[2], table symbol_table);

/**
 *  Constructs a data word based on the given parameters.
 *
 * This function builds a data word according to the provided addressing mode, data value,
 * and flags indicating whether the symbol is external and whether it's a source operand.
 * It sets the appropriate values for the ARE field and the data field of the data word.
 *
 * @param addressing The addressing mode of the data.
 * @param data The data value to be stored in the data word.
 * @param is_extern_symbol Flag indicating whether the symbol is external.
 * @param is_src_operand Flag indicating whether the data is a source operand.
 * @return Returns a pointer to the constructed data word if successful, otherwise returns NULL.
 */
data_word *build_data_word(addressing_type addressing, long data, bool is_extern_symbol, bool is_dest_operand);

/**
 *  Constructs a data word using two data values as registers.
 *
 * This function constructs a data word by combining two data values as registers,
 * according to the given addressing mode and addressing method. It sets the appropriate
 * values for the ARE field and the data field of the data word.
 *
 * @param first_data The first data value (register number).
 * @param second_data The second data value (register number).
 * @return Returns a pointer to the constructed data word if successful, otherwise returns NULL.
 */
data_word *build_data_word_reg(long first_data, long second_data);


#endif