#ifndef _FIRST_PASS_H
#define _FIRST_PASS_H
#include "globals.h"
#include "table.h"

/*Processes a single line in the first pass*/
bool process_line_fpass(line_info line, table *symbol_table, long *IC, long *DC, long *data_img);

#endif