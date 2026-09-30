#include "lexer.h"
#include <stdio.h>
#ifndef PARSER_H
#define PARSER_H
enum ParserError
{
    PARSER_NO_ERROR,
    PARSER_EXPECTED_VAR,
    PARSER_EXPECTED_IDENTIFIER,
    PARSER_EXPECTED_NUMBER,
    PARSER_EXPECTED_EQUAL,
    PARSER_EXPECTED_EXPRESSION,
    PARSER_EXPECTED_SEMICOLON,
    PARSER_EXPECTED_PRINT,
    PARSER_UNEXPECTED_TOKEN
};
enum ExpressionType
{
    EXPR_NUMBER,
    EXPR_IDENTIFIER,
    EXPR_BINARY,
    EXP_PRINT
};
enum StatementType
{
    STMT_VAR_DECLARATION,
    STMT_PRINT
};
struct Parser
{
    struct Lexer *lexer;
    struct Token current;
    struct Token previous;
    enum ParserError error;
};
struct Expression;
struct BinaryExpression
{
    struct Expression *left;
    enum TokenType operator;
    struct Expression *right;
};
struct PrintStatement
{
    struct Expression *value;
};

struct Expression
{
    enum ExpressionType type;
    union Data
    {
        int number;
        char identifier[100];
        struct BinaryExpression binary;
    } data;
};
struct VariableDeclaration
{
    char name[100];
    struct Expression *value;
};
struct Statement
{
    struct PrintStatement *printSt;
    struct VariableDeclaration *declaration;
    enum StatementType type;
};

struct Program
{
    struct Statement *statements[100];
    int statementCount;
};

void initializeParser(struct Parser *parser, struct Lexer *lexer);
void parserAdvance(struct Parser *parser);

int checkToken(struct Parser *parser, enum TokenType type);
int matchToken(struct Parser *parser, enum TokenType type);
int consumeToken(struct Parser *parser, enum TokenType type, enum ParserError error);

struct Expression *parsePrimary(struct Parser *parser);
struct Expression *parseExpression(struct Parser *parser);
struct VariableDeclaration *parseDeclaration(struct Parser *parser);
struct Program *parseProgram(struct Parser *parser);
struct PrintStatement *parsePrintStatement(struct Parser *parser);
#endif