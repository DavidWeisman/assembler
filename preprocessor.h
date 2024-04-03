#ifndef _PREPROCESSOR_H
#define _PREPROCESSOR_H

/**
 * @brief Handles macro processing for a given input file.
 * 
 * This function reads the contents of the input file, identifies macro definitions,
 * extracts their contents, and writes them into a separate output file. Macro invocations
 * are replaced with the corresponding macro content in the output file.
 * 
 * @param file_name Pointer to a string containing the name of the input file.
 */
void process_macros(char *file_name);

#endif