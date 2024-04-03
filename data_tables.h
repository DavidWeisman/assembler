#ifndef _TABLE_H
#define _TABLE_H
#include "global_variables.h"

/** 
 * Enum defining different symbol types.
 */
typedef enum symbol_type {
	CODE_SYMBOL,			/* Code symbol type */
	DATA_SYMBOL,			/* Data symbol type */
	EXTERNAL_SYMBOL,		/* External symbol type */
	EXTERNAL_REFERENCE,		/* External reference symbol type */
	MDEFINE_SYMBOL,			/* Mdefine symbol type */	
	ENTRY_SYMBOL			/* Entry symbol type */	
} symbol_type;				

/** 
 * Typedef for a pointer to the table entry structure.
 */
typedef struct entry* table;

/** 
 * Struct defining a table entry.
 */
typedef struct entry {
    table next;         /* Pointer to the next entry in the table */
    long value;         /* Value associated with the entry */
    char *name;         /* Name of the entry */
    symbol_type type;   /* Type of the entry (symbol_type enum) */
} table_entry;

/** 
 * Adds an item to the table in sorted order by value.
 * 
 * @param tab Pointer to the table.
 * @param name Name of the item to be added.
 * @param value Value associated with the item.
 * @param type Type of the item (symbol_type enum).
 * @return None.
 */
void add_table_item(table *tab, char *name, long value, symbol_type type);

/** 
 * Adds a value to the specified type in the table.
 * 
 * @param tab Table to add the value to.
 * @param to_add Value to add.
 * @param type Type of the item (symbol_type enum).
 * @return None.
 */
void add_value_to_type(table tab, long to_add, symbol_type type);

/** 
 * Checks if the item name matches a given name.
 * 
 * @param table_name Name of the item.
 * @param new_name Name to compare.
 * @return Returns true if the names match, otherwise false.
 */
bool check_item_name(char *table_name, char *new_name);

/** 
 * Finds an item by its type in the table.
 * 
 * @param tab Table to search.
 * @param name Name of the item to find.
 * @return Returns a pointer to the found item or NULL if not found.
 */
table_entry *find_by_types(table tab, char *name);

/** 
 * Frees the memory allocated for the table.
 * 
 * @param tab Table to free.
 * @return None.
 */
void free_table(table tab);

/** 
 * Filters the table by a specific type.
 * 
 * @param tab Table to filter.
 * @param type Type to filter by.
 * @return Returns a new table containing only items of the specified type.
 */
table filter_table_by_type(table tab, symbol_type type);

#endif
