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
    char name[MAX_NAME_LENGTH];
    char code[MAX_CODE_LENGTH];
};


void extract_macro(FILE *input_file, struct macro_info *macro) {
    char line[MAX_LINE_LENGTH];

    /* Read lines until "endmcr" directive is found */
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        if (strstr(line, "endmcr") != NULL) {
            break;
        }
        strcat(macro->code, line);
    }
}

bool handel_macro(char *file_name, bool *result) {
    FILE *input_file;
    FILE *output_file;
    int num_macros = 0;
    int index_l = 4;
    int index_m = 0;
    bool macro_found;
    int temp_index;
    char *input_file_name;
    char *outout_file_name;
    struct macro_info macros[MAX_MACROS];
    char line[MAX_LINE_LENGTH];

    /*Adds  .as extension */
    input_file_name = add_extension(file_name, ".as");

    /*Open file with chack*/
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL){
        printf("file can't be opend \n");
        free(input_file_name);
        return FALSE;
    } 

    /*Adds  .as extension */
    outout_file_name = add_extension(file_name, ".as");

    output_file = fopen(outout_file_name, "w");
    if (output_file == NULL){
        printf("file can't be opend \n");
        free(outout_file_name);
        return FALSE;
    } 

    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        if (strstr(line, "mcr") != NULL) {
            extract_macro(input_file, &macros[num_macros]);
            num_macros++;

            while (line[index_l] && line[index_l] != '\n' && line[index_l] != ' ') {
                macros[num_macros].name[index_m] = line[index_l];
                index_l++;
                index_m++;
            }
            macros[num_macros].name[index_m] = '\0';
        }
    }

    fseek(input_file, 0, SEEK_SET);
    
    while (fgets(line, MAX_LINE_LENGTH, input_file) != NULL) {
        macro_found = FALSE;

        for (temp_index = 0; temp_index < num_macros; temp_index++) {
            if (strstr(line, macros[temp_index].name) != NULL) {
                fprintf(output_file, "%s", macros[temp_index].code);
                macro_found = TRUE;
            }
        }
        if (!macro_found) {
            fprintf(output_file, "%s", line);
        }
    }
    return TRUE;
}

    
    