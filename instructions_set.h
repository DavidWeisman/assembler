#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H
#include "global_variables.h"

/**
 * Returns the instruction from the index of the input line
 * @param line The input line string
 * @param index The index of the start of the looking
 * @return The instruction that was found, if not NONE_INST, and if there is erroe ERROR_INST
*/
instruction find_instruction_from_index(line_info line, int *index);

/**
 * Processes a .string instruction from the index of the input line
 * @param line The input line string
 * @param index_l The index of the start of the looking
 * @param data_img The current data image
 * @param dc The current data counter
 * @return True or false if succeeded or not 
*/
bool process_string_instruction(line_info line, int index_l, long *data_img, long *dc);

/**
 * Processes a data instruction from the index of the input line
 * @param line The input line string
 * @param index_l The Index of the start of the looking
 * @param data_img The corrent data image
 * @param dc The current data counter
 * @return True or false if succeeded or not
*/
bool process_data_instruction(line_info line, int index_l, long *data_img, long *dc, table symbol_table);
#endif