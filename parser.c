#include "parser.h"
#include <stdio.h>
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
int parsePrimary(struct Parser *parser)
{
    if (parser->current.type == TOKEN_NUMBER)
    {
        consumeToken(parser, TOKEN_NUMBER, PARSER_EXPECTED_NUMBER);
        return 1;
    }
    if (parser->current.type == TOKEN_IDENTIFIER)
    {
        consumeToken(parser, TOKEN_IDENTIFIER, PARSER_EXPECTED_IDENTIFIER);
        return 1;
    }
    else
    {
        parser->error = PARSER_EXPECTED_EXPRESSION;
    }
    return 0;
}
int parseExpression(struct Parser *parser)
{
    if (parsePrimary(parser) == 0)
    {
        return 0;
    }
    while (matchToken(parser, TOKEN_PLUS) == 1)
    {
        if (parsePrimary(parser) == 0)
        {
            return 0;
        }
    }
    return 1;
}
void parseDeclaration(struct Parser *parser)
{
    if (consumeToken(parser, TOKEN_VAR, PARSER_EXPECTED_VAR) != 1)
    {
        return;
    }
    if (consumeToken(parser, TOKEN_IDENTIFIER, PARSER_EXPECTED_IDENTIFIER) != 1)
    {
        return;
    }
    if (consumeToken(parser, TOKEN_EQUAL, PARSER_EXPECTED_EQUAL) != 1)
    {
        return;
    }
    if (parseExpression(parser) != 1)
    {
        return;
    }
    if (consumeToken(parser, TOKEN_SEMICOLON, PARSER_EXPECTED_SEMICOLON) != 1)
    {
        return;
    }
}
int parseProgram(struct Parser *parser)
{
    while (parser->current.type == TOKEN_VAR)
    {
        parseDeclaration(parser);
        if (parser->error != PARSER_NO_ERROR)
        {
            return 0;
        }
    }
    if (parser->current.type == TOKEN_EOF)
    {
        return 1;
    }
    else
    {
        parser->error = PARSER_UNEXPECTED_TOKEN;
        return 0;
    }
}