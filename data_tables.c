#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "data_tables.h"
#include "utility_functions.h"

/* Adds an item to the table in sorted order by value */
void add_table_item(table *tab, char *name, long value, symbol_type type){
    char* copy_name;    /* Copy of the name string */
    table prev_table;   /* Previous table entry */
    table curr_table;   /* Current table entry */
    table new_table;    /* New table entry */

    /* Allocate memory for the new table entry */
    new_table = (table)malloc(sizeof(table_entry));
    if (new_table == NULL){
        printf("Memory not allocated.\n");
        return;
    }

    /* Allocate memory for the copy of the name */
    copy_name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (copy_name == NULL){
        printf("Memory not allocated.\n");
        return;
    }

    /* Copy the name and assign other values */
    strcpy(copy_name, name);
    new_table->name = copy_name;
    new_table->value = value;
    new_table->type = type;

    /* If the table is empty, set the new table item as the head of the table*/
    if ((*tab) == NULL || (*tab)->value > value) {
		new_table->next = (*tab);
		(*tab) = new_table;
		return;
	}

    /* Insert the new entry into the table in sorted order */
    curr_table = (*tab)->next;
	prev_table = *tab;
    while (curr_table != NULL && curr_table->value < value){
        prev_table = curr_table;
        curr_table = curr_table->next;
    }
    new_table->next = curr_table;
    prev_table->next = new_table;
}

/* Adds a value to the specified type in the table. */
void add_value_to_type(table tab, long to_add, symbol_type type) {
    table curr_item;
    for (curr_item = tab; curr_item != NULL; curr_item = curr_item->next) {
        if (curr_item->type == type) {
            curr_item->value += to_add;
        }
    }
}

/* Checks if the item name matches a given name */
bool check_item_name(char *table_name, char *new_name){
    return strcmp(table_name, new_name) == 0;
}

/* Finds an item by its type in the table */
table_entry *find_by_types(table tab, char *name){
    /* If the table is empty, there is nothing to find*/
    if (tab == NULL){
        return NULL;
    }
    
    /* Gose over all the table, and checks if the type is valid and have the same name*/
    do {
        switch (tab->type) {
            case CODE_SYMBOL:
            case DATA_SYMBOL:
            case EXTERNAL_SYMBOL:
            case ENTRY_SYMBOL:
            case EXTERNAL_REFERENCE:
            case MDEFINE_SYMBOL:
                if (check_item_name(tab->name, name)) {
                    return tab;
                }
                break;
        }
    } while ((tab = tab->next) != NULL);
    
    /* NO item was found*/
    return NULL;
}

/* Frees the memory allocated for the table */
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

/* Filters the table by a specific type */
table filter_table_by_type(table tab, symbol_type type) {
	table new_table = NULL;
	/* For each entry, check if has the type. if so, insert to the new table. */
	do {
		if (tab->type == type) {
			add_table_item(&new_table, tab->name, tab->value, tab->type);
		}
	} while ((tab = tab->next) != NULL);
	return new_table; /* It holds a pointer to the first entry, dynamically-allocated, so it's fine (not in stack) */
}






