#include <stdio.h>
#include "bytecode.h"
#include "parser.h"
#ifndef COMPILER_H
#define COMPILER_H
#define MAX_SYMBOL_LENGTH 100
#define MAX_BYTECODE 100
struct Symbol
{
    char name[100];
    int address;
};
struct SymbolTable
{
    struct Symbol symbol[MAX_SYMBOL_LENGTH];
    int count;
};
struct Compiler
{
    struct SymbolTable table;
    int bytecode[MAX_BYTECODE];
    int byteCodeSize;
};

void initialize_symbol(struct SymbolTable *symbol);
void addSymbol(struct SymbolTable *table, char *name);
int getSymbolAddress(struct SymbolTable *table, char *name);
void initializeCompiler(struct Compiler *compiler);
void emitByteCode(struct Compiler *compiler, int value);
void compileExpression(struct Compiler *compiler, struct Expression *expression);
void compileDelaration(struct VariableDeclaration *declaration, struct Compiler *compiler);
void compileAssignment(struct Compiler *compiler, struct AssignmentStatement *assingment);
void compileProgram(struct Compiler *compiler, struct Program *program);
#endif