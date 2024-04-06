#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_writeing.h"
#include "parsing_phase.h"
#include "analysis_phase.h"
#include "global_variables.h"
#include "utility_functions.h"
#include "preprocessor.h"

/**
 * @brief  Processes a singel assembly input file, and returns the result
 * @param file_name The file name
 * @return True or false, if succeeded or not
*/
bool handle_single_file(char *file_name);

/**
 * @brief The main function for the assembler program.
 * 
 * This function serves as the entry point for the assembler program. It processes command-line arguments,
 * processes macros in the first input file, then iterates over the remaining input files, handling each one individually.
 * 
 * @param argc The number of command-line arguments.
 * @param argv An array of strings containing the command-line arguments.
 * @return An integer indicating the exit status of the program.
 */
int main(int argc, char *argv[]){
    int file_index;         /* Index of the input file in the argv array */
    bool succeeded = TRUE;  /* Flag indicating the success of file processing */     

    /* Check if no input files are provided */
    if (argc == 1){
        printf("You didn't enter any files\n");
    }

    /* Iterate over the input files, starting from the second element in the argv array */
    for (file_index = 1; file_index < argc; ++file_index) {
        /* If a previous file processing failed, print a newline */
		if (!succeeded) {
            puts("");
        } 

        /* Process macros in the current input file */
        process_macros(argv[file_index]);
        /* Process the current input file and update the success flag */
		succeeded = handle_single_file(argv[file_index]);
	}
    
    /* Return 0 to indicate successful program execution */
	return 0;
}


bool handle_single_file(char *file_name) {
    /* File-related variables */
    char *input_file_name;                /* Name of the input assembly file */ 
    FILE *input_file_ptr;                 /* Current assembly file */

    /* Line processing variables */
    char current_line_buffer[MAX_LINE_LENGTH + 2];  /* Temporary string variable representing an input line */
    line_info current_line;               /* Information about the current line being processed */
    int current_character;                /* Temporary variable for skipping remaining characters*/
    int line_index;                       /* Index of line, used to track the position within a line of the input file */

    /* Assembly counters */
    long ic = IC_INIT_VALUE;                /* Instruction counter */
    long dc = 0;                            /* Data counter */
    long beginning_ic_value;                /* Initial value of instruction counter */
    long beginning_dc_value;                /* Initial value of data counter */

    /* Symbol table and image arrays */
    table symbol_table = NULL;              /* The symbol table */
    long data_image[CODE_ARR_IMG_LENGTH];   /* Contains an image of the data */
    machine_word *code_image[CODE_ARR_IMG_LENGTH];  /* Contains an image of the machine code */

    /* Assembly process status */
    bool process_success = TRUE;
    

    /* Add .as extension to input file name */
    input_file_name = add_extension(file_name, ".am");

    /* Open input file with error checking */
    input_file_ptr = fopen(input_file_name, "r");
    if (input_file_ptr == NULL){
        printf("file can't be opened \n");
        free(input_file_name);
        return FALSE;
    } 
    
    /* Initialize line information */
    current_line.file_name = input_file_name;
    current_line.content = current_line_buffer;
    current_line.line_number = 1; 

    /* First iteration: process each line of the input file */
    while (fgets(current_line_buffer, MAX_LINE_LENGTH + 2, input_file_ptr) != NULL) {
        /* Check for line length exceeding the maximum */ 
        if (strchr(current_line_buffer, '\n') == NULL && !feof(input_file_ptr)) {
            print_error(current_line, "Line too long to process. Maximum line length should be %d.", MAX_LINE_LENGTH);
            process_success = FALSE;

            /* skip leftovers */
            current_character = fgetc(input_file_ptr);
            while (current_character != '\n' && current_character != EOF) {
                current_character = fgetc(input_file_ptr);
            } 
        } else {
            /* Process the line in the current iteration */
            if (!process_line_fpass(current_line, &ic, &dc, code_image, data_image, &symbol_table)){
                if (process_success) {
                    beginning_ic_value = -1;
                    process_success = FALSE;
                }
            }  
        }
        current_line.line_number++;
    }
    
    /* Save initial IC and DC values */
    beginning_ic_value = ic;
    beginning_dc_value = dc;
    /* If the first iteration succeeded, start the second iteration */
    if (process_success) {
        ic = IC_INIT_VALUE;
        add_value_to_type(symbol_table, beginning_ic_value, DATA_SYMBOL);
        rewind(input_file_ptr);
        
        /* Second iteration: process each line of the input file */
        for (current_line.line_number = 1; !feof(input_file_ptr); current_line.line_number++) {
            line_index = 0;
            fgets(current_line_buffer, MAX_LINE_LENGTH, input_file_ptr);
            line_index = skip_spaces(current_line_buffer, line_index);
            if (code_image[ic - IC_INIT_VALUE] != NULL || current_line_buffer[line_index] == '.') {
                process_success &= process_line_spass(current_line, &ic, code_image, &symbol_table);
            }
            current_line.line_number++;
        }

        /* If second iteration succeeded, write output files */
		if (process_success) {
			process_success = write_output_files(code_image, data_image, beginning_ic_value, beginning_dc_value, file_name, symbol_table);
		}
    }

    /* Clean up resources */
	free(input_file_name);
	free_table(symbol_table);
	free_code_image(code_image, beginning_ic_value);

	return process_success;
}



