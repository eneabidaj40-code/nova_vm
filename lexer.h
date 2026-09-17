#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#define MAX_LEXEME_LENGTH 64
#define MAX_SOURCE_LENGTH 4096

enum TokenType
{
    TOKEN_VAR,
    TOKEN_IDENTIFIER,
    TOKEN_EQUAL,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_ERROR
};
struct Token
{
    enum TokenType type;
    char lexeme[MAX_LEXEME_LENGTH];
    int line;
};
struct Lexer
{
    char *source;
    int start;
    int current;
    int line;
};
void initializeLexer(struct Lexer *lexer, char *sourceBuffer);
char advance(struct Lexer *lexer);
char lexerPeek(struct Lexer *lexer);
int isAtEnd(struct Lexer *lexer);
int readFile(FILE *file_nova, char *bufferMemory);
struct Token scanToken(struct Lexer *lexer);

#endif