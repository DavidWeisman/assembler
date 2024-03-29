#ifndef _TABLE_H
#define _TABLE_H
#include "global_variables.h"
/** The symbols types */
typedef enum symbol_type {
	CODE_SYMBOL,
	DATA_SYMBOL,
	EXTERNAL_SYMBOL,
	EXTERNAL_REFERENCE,
	MDEFINE_SYMBOL,
	ENTRY_SYMBOL
} symbol_type;

/** pointer to table entry is just a table. */
typedef struct entry* table;

typedef struct entry {
	/** Next entry in table */
	table next;
	/** Address of the symbol */
	long value;
	/** Key (symbol name) is a string (aka char*) */
	char *name;
	/** Symbol type */
	symbol_type type;
} table_entry;

/**
 * Adds a new item to the table while keeping the table sorted
 * @param tab A pointer to the table
 * @param name The name of the new item to insert
 * @param value The value of the new item
 * @param tepy The type of the new item
*/
void add_table_item(table *tab, char *name, long value, symbol_type type);

/**
 * Compares between two input strings
 * @param table_name The first string
 * @param new_name The second string
 * @return True or false if the both strings are the same or not 
*/
bool check_item_name(char *table_name, char *new_name);

/**
 * Finds the item from the tabel by their types
 * @param tab A pointer to the table
 * @param name The name of the item
 * @return The table item, if not found NULL
*/
table_entry *find_by_types(table tab, char *name);

/**
 * Deallocates all the memory required by the table.
 * @param tab The table to deallocate
 */
void free_table(table tab);

/**
 * Adds the value to_add into the values of each table item
 * @param tab The table, containing the tables items
 * @param to_add the value to add
 * @param type the type of teh symbols to add the value to
*/
void add_value_to_type(table tab, long to_add, symbol_type type);

/**
 * Returns all the entries by their type in a new table
 * @param tab The table
 * @param type The type to look for
 * @return A new table, which contains the entries
 */
table filter_table_by_type(table tab, symbol_type type);
#endif
