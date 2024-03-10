#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H
#include "globals.h"


instruction find_instruction_from_index(line_info line, int *index);

bool process_string_instruction(line_info line, int index_l, long *data_img, long *dc);

#endif