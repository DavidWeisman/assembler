#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "table.h"
#include "utils.h"

void add_table_item(table *tab, char *name, long value, symbol_type type){
    char* copy_name;
    table prev_table;
    table curr_table;
    table new_table;

    new_table = (table)malloc(sizeof(table_entry));
    if (new_table == NULL){
        printf("Memory not allocated.\n");
        return;
    }

    copy_name = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if (copy_name == NULL){
        printf("Memory not allocated.\n");
        return;
    }

    strcpy(copy_name, name);
    new_table->name = copy_name;
    new_table->value = value;
    new_table->type = type;

    if ((*tab) == NULL || (*tab)->value > value) {
		new_table->next = (*tab);
		(*tab) = new_table;
		return;
	}

    curr_table = (*tab)->next;
	prev_table = *tab;
    while (curr_table != NULL && curr_table->value < value){
        prev_table = curr_table;
        curr_table = curr_table->next;
    }
    
    new_table->next = curr_table;
    prev_table->next = new_table;
}


bool check_tab_name(char *table_name, char *new_name){
    return strcmp(table_name, new_name) == 0;
}

table_entry *find_by_types(table tab, char *name){

    if (tab == NULL){
        return NULL;
    }
    do {
        switch (tab->type) {
            case CODE_SYMBOL:
                if(check_tab_name(tab->name, name)){
                    return tab;
                }
                break;
            case DATA_SYMBOL:
                if(check_tab_name(tab->name, name)){
                    return tab;
                }
                break;
            case EXTERNAL_SYMBOL:
                if(check_tab_name(tab->name, name)){
                    return tab;
                }
                break;
            case ENTRY_SYMBOL:
                if(check_tab_name(tab->name, name)){
                    return tab;
                }
                break;
        }
	} while ((tab = tab->next) != NULL);
    return NULL;
}



