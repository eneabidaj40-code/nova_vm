#include <stdio.h>
#include <string.h>
#include "compiler.h"
#include "parser.h"
void initialize_symbol(struct SymbolTable *symbol)
{
    symbol->count = 0;
}
void addSymbol(struct SymbolTable *table, char *name)
{
    strcpy(table->symbol[table->count].name, name);
    table->symbol[table->count].address = table->count;
    table->count++;
}
int getSymbolAddress(struct SymbolTable *table, char *name)
{
    for (int i = 0; i < table->count; i++)
    {
        if (strcmp(table->symbol[i].name, name) == 0)
        {
            return table->symbol[i].address;
        }
    }
    return -1;
}
void initializeCompiler(struct Compiler *compiler)
{
    initialize_symbol(&compiler->table);
    compiler->byteCodeSize = 0;
}
void emitByteCode(struct Compiler *compiler, int value)
{
    compiler->bytecode[compiler->byteCodeSize] = value;
    compiler->byteCodeSize++;
}
void compileExpression(struct Compiler *compiler, struct Expression *expression)
{
    switch (expression->type)
    {
    case EXPR_NUMBER:
        emitByteCode(compiler, PUSH);
        emitByteCode(compiler, expression->data.number);
        break;

    case EXPR_IDENTIFIER:
        int address = getSymbolAddress(&compiler->table, expression->data.identifier);
        if (address == -1)
        {
            printf("Failed getting memory address \n");
            return;
        }
        else
        {
            emitByteCode(compiler, LOAD);
            emitByteCode(compiler, address);
        }
        break;

    case EXPR_BINARY:
        compileExpression(compiler, expression->data.binary.left);
        compileExpression(compiler, expression->data.binary.right);
        if (expression->data.binary.operator == TOKEN_PLUS)
        {
            emitByteCode(compiler, ADD);
        }
        if (expression->data.binary.operator == TOKEN_MINUS)
        {
            emitByteCode(compiler, SUB);
        }
        if (expression->data.binary.operator == TOKEN_STAR)
        {
            emitByteCode(compiler, MUL);
        }
        if (expression->data.binary.operator == TOKEN_SLASH)
        {
            emitByteCode(compiler, DIV);
        }
        break;
    default:
        break;
    }
}
void compileDelaration(struct VariableDeclaration *declaration, struct Compiler *compiler)
{
    compileExpression(compiler, declaration->value);
    addSymbol(&compiler->table, declaration->name);
    int address = getSymbolAddress(&compiler->table, declaration->name);
    emitByteCode(compiler, STORE);
    emitByteCode(compiler, address);
}
void compileprintStatement(struct Compiler *compiler, struct PrintStatement *printStatement)
{
    compileExpression(compiler, printStatement->value);
    emitByteCode(compiler, PRINT);
}
void compileAssignment(struct Compiler *compiler, struct AssignmentStatement *assingment)
{
    compileExpression(compiler, assingment->value);
    int address = getSymbolAddress(&compiler->table, assingment->name);
    if (address == -1)
    {
        return;
    }
    emitByteCode(compiler, STORE);
    emitByteCode(compiler, address);
}

void compileProgram(struct Compiler *compiler, struct Program *program)
{
    for (int i = 0; i < program->statementCount; i++)
    {
        if (program->statements[i]->type == STMT_VAR_DECLARATION)
        {
            compileDelaration(program->statements[i]->declaration, compiler);
        }
        if (program->statements[i]->type == STMT_PRINT)
        {
            compileprintStatement(compiler, program->statements[i]->printSt);
        }
        if (program->statements[i]->type == STMT_ASSIGNMENT)
        {
            compileAssignment(compiler, program->statements[i]->assignment);
        }
    }
    emitByteCode(compiler, HALT);
}
