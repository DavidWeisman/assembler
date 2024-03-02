#include <stdio.h>


void handel_singel_file(char *file_name);

int main(int argc, char *argv[])
{
    int number_of_file;

    if (argc == 0){
        printf("You didn't enter any files\n");
    }

    for (number_of_file = 1; number_of_file < argc; number_of_file++){
        
        handel_singel_file(argv[number_of_file]);

    }
    printf("dd");
    return 0;
}


void change_file_name(char *file_name){



}

void handel_singel_file(char *file_name){

    FILE *input_file;

    input_file = fopen(file_name, "r");

    if (input_file == NULL){
        printf("file can't be opend \n");
        return;
    }
/*
    //function that change the end of the file in to .as
    
    // function that handel all the macro things

    

    // first pass

    // if the first pass was a succsid then 
    
    // second pass


*/

}


