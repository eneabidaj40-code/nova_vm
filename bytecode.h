#ifndef BYTECODE_H
#define BYTECODE_H
enum Opcode
{
    PUSH,
    ADD,
    MUL,
    SUB,
    PRINT,
    JUMP,
    JZ, // jump to zero
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
    DIV, // this is added to handle division by zero
    STORE,
    LOAD,
    CALL,
    RET,
    STORE_LOCAL,
    LOAD_LOCAL,
    HALT
};
extern char *OPCODE_NAMES[];
char *opcode_to_string(int opcode);

#endif