#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility_functions.h"
#include "data_tables.h"
#include "code_functions.h"

/* Finds the instruction from the index within a line */
instruction find_instruction_from_index(line_info line, int *index_line) {
    char current_instruction[MAX_LINE_LENGTH]; /* Buffer to store the current instruction */
    int index_instruction = 0; /* Index for storing characters in the current instruction */
    instruction result; /* Variable to store the result instruction */

    *index_line = skip_spaces(line.content, *index_line); /* Skip leading spaces */

    /* Check if the content at the specified index is not a '.' */
    if (line.content[*index_line] != '.') {
        return NONE_INST; /* If not, return NONE_INST */
    }

    /* Read the instruction until encountering a tab, space, or end of line */
    while (line.content[*index_line] && line.content[*index_line] != '\t' && line.content[*index_line] != ' ') {
        current_instruction[index_instruction] = line.content[*index_line]; /* Store character in current_instruction buffer */
        index_instruction++;
        (*index_line)++;
    }
    current_instruction[index_instruction] = '\0'; /* Null-terminate the current instruction string */

    /* Find the instruction by name */
    result = find_instruction_by_name(current_instruction + 1); /* Skip the '.' */
    if (result != NONE_INST) {
        return result; /* If found, return the instruction */
    }

    /* If the instruction is not found, print an error message and return ERROR_INST */
    print_error(line, "Invalid instruction name: %s", current_instruction);
    return ERROR_INST;
}

/* Processes the string instruction within a line */
bool process_string_instruction(line_info line, int index_line, long *data_img, long *dc) {
    char temp_string[MAX_LINE_LENGTH]; /* Temporary buffer to store the string content */
    char *last_quote_location = strrchr(line.content, '"'); /* Pointer to the last quote character in the line content */
    index_line = skip_spaces(line.content, index_line); /* Skip leading spaces */

    /* Check for the presence of the opening quote */
    if (line.content[index_line] != '"') {
        print_error(line, "Missing opening quote of string");
        return FALSE;
    }
    /* Check for the presence of the closing quote */
    else if (&line.content[index_line] == last_quote_location) {
        print_error(line, "Missing closing quote of string");
        return FALSE;
    }
    else {
        int index_string = 0; /* Index for storing characters in the temporary string buffer */

        /* Extract the string content between the quotes */
        while (line.content[index_line] && line.content[index_line] != '\n' && line.content[index_line] != EOF) {
            temp_string[index_string] = line.content[index_line];
            index_line++;
            index_string++;
        }

        /* Null-terminate the extracted string */
        temp_string[last_quote_location - line.content] = '\0';

        /* Convert the string content into ASCII values and store them in the data_img array */
        for (index_string = 1; temp_string[index_string] && temp_string[index_string] != '"'; index_string++) {
            data_img[*dc] = temp_string[index_string]; /* Store ASCII value */
            (*dc)++; /* Increment data counter */
        }
        data_img[*dc] = 0; /* Null-terminate the string in the data_img array */
        (*dc)++; /* Increment data counter */
    }
    return TRUE; /* Return TRUE indicating successful processing */
}

/* Processes the .data instruction within a line */
bool process_data_instruction(line_info line, int index_line, long *data_img, long *dc, table symbol_table) {
    char temp_string[MAX_LINE_LENGTH]; /* Temporary buffer to store the extracted string or number */
    char *temp_pointer; /* Temporary pointer for string to number conversion */
    long number_value; /* Parsed long integer value */
    int index_number; /* Index for storing characters in the temporary string buffer */

    index_line = skip_spaces(line.content, index_line); /* Skip leading spaces */

    if (line.content[index_line] == ',') {
        print_error(line, "Unexpected comma after .data instruction");
    }

    while (line.content[index_line] != '\n' && line.content[index_line] != EOF) {
        index_number = 0;

        /* Extract characters until encountering whitespace, comma, or newline */
        while (line.content[index_line] && line.content[index_line] != EOF && line.content[index_line] != '\t' &&
               line.content[index_line] != ' ' && line.content[index_line] != ',' && line.content[index_line] != '\n') {
            temp_string[index_number] = line.content[index_line];
            index_line++;
            index_number++;
        }

        temp_string[index_number] = '\0'; /* Null-terminate the extracted string */

        /* Convert any defined symbols to their corresponding numeric values */
        convert_defind(temp_string, symbol_table, FALSE);

        /* Check if the extracted string represents a valid integer */
        if (!check_if_digit(temp_string)) {
            print_error(line, "Expected integer for .data instruction (got '%s')", temp_string);
            return FALSE;
        }

        /* Convert the string to a long integer */
        number_value = strtol(temp_string, &temp_pointer, 10);
        data_img[*dc] = number_value; /* Store the parsed integer value */
        (*dc)++; /* Increment the data counter */

        index_line = skip_spaces(line.content, index_line); /* Skip leading spaces */

        /* Check for additional operands */
        if (line.content[index_line] == ',') {
            index_line++;
        }
        else if (!line.content[index_line] || line.content[index_line] == '\n' || line.content[index_line] == EOF) {
            break; /* End of line or file */
        }

        index_line = skip_spaces(line.content, index_line); /* Skip leading spaces */

        /* Validate the presence of expected characters after comma */
        if (line.content[index_line] == ',') {
            print_error(line, "Multiple consecutive commas.");
            return FALSE;
        }
        else if (line.content[index_line] == EOF || line.content[index_line] == '\n' || !line.content[index_line]) {
            print_error(line, "Missing data after comma");
            return FALSE;
        }
    }

    return TRUE; /* Return TRUE indicating successful processing */
}