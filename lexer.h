#ifndef LEXER_H
#define LEXER_H
#define MAX_LEXEME_LENGTH 64

enum TokenType
{
    TOKEN_VAR,
    TOKEN_VAR,
    TOKEN_IDENTIFIER,
    TOKEN_EQUAL,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_SEMICOLON
};
struct Token
{
    enum TokenType type;
    char lexeme[MAX_LEXEME_LENGTH];
};
struct Lexer
{
    char *source;
    int start;
    int current;
    int line;
};

#endif