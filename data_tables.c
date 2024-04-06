#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "data_tables.h"
#include "utility_functions.h"

/* Adds a new item to the symbol table */
void add_table_item(table *tab, char *name, long value, symbol_type type) {
    char* copy_name;            /* Copy of the symbol name */
    table prev_table;           /* Pointer to the previous table entry */
    table curr_table;           /* Pointer to the current table entry */
    table new_table;            /* Pointer to the new table entry */

    /* Allocate memory for the new table entry */
    new_table = (table)malloc(sizeof(table_entry));
    if (new_table == NULL) {
        printf("Memory not allocated.\n");
        return;
    }

    /* Allocate memory for the copy of the symbol name */
    copy_name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (copy_name == NULL) {
        printf("Memory not allocated.\n");
        return;
    }

    /* Copy the symbol name */
    strcpy(copy_name, name);
    new_table->name = copy_name;
    new_table->value = value;
    new_table->type = type;

    /* Insert the new entry into the symbol table */
    if ((*tab) == NULL || (*tab)->value > value) {
        new_table->next = (*tab);
        (*tab) = new_table;
        return;
    }

    /* Traverse the symbol table to find the correct position to insert the new entry */
    curr_table = (*tab)->next;
    prev_table = *tab;
    while (curr_table != NULL && curr_table->value < value) {
        prev_table = curr_table;
        curr_table = curr_table->next;
    }
    
    /* Insert the new entry */
    new_table->next = curr_table;
    prev_table->next = new_table;
}

/* Adds a value to all symbols of a specific type in the symbol table */
void add_value_to_type(table tab, long to_add, symbol_type type) {
    table curr_item;  /* Pointer to the current table entry */

    /* Iterate through the symbol table */
    for (curr_item = tab; curr_item != NULL; curr_item = curr_item->next) {
        /* Check if the current symbol's type matches the specified type */
        if (curr_item->type == type) {
            /* Add the specified value to the current symbol's value */
            curr_item->value += to_add;
        }
    }
}

/* Checks if the name of an item matches a given name */
bool check_item_name(char *table_name, char *new_name){
    return strcmp(table_name, new_name) == 0;
}

/* Finds an item in the table by its name and type */
table_entry *find_by_types(table tab, char *key, int symbol_count, ...) {
	int i;
    va_list arglist;
	symbol_type *valid_symbol_types = malloc((symbol_count) * sizeof(int));
    
    if (valid_symbol_types == NULL) {
        return NULL;
    }
	/* Build a list of the valid types */
	
	va_start(arglist, symbol_count);
	for (i = 0; i < symbol_count; i++) {
		valid_symbol_types[i] = va_arg(arglist, symbol_type);
	}
	va_end(arglist);
	/* table null => nothing to dos */
	if (tab == NULL) {
		free(valid_symbol_types);
		return NULL;
	}
	/* iterate over table and then over array of valid. if type is valid and same key, return the entry. */
	do {
		for (i = 0; i < symbol_count; i++) {
			if (valid_symbol_types[i] == tab->type && strcmp(key, tab->name) == 0) {
				free(valid_symbol_types);
				return tab;
			}
		}
	} while ((tab = tab->next) != NULL);
	/* not found, return NULL */
	free(valid_symbol_types);
	return NULL;
}

/* Frees the memory allocated for a table */
void free_table(table tab) {
	table prev_entry;
    table curr_entry = tab;
     
	while (curr_entry != NULL) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
		free(prev_entry->name); 
		free(prev_entry);
	}
}

/* Filters a table by symbol type */
table filter_table_by_type(table tab, symbol_type type) {
	table new_table = NULL;
	do {
		if (tab->type == type) {
			add_table_item(&new_table, tab->name, tab->value, tab->type);
		}
	} while ((tab = tab->next) != NULL);
	return new_table; 
}






