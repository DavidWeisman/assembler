#ifndef _UTILS_H
#define _UTILS_H

#include "globals.h"


char* add_extension(char *file_name, char *extension );

int skip_spaces(char *string, int index);

bool extract_label(line_info line, char *symbol);

bool check_label_name(char *label_name);

bool chec_all_str_alpha(char *string);

#endif