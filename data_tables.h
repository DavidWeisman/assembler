#ifndef _DATA_TABLES_H
#define _DATA_TABLES_H
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
 * @brief Adds a new item to the symbol table.
 *
 * This function creates a new table entry with the specified name, value, and type, and inserts it into the symbol table in sorted order based on the value.
 *
 * @param tab Pointer to the symbol table.
 * @param name The name of the symbol to be added.
 * @param value The value associated with the symbol.
 * @param type The type of the symbol (e.g., DATA_SYMBOL, CODE_SYMBOL).
 */
void add_table_item(table *tab, char *name, long value, symbol_type type);

/**
 * @brief Adds a value to all symbols of a specific type in the symbol table.
 *
 * This function iterates through the symbol table and adds the specified value to the 'value' field of all symbols of the given type.
 *
 * @param tab The symbol table.
 * @param to_add The value to add to symbols of the specified type.
 * @param type The type of symbols to which the value will be added (e.g., DATA_SYMBOL, CODE_SYMBOL).
 */
void add_value_to_type(table tab, long to_add, symbol_type type);

/**
 * @brief Checks if the name of an item matches a given name.
 *
 * This function compares the name of an item in the table with a given name and returns TRUE if they are equal, otherwise FALSE.
 *
 * @param table_name The name of the item in the table.
 * @param new_name The name to compare with the item's name.
 * @return Returns TRUE if the names match, otherwise FALSE.
 */
bool check_item_name(char *table_name, char *new_name);


/**
 * @brief Finds an item in the table by its name and type.
 *
 * This function searches for an item in the table with the specified name and type. It returns a pointer to the found item if it exists, otherwise NULL.
 *
 * @param tab The table to search within.
 * @param name The name of the item to search for.
 * @return A pointer to the found item if it exists, otherwise NULL.
 */
table_entry *find_by_types(table tab,char *key, int symbol_count, ...);

/**
 * @brief Frees the memory allocated for a table.
 *
 * This function deallocates the memory used by a table and its entries.
 *
 * @param tab The table to be freed.
 */
void free_table(table tab);

/**
 * @brief Filters a table by symbol type.
 *
 * This function creates a new table containing only the entries with the specified symbol type.
 *
 * @param tab The original table to be filtered.
 * @param type The symbol type to filter by.
 * @return Returns a new table containing entries with the specified symbol type.
 */
table filter_table_by_type(table tab, symbol_type type);

#endif