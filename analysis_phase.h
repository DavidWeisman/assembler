#ifndef _SECOND_PASS_H
#define _SECOND_PASS_H
#include "global_variables.h"
#include "data_tables.h"

/**
 *  Processes a line in the second pass of the assembly process.
 *
 * This function handles the processing of a line in the second pass of the assembly process.
 * It skips leading spaces, checks for comments or empty lines, and processes instructions or directives.
 * If the line contains a label, it updates the index to skip it.
 * If the line contains a directive (e.g., .entry), it processes it accordingly.
 * If the line contains a code instruction, it adds the instruction to the code image.
 *
 * @param line The line_info structure containing information about the current line.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param code_img Array to store machine words representing code instructions.
 * @param symbol_table Pointer to the symbol table containing defined symbols.
 * @return Returns TRUE if the line is successfully processed, otherwise returns FALSE.
 */
bool process_line_spass(line_info line, long *ic, machine_word **code_img, table *symbol_table);

/**
 *  Adds symbols to the code image.
 *
 * This function adds symbols to the code image based on the line content.
 * It extracts labels and operands from the line content and processes them.
 * If a label is found, it skips to the next token.
 * It then analyzes the operands and processes them separately.
 *
 * @param line The line_info structure containing information about the current line.
 * @param ic Pointer to the instruction counter to track the position in the code_img array.
 * @param code_img Array to store machine words representing code instructions.
 * @param symbol_table Pointer to the symbol table containing defined symbols.
 * @return Returns TRUE if the symbols are successfully added to the code image, otherwise returns FALSE.
 */
bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table);
#endif