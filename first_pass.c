#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "utils.h"


void process_line_fpass(line_info line){

    int index = 0;
    char symbol[MAX_LINE_LENGTH];

    index = skip_spaces(line.content, index); /*Skips all the spaces of tabs*/

    /*Cheaks if the line is empty or a comment line*/
    /*If it does return 1 - no error*/
    if (!line.content[index] || line.content[index] == '\n' || line.content[index] == EOF || line.content[index] == ';'){
        return;
    } 


    

    
}
