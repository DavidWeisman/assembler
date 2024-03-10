#ifndef _TABLE_H
#define _TABLE_H


/** The symbols types */
typedef enum symbol_type {
	CODE_SYMBOL,
	DATA_SYMBOL,
	EXTERNAL_SYMBOL,
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

void add_table_item(table *tab, char *key, long value, symbol_type type);

table_entry* find_by_types(table tab, char *key);


#endif