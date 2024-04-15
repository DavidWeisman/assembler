#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility_functions.h"
#include "data_tables.h"

/**
 * @brief Macro to keep only the 24 least significant bits of a value.
 *
 * This macro masks the input value with 0xFFFFFF, keeping only the 24 least significant bits.
 *
 * @param value The value whose 24 least significant bits are to be kept.
 * @return The result after masking with 0xFFFFFF to keep only the 24 least significant bits.
 */
#define KEEP_ONLY_24_LSB(value) ((value) & 0xFFFFFF)

/* Macro to keep only the 21 least significant bits of a value.
 *
 * This macro masks the input value with 0x1FFFFF, keeping only the 21 least significant bits.
 *
 * @param value The value whose 21 least significant bits are to be kept.
 * @return The result after masking with 0x1FFFFF to keep only the 21 least significant bits.
 */
#define KEEP_ONLY_21_LSB(value) ((value) & 0x1FFFFF)


/**
 * @brief Writes machine code and data to a .ob output file.
 *
 * This function writes machine code and data to a .ob output file. It opens the file for writing,
 * writes the headers containing the final instruction and data counters, then writes the machine
 * code and data lines to the file. It also prints the memory address and binary representation of
 * each code and data entry to the console. Finally, it closes the file and returns whether the
 * operation was successful.
 *
 * @param code_img The machine code image.
 * @param data_img The data image.
 * @param icf The final instruction counter value.
 * @param dcf The final data counter value.
 * @param filename The base filename for the output file.
 * @return Returns true if the file was written successfully, otherwise returns false.
 */
static bool write_ob(machine_word **code_img, long *data_img, long icf, long dcf, char *filename);

/**
 * @brief Writes a table to a file with the specified filename and extension.
 *
 * This function writes the contents of a table to a file with the specified filename and extension.
 * It opens the file for writing, writes the table entries to the file, then closes the file.
 * If the table is NULL or empty, it returns true without performing any operations.
 * If the file cannot be opened, it prints an error message and returns false.
 *
 * @param tab The table to be written to the file.
 * @param filename The base filename for the output file.
 * @param file_extension The extension for the output file.
 * @return Returns true if the table was successfully written to the file, otherwise returns false.
 */
static bool write_table_to_file(table tab, char *filename, char *file_extension);


/* Writes output files including machine code, external references, and entry symbols. */
int write_output_files(machine_word **code_img, long *data_img, long icf, long dcf, char *filename, table symbol_table) {
    bool result; /* Result of file writing operations */
    table externals = filter_table_by_type(symbol_table, EXTERNAL_REFERENCE); /* Extract external references */
    table entries = filter_table_by_type(symbol_table, ENTRY_SYMBOL); /* Extract entry symbols */
    
    /* Write machine code, external references, and entry symbols to separate files */
    result = write_ob(code_img, data_img, icf, dcf, filename) &&
             write_table_to_file(externals, filename, ".ext") &&
             write_table_to_file(entries, filename, ".ent");

    /* Free memory allocated for the filtered tables */
    free_table(externals);
    free_table(entries);

    return result; /* Return result of file writing operations */
}


/* Writes the output of the assembly process to a file in the Object (".ob") format. */
static bool write_ob(machine_word **code_img, long *data_img, long icf, long dcf, char *filename) {
    FILE *file; /* File descriptor for the output file */
    int i, j; /* Loop iterators */
    int symbol; /* Symbol to represent binary values */
    long value; /* Value to be written to the file */
    char *output_file = add_extension(filename, ".ob"); /* File name with ".ob" extension */
    
    /* Attempt to open the output file */
    if (!(file = fopen(output_file, "w"))) {
        printf("Can't create or rewrite to file %s.", output_file);
        free(output_file);
        return FALSE; /* Failed to open file, return FALSE */
    }
    free(output_file); /* Free memory allocated for the output file name */
    
    /* Write the header of the object file */
    fprintf(file, "%ld %ld", icf - IC_INIT_VALUE, dcf);
    
    /* Write the instructions to the object file */
    for (i = 0; i < icf - IC_INIT_VALUE; ++i) {
        /* Construct the value to be written based on the machine word */
        value = code_img[i]->length > 0 ?
                (code_img[i]->word.code->opcode << 6) | (code_img[i]->word.code->src_addressing << 4) |
                (code_img[i]->word.code->dest_addressing << 2) | (code_img[i]->word.code->ARE) :
                (KEEP_ONLY_21_LSB(code_img[i]->word.data->data) << 2) | (code_img[i]->word.data->ARE);
                
        /* Write the address followed by the binary representation of the value */
        fprintf(file, "\n%.7d\t", i + 100);
        for (j = 12; j >= 0; j -= 2) {
            symbol = (value >> j) & 3; 
            fputc("*#%!"[symbol], file); /* Write the appropriate symbol to represent the binary value */
        }
    }
    
    /* Write the data to the object file */
    for (i = 0; i < dcf; ++i) {
        /* Write the address followed by the binary representation of the data */
        fprintf(file, "\n%.7d\t", i + icf);
        value = KEEP_ONLY_24_LSB(data_img[i]);
        for (j = 12; j >= 0; j -= 2) {
            symbol = (value >> j) & 3; 
            fputc("*#%!"[symbol], file); /* Write the appropriate symbol to represent the binary value */
        }
    }

    fclose(file); /* Close the file */
    return TRUE; /* Successfully wrote the object file, return TRUE */
}





/* Writes a table to a file with the specified filename and extension */
static bool write_table_to_file(table tab, char *filename, char *file_extension) {
	FILE *file_descriptor;
    char *full_filename;

    /* If the table is NULL or empty, return true without performing any operations */
    if (tab == NULL) {
        return TRUE;
    }

    full_filename = add_extension(filename, file_extension); /* Create full filename with extension */

    file_descriptor = fopen(full_filename, "w"); /* Open the output file for writing */
    free(full_filename); /* Free memory allocated for the full filename */

    /* Check if file is successfully opened */
    if (file_descriptor == NULL) {
        printf("Can't create or rewrite to file %s.", full_filename);
        return FALSE;
    }

    /* Write table entries to the file */
    fprintf(file_descriptor, "%s %.7ld", tab->name, tab->value);
    while ((tab = tab->next) != NULL) {
        fprintf(file_descriptor, "\n%s %.7ld", tab->name, tab->value);
    }

    fclose(file_descriptor); /* Close the output file */
    return TRUE; /* Return true indicating successful operation */
}



