#include "bytecode.h"
#include <stddef.h>

char *OPCODE_NAMES[] = {
    "PUSH",
    "ADD",
    "MUL",
    "SUB",
    "PRINT",
    "JUMP",
    "JZ",
    "EQUAL",
    "LESS_THAN",
    "GREATER_THAN",
    "DIV",
    "STORE",
    "LOAD",
    "CALL",
    "RET",
    "STORE_LOCAL",
    "LOAD_LOCAL",
    "HALT",
    NULL};
;
char *opcode_to_string(int opcode)
{
    int total_opcodes = sizeof(OPCODE_NAMES) / sizeof(OPCODE_NAMES[0]);
    if (opcode >= 0 && opcode < total_opcodes)
    {
        return OPCODE_NAMES[opcode];
    }
    return "UNKNOWN";
}