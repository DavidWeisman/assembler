#ifndef _UTILS_H
#define _UTILS_H

#include "globals.h"
#include "table.h"
/**
 * Adds extension to the name of the file
 * Connects to strings
 * @param file_name the first string
 * @param extension the second string
 * @return A pointer to the new string
*/
char* add_extension(char *file_name, char *extension );

/**
 * Return the index of the next char in the input string
 * @param string the string we work on
 * @param index the current place on the index
 * @return the index of the next char in the input string
*/
int skip_spaces(char *string, int index);

/**
 * Gets the label from the line
 * Checks if there is already a label under the name it founds
 * If there is one, it's an erroe
 * If there isn't, save in the symbol varibal
 * @param line the input line we work on
 * @param symbol the place where goes the result
 * @return true or false based on the result
*/
bool extract_label(line_info line, char *symbol);

/**
 * Return the instruction based on the input instruction name
 * @param name The of the instruction
 * @return the instruction, and if not found NONE_INST 
*/
instruction find_instruction_by_name(char *name);

/**
 * Checks if the input word is a special word
 * Special word is on of those words -  opcode, register, instruction
 * @param word The word that we check 
 * @return True or false if the word is special or not
*/
bool is_special_word(char *word);

/**
 * Checks if all the characters are alphanumeric
 * @param string the input string we checks
 * @return True or false if all the characters are alphanumeric or not
*/
bool chec_all_str_alpha(char *string);

/**
 * Checks if the name of the label if a valid name
 * @param label_name The name the function checks
 * @return True or false if the name is valid or not 
*/
bool check_label_name(char *label_name);

/**
 * Check if all the chars in the given string are digits
 * @param string The input string that needs to be checked
 * @return True or false depens if the input string is a digit or not
*/
bool check_if_digit(char *string);

/**
 * Frees all the dynamically-allocated memory for the code image.
 * @param code_image A pointer to the code images buffer
 * @param fic The final instruction counter value
 */
void free_code_image(machine_word **code_image, long fic);

bool check_mdefine(line_info line, table *symbol_table, char *symbol);
#endif