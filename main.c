#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "second_pass.h"
#include "globals.h"
#include "utils.h"



/*
Things to fix

----- cmp r3, #sz -> #sz
----- LIST: .data 6, -9, len -> len
------ MAIN: mov r3, LIST[sz] -> sz

*/

/*Processes singel assembly file*/
bool handel_singel_file(char *file_name);


int main(int argc, char *argv[]){
    int number_of_file; /*Number of input assembly files*/

    /* In case of erroe break line */
    bool succeeded = TRUE;

    if (argc == 1){
        printf("You didn't enter any files\n");
    }
   
    /*Processes each file*/
    for (number_of_file = 1; number_of_file < argc; number_of_file++){
        if (!succeeded) { /* if last process failed and there's another file, break line: */
            puts("");
        }
        succeeded = handel_singel_file(argv[number_of_file]);
    }
    return 0;
}



bool handel_singel_file(char *file_name) {
    int temp_c; /*Temporary variable for skiping remainin characters*/
    int index_l;
    long ic = IC_INIT_VALUE;
    long dc = 0;
    long icf;
    long dcf;
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
            printf("Line too long to process.");

            /* Skip remaining characters in the line */
            temp_c = fgetc(input_file);
            while (temp_c != '\n' && temp_c != EOF) {
                temp_c = fgetc(input_file);
            } 
        }
        else{
            if (!process_line_fpass(current_line, &ic, &dc, code_img, data_img, &symbol_table)){
                if (is_success) {
                    icf = -1;
                    is_success = FALSE;
                }
            }  
        }
    }
    
    /* Saves ICF and DCF*/
    icf = ic;
    dcf = dc;

    if (is_success) {

        ic = IC_INIT_VALUE;

        /* Adds teh IC to each DC for each of teh data symbols in the table*/
        add_value_to_type(symbol_table, icf, DATA_SYMBOL);
        
        /* Starts teh second pass*/
        rewind(input_file); /* Starts from the beginning of the file */
        
        for (current_line.line_number = 1; !feof(input_file); current_line.line_number++) {
            index_l = 0;
            fgets(temp_line, MAX_LINE_LENGTH, input_file); /* Gets the line */
            index_l = skip_spaces(temp_line, index_l);  /*Skips all the spaces or tabs*/
            if (code_img[ic - IC_INIT_VALUE] != NULL || temp_line[index_l] == '.') {
                is_success &= process_line_spass(current_line, &ic, code_img, &symbol_table);
            }
        }
    }
    printf("all good");


  

	/* return whether every assembling succeeded */
    return is_success;
}


