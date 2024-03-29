#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "table.h"
#include "utils.h"

/* Adds a new item the to table*/
void add_table_item(table *tab, char *name, long value, symbol_type type){
    char* copy_name;
    table prev_table;
    table curr_table;
    table new_table;

    /* Allocate memory for the new table item*/
    new_table = (table)malloc(sizeof(table_entry));
    if (new_table == NULL){
        printf("Memory not allocated.\n");
        return;
    }

    /* prevents form Aliasing*/
    copy_name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (copy_name == NULL){
        printf("Memory not allocated.\n");
        return;
    }

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

    /* Adds the new table item to the table*/
    curr_table = (*tab)->next;
	prev_table = *tab;
    while (curr_table != NULL && curr_table->value < value){
        prev_table = curr_table;
        curr_table = curr_table->next;
    }
    
    new_table->next = curr_table;
    prev_table->next = new_table;
}

void add_value_to_type(table tab, long to_add, symbol_type type) {
    table curr_item;
    for (curr_item = tab; curr_item != NULL; curr_item = curr_item->next) {
        if (curr_item->type == type) {
            curr_item->value += to_add;
        }
    }
}

/* Checks if the names are same */
bool check_item_name(char *table_name, char *new_name){
    return strcmp(table_name, new_name) == 0;
}

/* Find a table item by a type */
table_entry *find_by_types(table tab, char *name){
    
    /* If the table is empty, there is nothing to find*/
    if (tab == NULL){
        return NULL;
    }
    
    /* Gose over all the table, and checks if the type is valid and have the same name*/
    do {
        switch (tab->type) {
            case CODE_SYMBOL:
                if(check_item_name(tab->name, name)){
                    return tab;
                }
                break;
            case DATA_SYMBOL:
                if(check_item_name(tab->name, name)){
                    return tab;
                }
                break;
            case EXTERNAL_SYMBOL:
                if(check_item_name(tab->name, name)){
                    return tab;
                }
                break;
            case ENTRY_SYMBOL:
                if(check_item_name(tab->name, name)){
                    return tab;
                }
                break;
            case EXTERNAL_REFERENCE:
                if (check_item_name(tab->name, name)) {
                    return tab;
                }
                break;
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

void free_table(table tab) {
	table prev_entry, curr_entry = tab;
	while (curr_entry != NULL) {
		prev_entry = curr_entry;
		curr_entry = curr_entry->next;
		free(prev_entry->name); /* Didn't forget you!ssss */
		free(prev_entry);
	}
}


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






