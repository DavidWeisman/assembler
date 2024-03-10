#ifndef _CODE_H
#define _CODE_H
#include "table.h"
#include "globals.h"

/**
 * Returns the register value by the input name
 * @param *name The name of the register
 * @return The register value of the register that he found by the input name, otherwise returns NOONE_REG
*/
reg get_register_by_name(char *name);

/**
 * Puts into the opcode input pointer the opcode of the command name 
 * @param *name The name of the command
 * @param *opcpde_out The pointer where the function puts the opcode of the command
*/
void get_opcode(char *name, opcode *opcode_out);

#endif