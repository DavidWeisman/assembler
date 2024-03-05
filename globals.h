#ifndef GLOBALS_H
#define GLOBALS_H

/** Maximum length of a single source line  */
#define MAX_LINE_LENGTH 80

/* A struct that represet the informasion of a line*/
typedef struct line_info {
	/** Line number in file */
	long line_number;
	/** File name */
	char *file_name;
	/** Line content (source) */
	char *content;
} line_info;


#endif 