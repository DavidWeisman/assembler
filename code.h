#ifndef _CODE_H
#define _CODE_H
#include "globals.h"



reg get_register_by_name(char *name);


void get_opcode(char *cmd, opcode *opcode_out);


#endif