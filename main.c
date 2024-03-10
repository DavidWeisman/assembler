#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "first_pass.h"
#include "globals.h"


/*Processes singel assembly file*/
void handel_singel_file(char *file_name);


int main(int argc, char *argv[]){
    int number_of_file; /*Number of input assembly files*/

    if (argc == 1){
        printf("You didn't enter any files\n");
    }

    /*Processes each file*/
    for (number_of_file = 1; number_of_file < argc; number_of_file++){
        handel_singel_file(argv[number_of_file]);
    }
    return 0;
}



void handel_singel_file(char *file_name){
    FILE *input_file; /*Current assembly file*/
    char *input_filename;
    int temp_c; /*Temporary variable for skiping remainin characters*/
    line_info current_line;
    char temp_line[MAX_LINE_LENGTH + 2]; /*Temporary string variable representing an input line*/

    /*Adds  .as extension */
    input_filename = add_extension(file_name, ".as");


    /*Open file with chack*/
    input_file = fopen(input_filename, "r");
    if (input_file == NULL){
        printf("file can't be opend \n");
        free(input_file);
        return;
    } 

    /*Start first pass:*/
    current_line.file_name = input_filename;
    current_line.content = temp_line;
    current_line.line_number = 1;

    /* Processes each line for the file separately */
    while (fgets(temp_line, MAX_LINE_LENGTH + 2, input_file) != NULL)
    {
        /* In case of to o long line */    
        if (strchr(temp_line, '\n') == NULL && !feof(input_file)){
            printf("Line too long to process.");

            /* Skip remaining characters in the line */
            temp_c = fgetc(input_file);
            while (temp_c != '\n' && temp_c != EOF) {
                temp_c = fgetc(input_file);
            } 
        }
        else{
            
            
        }
    }
    
   
    

}


