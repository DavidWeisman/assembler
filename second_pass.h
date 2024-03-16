/* Second pass line processing related functions */
#ifndef _SECOND_PASS_H
#define _SECOND_PASS_H
#include "globals.h"
#include "table.h"

/**
 * processes a single line in the second pass
 * @param line The line we work on
 * @param ic A pointer to instruction counter
 * @param code_img The code image
 * @param symbol_table The simbol table
 * @return True of false if the process succeeded or not
*/
bool process_line_spass(line_info line, long *ic, machine_word **code_img, table *symbol_table);

/**
 * Adds the symbol-dependent data words by the code line.
 * @param line The source code line
 * @param ic A pointer to the current instruction counter
 * @param code_img The code image
 * @param symbol_table The symbol table
 * @return Whether succeeded
 */
bool add_symbols_to_code(line_info line, long *ic, machine_word **code_img, table *symbol_table);
#endif