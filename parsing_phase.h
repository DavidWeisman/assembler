#ifndef _PARSING_PHASE_H
#define _PARSING_PHASE_H
#include "global_variables.h"
#include "data_tables.h"

/**
 * @brief Processes a single line of code during the first pass of assembly.
 *
 * This function processes a single line of code during the first pass of the assembly process.
 * It extracts the label, determines the instruction type, and processes the operands accordingly.
 * Additionally, it updates the instruction and data counters and adds entries to the symbol table.
 *
 * @param line The line_info structure containing information about the current line.
 * @param IC Pointer to the instruction counter to track the position in the code_img array.
 * @param DC Pointer to the data counter to track the position in the data_img array.
 * @param code_img Array to store machine words representing code instructions.
 * @param data_img Array to store long integer values representing data.
 * @param symbol_table Pointer to the symbol table containing defined symbols.
 * @return Returns TRUE if the line is successfully processed, otherwise returns FALSE.
 */
bool process_line_fpass(line_info line, long *IC, long *DC, machine_word **code_img, long *data_img, table *symbol_table);
#endif