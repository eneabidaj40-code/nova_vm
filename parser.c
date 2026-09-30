#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void initializeParser(struct Parser *parser, struct Lexer *lexer)
{
    parser->lexer = lexer;
    parser->error = PARSER_NO_ERROR;
    parser->current = scanToken(parser->lexer);
}
void parserAdvance(struct Parser *parser)
{
    parser->previous = parser->current;
    parser->current = scanToken(parser->lexer);
}
int checkToken(struct Parser *parser, enum TokenType type)
{
    if (parser->current.type == type)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int matchToken(struct Parser *parser, enum TokenType type)
{
    if (checkToken(parser, type) == 1)
    {
        parserAdvance(parser);
        return 1;
    }
    else
    {
        return 0;
    }
}
int consumeToken(struct Parser *parser, enum TokenType type, enum ParserError error)
{
    if (parser->current.type == type)
    {
        parserAdvance(parser);
        return 1;
    }
    else
    {
        parser->error = error;
        return 0;
    }
}
struct Expression *parsePrimary(struct Parser *parser)
{
    struct Expression *expression = NULL;
    if (parser->current.type == TOKEN_NUMBER)
    {
        expression = (struct Expression *)malloc(sizeof(struct Expression));
        if (expression == NULL)
        {
            printf("Failure of accessing the memory\n");
            return NULL;
        }
        expression->type = EXPR_NUMBER;
        expression->data.number = atoi(parser->current.lexeme);

        consumeToken(parser, TOKEN_NUMBER, PARSER_EXPECTED_NUMBER);
        return expression;
    }

    if (parser->current.type == TOKEN_IDENTIFIER)
    {
        expression = (struct Expression *)malloc(sizeof(struct Expression));
        if (expression == NULL)
        {
            printf("Failure of accessing the memory\n");
            return NULL;
        }
        expression->type = EXPR_IDENTIFIER;
        strcpy(expression->data.identifier, parser->current.lexeme);
        consumeToken(parser, TOKEN_IDENTIFIER, PARSER_EXPECTED_IDENTIFIER);
        return expression;
    }

    else
    {
        parser->error = PARSER_EXPECTED_EXPRESSION;
    }
    return expression;
}
struct Expression *parseTerm(struct Parser *parser)
{
    struct Expression *left;
    struct Expression *right;
    struct Expression *binary;
    enum TokenType operator;

    left = parsePrimary(parser);
    if (left == NULL)
    {
        return NULL;
    }
    while (parser->current.type == TOKEN_SLASH || parser->current.type == TOKEN_STAR)
    {
        operator = parser->current.type;
        if (operator == TOKEN_SLASH)
        {
            matchToken(parser, TOKEN_SLASH);
        }
        if (operator == TOKEN_STAR)
        {
            matchToken(parser, TOKEN_STAR);
        }

        right = parsePrimary(parser);
        if (right == NULL)
        {
            return NULL;
        }
        binary = (struct Expression *)malloc(sizeof(struct Expression));
        if (binary == NULL)
        {
            printf("Failed accessing the memory\n");
            return NULL;
        }

        binary->type = EXPR_BINARY;
        binary->data.binary.left = left;
        binary->data.binary.operator = operator;
        binary->data.binary.right = right;

        left = binary;
    }
    return left;
}
struct Expression *parseExpression(struct Parser *parser)
{
    struct Expression *left;
    struct Expression *right;
    struct Expression *binary;
    enum TokenType operator;

    left = parsePrimary(parser);
    if (left == NULL)
    {
        return NULL;
    }
    while (parser->current.type == TOKEN_PLUS || parser->current.type == TOKEN_MINUS)
    {
        operator = parser->current.type;
        if (operator == TOKEN_PLUS)
        {
            matchToken(parser, TOKEN_PLUS);
        }
        if (operator == TOKEN_MINUS)
        {
            matchToken(parser, TOKEN_MINUS);
        }

        right = parseTerm(parser);
        if (right == NULL)
        {
            return NULL;
        }
        binary = (struct Expression *)malloc(sizeof(struct Expression));
        if (binary == NULL)
        {
            printf("Failed accessing the memory\n");
            return NULL;
        }

        binary->type = EXPR_BINARY;
        binary->data.binary.left = left;
        binary->data.binary.operator = operator;
        binary->data.binary.right = right;

        left = binary;
    }
    return left;
}
struct VariableDeclaration *parseDeclaration(struct Parser *parser)
{
    struct VariableDeclaration *var = NULL;
    struct Expression *value;
    char temp[100];

    if (consumeToken(parser, TOKEN_VAR, PARSER_EXPECTED_VAR) != 1)
    {
        return NULL;
    }

    strcpy(temp, parser->current.lexeme);
    if (consumeToken(parser, TOKEN_IDENTIFIER, PARSER_EXPECTED_IDENTIFIER) != 1)
    {
        return NULL;
    }

    if (consumeToken(parser, TOKEN_EQUAL, PARSER_EXPECTED_EQUAL) != 1)
    {
        return NULL;
    }

    value = parseExpression(parser);
    if (value == NULL)
    {
        return NULL;
    }

    if (consumeToken(parser, TOKEN_SEMICOLON, PARSER_EXPECTED_SEMICOLON) != 1)
    {
        return NULL;
    }

    var = (struct VariableDeclaration *)malloc(sizeof(struct VariableDeclaration));
    if (var == NULL)
    {
        printf("Cannot access the memory\n");
        return NULL;
    }
    strcpy(var->name, temp);
    var->value = value;

    return var;
}
struct PrintStatement *parsePrintStatement(struct Parser *parser)
{
    struct Expression *value = NULL;

    if (consumeToken(parser, TOKEN_PRINT, PARSER_EXPECTED_PRINT) != 1)
    {
        return NULL;
    }
    value = parseExpression(parser);
    if (value == NULL)
    {
        return NULL;
    }
    if (consumeToken(parser, TOKEN_SEMICOLON, PARSER_EXPECTED_SEMICOLON) != 1)
    {
        return NULL;
    }
    struct PrintStatement *print;
    print = (struct PrintStatement *)malloc(sizeof(struct PrintStatement));
    if (print == NULL)
    {
        return NULL;
    }

    print->value = value;
    return print;
}
struct Statement *parseStatement(struct Parser *parser)
{
    struct Statement *statement = NULL;
    if (parser->current.type == TOKEN_VAR)
    {
        statement = (struct Statement *)malloc(sizeof(struct Statement));
        if (statement == NULL)
        {
            return NULL;
        }
        statement->declaration = parseDeclaration(parser);
        if (statement->declaration == NULL)
        {
            return NULL;
        }

        statement->type = STMT_VAR_DECLARATION;
        statement->printSt = NULL;
    }
    else if (parser->current.type == TOKEN_PRINT)
    {
        statement = (struct Statement *)malloc(sizeof(struct Statement));
        if (statement == NULL)
        {
            return NULL;
        }
        statement->printSt = parsePrintStatement(parser);
        if (statement->printSt == NULL)
        {
            return NULL;
        }
        statement->type = STMT_PRINT;
        statement->declaration = NULL;
    }
    return statement;
}
struct Program *parseProgram(struct Parser *parser)
{
    struct Program *astProgram = NULL;
    astProgram = (struct Program *)malloc(sizeof(struct Program));
    if (astProgram == NULL)
    {
        printf("Cannot access the memory\n");
        return NULL;
    }
    astProgram->statementCount = 0;

    while (parser->current.type == TOKEN_VAR || parser->current.type == TOKEN_PRINT)
    {
        struct Statement *statement;
        statement = parseStatement(parser);
        if (statement == NULL)
        {
            return NULL;
        }
        astProgram->statements[astProgram->statementCount] = statement;
        astProgram->statementCount++;
    }
    if (parser->current.type == TOKEN_EOF)
    {
        return astProgram;
    }
    else
    {
        parser->error = PARSER_UNEXPECTED_TOKEN;
        return NULL;
    }
}
