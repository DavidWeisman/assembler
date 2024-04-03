#ifndef _WRITEFILES_H
#define _WRITEFILES_H
#include "global_variables.h"
#include "data_tables.h"

/* Writes output files including machine code, external references, and entry symbols.
 *
 * This function writes output files containing machine code, external references, and entry symbols.
 * It first filters the symbol table to extract external references and entry symbols. Then, it calls
 * helper functions to write machine code, external references, and entry symbols to separate files.
 * Finally, it frees the memory allocated for the filtered tables.
 *
 * @param code_img The machine code image.
 * @param data_img The data image.
 * @param icf The final instruction counter value.
 * @param dcf The final data counter value.
 * @param filename The base filename for output files.
 * @param symbol_table The symbol table.
 * @return Returns true if all files were written successfully, otherwise returns false.
 */
int write_output_files(machine_word **code_img, long *data_img, long icf, long dcf, char *filename, table symbol_table);

#endif