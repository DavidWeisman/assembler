#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global_variables.h"
#include "utility_functions.h"

#define MAX_NAME_LENGTH 50
#define MAX_CODE_LENGTH 1024
#define MAX_MACROS 10

/* Define struct for macro information */
struct macro_info {
    char macro_name[MAX_NAME_LENGTH];
    char macro_code[MAX_CODE_LENGTH];
    int macro_line_number;
};

/**
 * @brief Extracts a macro definition from an input file and populates a struct with its information.
 * 
 * This function reads lines from the input file until it encounters the "endmcr" marker,
 * indicating the end of the macro definition. It extracts the macro name and code,
 * and records the line number where the macro definition starts.
 * 
 * @param input_file Pointer to the input file stream.
 * @param line A struct containing the content of the line where the macro definition starts and its corresponding line number.
 * @param macros An array of struct macro_info where the extracted macro information will be stored.
 * @param index_of_macro The index of the macro_info array where the extracted macro information will be stored.
 * @param new_line_num Pointer to an integer storing the number of lines processed in the input file.
 * 
 * @return void
 */
static void extract_macro(FILE *input_file, line_info line, struct macro_info *macros, int index_of_macro, int *new_line_num) {
    char current_line[MAX_LINE_LENGTH];
    int line_index  = 0;
    int macro_index  = 0;
    bool code_copy_finished  = FALSE;
    
    /* Read lines from the input file until encountering "endmcr" marker */
    while (code_copy_finished  == FALSE && fgets(current_line, MAX_LINE_LENGTH, input_file)) {
        /* Check if the line contains "endmcr" marker */
        if (strstr(current_line, "endmcr") == NULL) {
            /* Concatenate the line content to the macro's code */
            strcat(macros[index_of_macro].macro_code, current_line);
            (*new_line_num)++;   /* Increment the line count */
        }
        else {
            code_copy_finished  = TRUE; /* Mark the end of macro definition */
        }
    }

    /* Skip spaces and move to the macro name */
    line_index  = skip_spaces(line.content, line_index );
    line_index  += 4; /* Skip the "mcr" part */
    line_index  = skip_spaces(line.content, line_index );

    /* Extract the macro name */
    while (line.content[line_index ] && line.content[line_index ] != ' ' && line.content[line_index ] != '\n') {
        macros[index_of_macro].macro_name[macro_index ] = line.content[line_index ];
        line_index++;
        macro_index ++;
    }
    macros[index_of_macro].macro_name[macro_index ] = '\0'; /* Null-terminate the macro name */

    macros[index_of_macro].macro_line_number = line.line_number; /* Record the line number of macro definition */
}

/**
 * @brief Skips lines corresponding to macro definitions in the input file.
 * 
 * This function checks if the current line number matches any macro definition's line number.
 * If a match is found, it reads subsequent lines until it encounters the "endmcr" marker
 * or reaches the end of the file. This is done to skip macro definitions in the input file
 * since they are not directly copied to the output file.
 * 
 * @param input_file Pointer to the input file stream.
 * @param line A struct containing the content of the current line and its corresponding line number.
 * @param macros An array of struct macro_info containing information about defined macros.
 * @param number_of_macros The number of macros in the macros array.
 * @param end_of_macro_line Pointer to an integer storing the line number where the macro definition ends.
 * @return Returns false if the current line marks the end of a macro definition, otherwise returns true.
 */
static bool skip_macro_definitions(FILE *input_file, line_info line, struct macro_info *macros, int number_of_macros, int *end_of_macro_line) {
    char line_buffer[MAX_LINE_LENGTH];
    int line_index;
    int remaining_macro_count = number_of_macros;

    /* Iterate through the macros to find the one matching the current line number */
    while (remaining_macro_count--){
        
        if (line.line_number == macros[remaining_macro_count].macro_line_number) {
            line.content = line_buffer;

            /* Read subsequent lines until the end of the current macro definition is found */
            do {
                /* Skip spaces and move to the next char in the line*/
                line_index = 0;  
                line_index = skip_spaces(line.content, line_index);
                line.line_number++; /* Increment the line count */

                /* Check if the line contains the "endmcr" marker */
                if (strncmp(line.content + line_index, "endmcr", 6) == 0) {
                    (*end_of_macro_line) = line.line_number;
                    return FALSE;   /* Macro definition found, skip to the endmcr marker */
                }
                
            }
            while(fgets(line_buffer, MAX_LINE_LENGTH, input_file) != NULL); 
        }
        
    }
    return TRUE;    /* Not a macro definition, continue processing */
}

/**
 * @brief Checks if the current line contains a macro invocation and retrieves the corresponding macro index.
 * 
 * This function iterates through the list of defined macros and checks if the current line content matches
 * any macro's name. If a match is found, it stores the index of the matched macro in the index_of_macro pointer
 * and returns true, indicating that the current line contains a macro invocation. Otherwise, it returns false.
 * 
 * @param line A struct containing the content of the current line.
 * @param macros An array of struct macro_info containing information about defined macros.
 * @param number_of_macros The number of macros in the macros array.
 * @param macro_index_output Pointer to an integer storing the index of the matched macro.
 * 
 * @return Returns true if the current line contains a macro invocation, otherwise returns false.
 */
static bool find_macro_index(line_info line, struct macro_info *macros, int number_of_macros, int *macro_index_output) {
    int line_index;
    int matched_macro_index;
    int macro_name_index = 0;
    bool is_macro_match  = TRUE;
    
    /* Iterate through the macros array to find a matching macro */
    for (matched_macro_index = 0; matched_macro_index < number_of_macros; matched_macro_index++) {
        line_index = 0;
        macro_name_index = 0;
        line_index = skip_spaces(line.content, line_index);
        is_macro_match  = TRUE;

        /* Check if the content of the current line matches the macro name */
        while (line.content[line_index] && line.content[line_index] != ' ' && line.content[line_index] != '\0' && line.content[line_index] != '\n') {
            if (line.content[line_index] != macros[matched_macro_index].macro_name[macro_name_index]) {
                is_macro_match  = FALSE;
            }
            line_index++;
            macro_name_index++;
            
        }

        /* If a matching macro is found, update the macro_index_output and return true */
        if (is_macro_match ) {
            (*macro_index_output) = matched_macro_index;
            return TRUE;
        }
    }

    /* No matching macro found, return false*/
    return FALSE;
}

void process_macros(char *file_name) {
    FILE *input_file_ptr;                   /* Pointer to input file */
    FILE *output_file_ptr;                  /* Pointer to output file */
    line_info current_line;                 /* Information about the current line */
    int line_index = 0;                     /* Index of the current line */
    char *input_file_name;                  /* Pointer to input file name */
    char *outout_file_name;                 /* Pointer to output file name */
    struct macro_info macros[MAX_MACROS];   /* Array to store macros */
    char line_buffer[MAX_LINE_LENGTH];      /* Buffer for temporary line storage */
    int total_macros;                       /* Total number of macros */
    int macro_end_line = 0;                 /* Line number where macro definition ends */
    int matched_macro_index;                /* Index of the matched macro */

    /* Add .as extension to input file name */
    input_file_name = add_extension(file_name, ".as");

    /* Open input file with error checking */
    input_file_ptr = fopen(input_file_name, "r");
    if (input_file_ptr == NULL){
        printf("file can't be opend \n");
        free(input_file_ptr);
        return;
    } 

    /* Add .txt extension to output file name */
    outout_file_name = add_extension(file_name, ".txt");

    /* Open output file with error checking */
    output_file_ptr = fopen(outout_file_name, "w");
    if (output_file_ptr == NULL){
        printf("file can't be opend \n");
        free(outout_file_name);
        return;
    } 
    
    current_line.content = line_buffer;
    current_line.line_number = 1;
    total_macros = 0;

    /* Read input file line by line */
    while (fgets(line_buffer, MAX_LINE_LENGTH, input_file_ptr) != NULL) {
        line_index = 0;
        line_index = skip_spaces(current_line.content, line_index);

        /* Check for macro definition */
        if (strncmp(current_line.content + line_index, "mcr ", 4) == 0) {
            macro_end_line = 0;
            extract_macro(input_file_ptr, current_line, macros, total_macros, &macro_end_line);
            current_line.line_number += macro_end_line;
            total_macros++; 
            current_line.line_number += 2;  /* Increment line number to skip macro definition */
            continue;
        }
        current_line.line_number++;
    }
   
    fseek(input_file_ptr, 0, SEEK_SET);
    current_line.content = line_buffer;
    current_line.line_number = 1;
    
    /*  Read input file again to handle macro invocations */
    while (fgets(line_buffer, MAX_LINE_LENGTH, input_file_ptr) != NULL) {
        
        /* Skips lines corresponding to macro definitions */
        if(!skip_macro_definitions(input_file_ptr, current_line, macros, total_macros, &macro_end_line)) {
            current_line.line_number = macro_end_line;
            continue;
        }

        /* Check if the line contains a macro invocation and replace it with macro content */
        if(find_macro_index(current_line, macros, total_macros, &matched_macro_index)) {
            fprintf(output_file_ptr, "%s", macros[matched_macro_index].macro_code);
            continue;
        }
        
        /* Write unchanged line to output file */
        fprintf(output_file_ptr, "%s", current_line.content);
        current_line.line_number++;
    }

    fclose(output_file_ptr);
}

