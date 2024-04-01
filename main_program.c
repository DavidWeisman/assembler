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
 * Processes a singel assembly input file, and returns the result
 * @param file_name The file name
 * @return True or false, if succeeded or not
*/
bool handel_singel_file(char *file_name);


int main(int argc, char *argv[]){
    char output[MAX_LINE_LENGTH];
    char *output_filename, c;
    FILE *outputfile;
    int number_of_file; /*Number of input assembly files*/

    /* In case of erroe break line */
    bool succeeded = TRUE;

    if (argc == 1){
        printf("You didn't enter any files\n");
    }

    output_filename = handel_macro(argv[1]);
    

    /*Processes each file
        for (number_of_file = 1; number_of_file < argc; number_of_file++){
        if (!succeeded) {  if last process failed and there's another file, break line: 
            puts("");
        }
        succeeded = handel_singel_file(argv[number_of_file]);
    }
    
    */
    
    return 0;
}



bool handel_singel_file(char *file_name) {
    int temp_c; /*Temporary variable for skiping remainin characters*/
    int index_l; /* The index of line, used to track the position within a line of the input file*/
    long ic = IC_INIT_VALUE; /* Instruction counter*/
    long dc = 0;    /* Data counter*/
    long ic_first_pass;     /* Store the value of the Instruction Counter (IC) after the first pass */   
    long dc_first_pass;     /* Store the value of the Data Counter (DC) after the first pass */
    bool is_success = TRUE;
    char *input_filename; 
    char temp_line[MAX_LINE_LENGTH + 2]; /*Temporary string variable representing an input line*/
    FILE *input_file; /*Current assembly file*/
    long data_img[CODE_ARR_IMG_LENGTH]; /* Contains an image of the data*/
    machine_word *code_img[CODE_ARR_IMG_LENGTH]; /* Contains an image of the machine code */
    table symbol_table = NULL; /* The symbol table */
    line_info current_line;
    
    /*Adds  .as extension */
    input_filename = add_extension(file_name, ".as");

    /*Open file with chack*/
    input_file = fopen(input_filename, "r");
    if (input_file == NULL){
        printf("file can't be opend \n");
        free(input_filename);
        return FALSE;
    } 
    
    /*Start first pass:*/
    current_line.file_name = input_filename;
    current_line.content = temp_line; /* The porpus of temp_line is to read from the file*/
    current_line.line_number = 1; 

    /* Processes each line for the file separately */
    /* Stop if there is a erroe of when EOF */
    while (fgets(temp_line, MAX_LINE_LENGTH + 2, input_file) != NULL)
    {
        /* In case of too long line */    
        if (strchr(temp_line, '\n') == NULL && !feof(input_file)){
            print_error(current_line, "Line too long to process. Maximum line length should be %d.", MAX_LINE_LENGTH);
            is_success = FALSE;

            /* Skip remaining characters in the line */
            temp_c = fgetc(input_file);
            while (temp_c != '\n' && temp_c != EOF) {
                temp_c = fgetc(input_file);
            } 
        }
        else{
            if (!process_line_fpass(current_line, &ic, &dc, code_img, data_img, &symbol_table)){
                if (is_success) {
                    /* free_code_image(code_img, ic_before); */
                    ic_first_pass = -1;
                    is_success = FALSE;
                }
            }  
        }
        current_line.line_number++;
    }
    
    /* Saves IC and DC of the first pass*/
    ic_first_pass = ic;
    dc_first_pass = dc;


    /* If the first pass success, start the seconcd pass */
    if (is_success) {

        ic = IC_INIT_VALUE;

        /* Adds teh IC to each DC for each of teh data symbols in the table*/
        add_value_to_type(symbol_table, ic_first_pass, DATA_SYMBOL);
        
        /* Starts teh second pass*/
        rewind(input_file); /* Starts from the beginning of the file */
        
        for (current_line.line_number = 1; !feof(input_file); current_line.line_number++) {
            index_l = 0;
            fgets(temp_line, MAX_LINE_LENGTH, input_file); /* Gets the line */
            index_l = skip_spaces(temp_line, index_l);  /*Skips all the spaces or tabs*/
            if (code_img[ic - IC_INIT_VALUE] != NULL || temp_line[index_l] == '.') {
                is_success &= process_line_spass(current_line, &ic, code_img, &symbol_table);
            }
            current_line.line_number++;
        }

        /* Write files if second pass succeeded */
		if (is_success) {
			/* Everything was done. Write to *filename.ob/.ext/.ent */
			is_success = write_output_files(code_img, data_img, ic_first_pass, dc_first_pass, file_name, symbol_table);
		}
    }
    /* Now let's free some pointer: */
	/* current file name */
	free(input_filename);
	/* Free symbol table */
	free_table(symbol_table);
	/* Free code & data buffer contents */
	free_code_image(code_img, ic_first_pass);

	/* return whether every assembling succeeded */
	return is_success;
}


