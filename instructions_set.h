#ifndef _INSTRUCTIONS_SET_H
#define _INSTRUCTIONS_SET_H
#include "global_variables.h"

/**
 * @brief Finds the instruction from the index within a line.
 *
 * This function extracts the instruction from the specified index within the content of a line.
 * It skips leading spaces, reads the instruction until it encounters a tab, space, or the end of the line,
 * then searches for the instruction by name. If found, it returns the instruction; otherwise, it prints
 * an error message and returns ERROR_INST.
 *
 * @param line The line_info structure containing information about the current line.
 * @param index_line Pointer to the index within the line content from where to start searching for the instruction.
 *                   This value is updated to reflect the index after the instruction.
 * @return Returns the instruction found from the specified index within the line, or ERROR_INST if not found.
 */
instruction find_instruction_from_index(line_info line, int *index_line);

/**
 * @brief Processes the string instruction within a line.
 *
 * This function extracts and processes the string instruction from the specified index within the content of a line.
 * It checks for the presence of opening and closing quotes, then extracts the string content between them and 
 * converts it into ASCII values, storing them in the data_img array. The resulting string is null-terminated.
 *
 * @param line The line_info structure containing information about the current line.
 * @param index_line The index within the line content from where to start processing the string instruction.
 *                   This value is updated to reflect the index after the string instruction.
 * @param data_img Array to store the ASCII values of the characters in the string.
 * @param dc Pointer to the data counter to track the position in the data_img array.
 * @return Returns TRUE if the string instruction is successfully processed, otherwise returns FALSE.
 */
bool process_string_instruction(line_info line, int index_line, long *data_img, long *dc);

/**
 * @brief Processes the .data instruction within a line.
 *
 * This function extracts and processes the .data instruction and its operands from the specified index within the content of a line.
 * It reads integer values separated by commas, converts them to long integers, and stores them in the data_img array.
 *
 * @param line The line_info structure containing information about the current line.
 * @param index_line The index within the line content from where to start processing the .data instruction.
 *                   This value is updated to reflect the index after the .data instruction.
 * @param data_img Array to store the long integer values parsed from the .data instruction.
 * @param dc Pointer to the data counter to track the position in the data_img array.
 * @param symbol_table The symbol table containing defined symbols.
 * @return Returns TRUE if the .data instruction is successfully processed, otherwise returns FALSE.
 */
bool process_data_instruction(line_info line, int index_line, long *data_img, long *dc, table symbol_table);

#endif