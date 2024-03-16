#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H
#include "globals.h"
#include "table.h"

/** Processes a single line in the first pass
 * @param line The line we work on
 * @param IC A pointer to the current instrucion counter
 * @param DC A pointer to the current data counter
 * @param code_img The code image
 * @param data_img The data image
 * @param symbol_table The symbol table
 * @return True or false if the process succeeded or not
*/
bool process_line_fpass(line_info line, long *IC, long *DC, machine_word **code_img, long *data_img, table *symbol_table);
#endif