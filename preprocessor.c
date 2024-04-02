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
    int line_num;
};


static void extract_macro(FILE *input_file, line_info line, struct macro_info *macros, int num_macro, int *new_line_num) {
    char line_string[MAX_LINE_LENGTH];
    int index_l = 0;
    int index_m = 0;
    bool finish_copy_code = FALSE;
    
    while (finish_copy_code == FALSE && fgets(line_string, MAX_LINE_LENGTH, input_file)) {
       
        if (strstr(line_string, "endmcr") == NULL) {
            strcat(macros[num_macro].code, line_string);
            (*new_line_num)++;
        }
        else {
            finish_copy_code = TRUE;
        }
    }
    index_l = skip_spaces(line.content, index_l);
    index_l += 4;
    index_l = skip_spaces(line.content, index_l);

    while (line.content[index_l] && line.content[index_l] != ' ' && line.content[index_l] != '\n') {
        macros[num_macro].name[index_m] = line.content[index_l];
        index_l++;
        index_m++;
    }
    macros[num_macro].name[index_m] = '\0';

    macros[num_macro].line_num = line.line_number;
}


static bool check_line(FILE *input_file, line_info line, struct macro_info *macros, int num_macro, int *new_line_num) {
    char temp_line[MAX_LINE_LENGTH];
    int index_l;
    int num_macro_copy = num_macro;
    while (num_macro_copy--){
        
        if (line.line_number == macros[num_macro_copy].line_num) {

            line.content = temp_line;
            do {
                index_l = 0;  
                index_l = skip_spaces(line.content, index_l);
                
                line.line_number++;
                if (strncmp(line.content + index_l, "endmcr", 6) == 0) {
                    (*new_line_num) = line.line_number;
                    return FALSE;
                }
                
            }
            while(fgets(temp_line, MAX_LINE_LENGTH, input_file) != NULL); 
        }
        
    }
    
    return TRUE;
}

static bool put_macro(line_info line, struct macro_info *macros, int number_of_macros, int *index_of_macro) {
    int index_l;
    int index_macro_arry;
    int index_m = 0;
    bool same = TRUE;
    

    for (index_macro_arry = 0; index_macro_arry < number_of_macros; index_macro_arry++) {
        index_l = 0;
        index_m = 0;
        index_l = skip_spaces(line.content, index_l);
        same = TRUE;
        while (line.content[index_l] && line.content[index_l] != ' ' && line.content[index_l] != '\0' && line.content[index_l] != '\n') {
            if (line.content[index_l] != macros[index_macro_arry].name[index_m]) {
                same = FALSE;
            }
            index_l++;
            index_m++;
            
        }
        if (same) {
            (*index_of_macro) = index_macro_arry;
            return TRUE;
        }
    }
    return FALSE;
}



void handel_macro(char *file_name) {
    FILE *input_file;
    FILE *output_file;
    line_info line;
    int index_l = 0;
    char *input_file_name;
    char *outout_file_name;
    struct macro_info macros[MAX_MACROS];
    char temp_line[MAX_LINE_LENGTH];
    int num_macros;
    int new_line_num = 0;   
    int index_m; 

    /*Adds  .as extension */
    input_file_name = add_extension(file_name, ".as");

    /*Open file with chack*/
    input_file = fopen(input_file_name, "r");
    if (input_file == NULL){
        printf("file can't be opend \n");
        free(input_file);
        return;
    } 
    /*Adds  .as extension */
    outout_file_name = add_extension(file_name, ".txt");

    output_file = fopen(outout_file_name, "w");
    if (output_file == NULL){
        printf("file can't be opend \n");
        free(outout_file_name);
        return;
    } 
    
    line.content = temp_line;
    line.line_number = 1;
    num_macros = 0;
    while (fgets(temp_line, MAX_LINE_LENGTH, input_file) != NULL) {
        index_l = 0;
        index_l = skip_spaces(line.content, index_l);
        if (strncmp(line.content + index_l, "mcr ", 4) == 0) {
            new_line_num = 0;
            extract_macro(input_file, line, macros, num_macros, &new_line_num);
            line.line_number += new_line_num;
            num_macros++; 
            line.line_number += 2;
            continue;
        }
        line.line_number++;
    }
   
    fseek(input_file, 0, SEEK_SET);
    line.content = temp_line;
    line.line_number = 1;
    
    while (fgets(temp_line, MAX_LINE_LENGTH, input_file) != NULL) {
        
        
        if(!check_line(input_file, line, macros, num_macros, &new_line_num)) {
            line.line_number = new_line_num;
            continue;
        }
        if(put_macro(line, macros, num_macros, &index_m)) {
            fprintf(output_file, "%s", macros[index_m].code);
            continue;
        }
        
        fprintf(output_file, "%s", line.content);
        line.line_number++;
    }



    fclose(output_file);
}

