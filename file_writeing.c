#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility_functions.h"
#include "data_tables.h"

/* Macro to keep only the 24 least significant bits of a value.
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

void print_binary(long value) {
    /* Mask for checking each bit */
	int i;
    /* Loop through each bit and print it */
    for (i = 13; i >= 0; i--) {
        printf("%ld", (value >> i) & 1); /* Shift and mask the bit */
    }
	printf("\n");
}

/**
 * Writes machine code and data to a .ob output file.
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
 *  Writes a table to a file with the specified filename and extension.
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

/* Writes machine code and data to a .ob output file */
static bool write_ob(machine_word **code_img, long *data_img, long icf, long dcf, char *filename) {
	int index; /* Loop iterator */
    long value; /* Value to be written to the file */
    FILE *file_descriptor; /* File descriptor for the output file */
    char *output_file; /* Filename with .ob extension */

    output_file = add_extension(filename, ".ob"); /* Get output file name */
    file_descriptor = fopen(output_file, "w"); /* Open the output file for writing */
    free(output_file); /* Free memory allocated for the output filename */
    
    /* Check if file is successfully opened */
    if (file_descriptor == NULL) {
        printf("Can't create or rewrite to file %s.", output_file);
        return FALSE;
    }

    /* Write headers containing the final instruction and data counters */
    fprintf(file_descriptor, "%ld %ld", icf - IC_INIT_VALUE, dcf);

    /* Write machine code lines to the file */
    for (index = 0; index < icf - IC_INIT_VALUE; index++) {
        if (code_img[index]->length > 0) {
            value = (code_img[index]->word.code->opcode << 6) | (code_img[index]->word.code->src_addressing << 4) |
                  (code_img[index]->word.code->dest_addressing << 2) | (code_img[index]->word.code->ARE);
        } else {
            value = (KEEP_ONLY_21_LSB(code_img[index]->word.data->data) << 2) | (code_img[index]->word.data->ARE);
        }
        printf("%.7d\t", index + 100); /* Print memory address to console */
        print_binary(value); /* Print binary representation to console */
        fprintf(file_descriptor, "\n%.7d %.6lx", index + 100, value); /* Write line to file */
    }

    /* Write data lines to the file */
    for (index = 0; index < dcf; index++) {
        value = KEEP_ONLY_24_LSB(data_img[index]);
        printf("%.7d\t", icf + index); /* Print memory address to console */
        print_binary(value); /* Print binary representation to console */
        fprintf(file_descriptor, "\n%.7ld %.6lx", icf + index, value); /* Write line to file */
    }

    fclose(file_descriptor); /* Close the output file */
    return TRUE; /* Return true indicating successful operation */
}

/* Writes a table to a file with the specified filename and extension */
static bool write_table_to_file(table tab, char *filename, char *file_extension) {
	FILE *file_descriptor;
    char *full_filename = add_extension(filename, file_extension); /* Create full filename with extension */

    file_descriptor = fopen(full_filename, "w"); /* Open the output file for writing */
    free(full_filename); /* Free memory allocated for the full filename */

    /* Check if file is successfully opened */
    if (file_descriptor == NULL) {
        printf("Can't create or rewrite to file %s.", full_filename);
        return FALSE;
    }

    /* If the table is NULL or empty, return true without performing any operations */
    if (tab == NULL) return TRUE;

    /* Write table entries to the file */
    fprintf(file_descriptor, "%s %.7ld", tab->name, tab->value);
    while ((tab = tab->next) != NULL) {
        fprintf(file_descriptor, "\n%s %.7ld", tab->name, tab->value);
    }

    fclose(file_descriptor); /* Close the output file */
    return TRUE; /* Return true indicating successful operation */
}



