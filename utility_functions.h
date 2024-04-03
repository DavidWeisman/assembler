#ifndef _UTILS_H
#define _UTILS_H

#include "global_variables.h"
#include "data_tables.h"

/**
 * @brief Adds an extension to a file name.
 * 
 * This function takes a file name and an extension, and concatenates them to create a new file name
 * with the provided extension added.
 * 
 * @param original_file_name The original file name.
 * @param extension The extension to be added to the file name.
 * 
 * @return A pointer to a new string containing the file name with the added extension.
 *         Returns NULL if memory allocation fails.
 */
char* add_extension(char *original_file_name, char *extension );

/**
 * @brief Skips whitespace characters in a string starting from the given index.
 * 
 * This function iterates through the string starting from the given index and skips
 * any whitespace characters (' ' or '\t') until a non-whitespace character is encountered
 * or the end of the string is reached.
 * 
 * @param string The string to skip whitespace characters from.
 * @param index The starting index in the string.
 * 
 * @return The index of the first non-whitespace character encountered after skipping whitespace characters.
 */
int skip_spaces(char *string, int index);

/**
 * @brief Checks if a string represents a valid integer.
 * 
 * This function checks if the given string represents a valid integer. It allows an optional
 * leading positive or negative sign ('+' or '-'). The string is considered a valid integer if
 * it consists entirely of digits after removing the sign.
 * 
 * @param string The string to be checked.
 * 
 * @return Returns true if the string represents a valid integer, otherwise returns false.
 */
bool check_if_digit(char *string);

/**
 * @brief Checks if a string contains only alphanumeric characters.
 * 
 * This function iterates through the characters of the string and checks if each character
 * is either alphabetic or a digit. If any character is neither alphabetic nor a digit, the function
 * returns false; otherwise, it returns true.
 * 
 * @param string The string to be checked.
 * @return Returns true if the string contains only alphanumeric characters, otherwise false.
 */
bool chec_all_str_alpha(char *string);

/**
 * @brief Checks if a word is a special word.
 * 
 * This function checks if the given word is a special word by comparing it with opcodes,
 * register names, and instruction names. If the word matches any opcode, register name,
 * or instruction name, it is considered a special word and the function returns true; otherwise,
 * it returns false.
 * 
 * @param word The word to be checked.
 * @return Returns true if the word is a special word, otherwise false.
 */
bool is_special_word(char *word);

/**
 * @brief Checks if a label name is valid.
 * 
 * This function checks if the given label name is valid according to certain criteria:
 * - The label name must not be an empty string.
 * - The label name must have a length less than 32 characters.
 * - The first character of the label name must be alphabetic.
 * - All subsequent characters of the label name must be alphabetic.
 * - The label name must not be a special word.
 * 
 * @param label_name The label name to be checked.
 * 
 * @return Returns true if the label name is valid, otherwise returns false.
 */
bool check_label_name(char *label_name);

/**
 * @brief Extracts a label from a line of code.
 * 
 * This function extracts a label from the given line of code and stores it in the symbol array.
 * It skips leading spaces in the line and copies characters from the line to the symbol array until
 * a colon (':') or the end of the line is encountered. It then checks if the extracted label is valid
 * using the check_label_name function. If the label is invalid, it prints an error message and sets
 * the symbol array to an empty string.
 * 
 * @param line The line of code from which the label is extracted.
 * @param symbol The array to store the extracted label.
 * 
 * @return Returns true if an error occurs during label extraction, otherwise false.
 */
bool extract_label(line_info line, char *symbol);

/**
 * @brief Finds the instruction corresponding to the given name.
 * 
 * This function searches for the instruction with the given name in the instructions lookup table.
 * If the instruction is found, its corresponding value is returned. If no instruction is found with
 * the given name, the function returns NONE_INST.
 * 
 * @param name The name of the instruction to be found.
 * @return The instruction corresponding to the given name, or NONE_INST if not found.
 */
instruction find_instruction_by_name(char *name);

/**
 * @brief Prints an error message.
 * 
 * This function prints an error message to the error output file, indicating the file name and line number
 * where the error occurred, followed by the provided error message. The error message can include additional
 * format specifiers and arguments.
 * 
 * @param line The line_info struct containing information about the error location.
 * @param message The error message to be printed.
 * @param ... Additional format specifiers and arguments to be included in the error message.
 * @return Returns true if the error message is successfully printed, otherwise false.
 */
bool print_error(line_info line, char *massage, ...);

/**
 * @brief Checks and processes a line for define definition.
 * 
 * This function checks if the input line contains a define definition syntax (.define),
 * extracts the label name and associated value, validates them, and updates the symbol table
 * if the definition is valid. It prints error messages if any validation fails.
 * 
 * @param line The line_info struct representing the line to be processed.
 * @param symbol_table The symbol table to be updated.
 * @param symbol The symbol array to store the extracted label.
 * @return Returns true if the line is successfully processed, otherwise false.
 */
bool check_mdefine(line_info line, table *symbol_table, char *symbol);

/**
 * @brief Frees memory allocated for the code image.
 * 
 * This function iterates through each index of the code image array and frees the memory
 * allocated for each machine word and its associated data if applicable. If the machine word
 * is not NULL, it checks its length to determine whether it's a code word or a data word,
 * and frees the appropriate memory block accordingly. Finally, it frees the memory for the
 * machine word itself and sets the corresponding index in the code image array to NULL.
 * 
 * @param code_image The pointer to the array of machine words representing the code image.
 * @param total_instructions The total number of instructions in the code image array.
 */
void free_code_image(machine_word **code_image, long total_instructions);

#endif