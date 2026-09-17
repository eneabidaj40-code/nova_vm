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
    PARSER_UNEXPECTED_TOKEN
};
struct Parser
{
    struct Lexer *lexer;
    struct Token current;
    struct Token previous;
    enum ParserError error;
};
void initializeParser(struct Parser *parser, struct Lexer *lexer);
void parserAdvance(struct Parser *parser);

int checkToken(struct Parser *parser, enum TokenType type);
int matchToken(struct Parser *parser, enum TokenType type);
int consumeToken(struct Parser *parser, enum TokenType type, enum ParserError error);

int parsePrimary(struct Parser *parser);
int parseExpression(struct Parser *parser);
void parseDeclaration(struct Parser *parser);
#endif