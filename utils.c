#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "globals.h"

char* add_extension(char *file_name, char *extension ){

    char *new_ex = (char*)malloc((strlen(file_name) + strlen(extension) + 1) * sizeof(char));

    if(new_ex == NULL){
        printf("Memory allocation failed.");
        return NULL;
    }
    
    strcpy(new_ex, file_name);
    strcat(new_ex, extension);

    return new_ex;
}