#include <stdio.h>
#include <string.h>
#include "lexer.h"
void initializeLexer(struct Lexer *lexer, char *sourceBuffer)
{
    lexer->source = sourceBuffer;
    lexer->start = 0;
    lexer->current = 0;
    lexer->line = 1;
}
char advance(struct Lexer *lexer) // inspect and consume
{
    return lexer->source[lexer->current++];
}

char lexerPeek(struct Lexer *lexer) // inspect only
{
    return lexer->source[lexer->current];
}

int isAtEnd(struct Lexer *lexer)
{
    if (lexer->source[lexer->current] == '\0')
    {
        return 1;
    }
    return 0;
}

int readFile(FILE *file_nova, char *bufferMemory)
{
    char line[256];

    int sourceLength = 0;
    int remaining_space;
    bufferMemory[0] = '\0';

    while (fgets(line, sizeof(line), file_nova) != NULL)
    {
        remaining_space = MAX_SOURCE_LENGTH - sourceLength;
        if (remaining_space <= 0 || strlen(line) >= remaining_space)
        {
            break;
        }
        else
        {
            strcpy(&bufferMemory[sourceLength], line);
            sourceLength += strlen(line);
        }
    }
    fclose(file_nova);
    return 1;
}
struct Token scanToken(struct Lexer *lexer)
{
    struct Token token;
    char lex[100];
    int i = 0;

    char inspect = lexerPeek(lexer);

    while (inspect == ' ' || inspect == '\t' || inspect == '\n')
    {
        if (inspect == '\n')
        {
            lexer->line++;
        }

        advance(lexer);
        inspect = lexerPeek(lexer);
    }

    if (isAtEnd(lexer) == 1)
    {
        token.type = TOKEN_EOF;
        token.lexeme[0] = '\0';
        token.line = lexer->line;
        return token;
    }

    lexer->start = lexer->current;

    if ((inspect >= 'a' && inspect <= 'z') || inspect == '_' || (inspect >= 'A' && inspect <= 'Z'))
    {
        while (inspect >= 'a' && inspect <= 'z' || inspect == '_' || (inspect >= 'A' && inspect <= 'Z') || (inspect >= '0' && inspect <= '9'))
        {
            lex[i] = advance(lexer);
            inspect = lexerPeek(lexer);
            i++;
        }
        lex[i] = '\0';
        if (strcmp(lex, "var") == 0)
        {
            token.type = TOKEN_VAR;
            strcpy(token.lexeme, lex);
        }
        else if (strcmp(lex, "print") == 0)
        {
            token.type = TOKEN_PRINT;
            strcpy(token.lexeme, lex);
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
            strcpy(token.lexeme, lex);
        }

        token.line = lexer->line;
        return token;
    }

    if (inspect >= '0' && inspect <= '9')
    {
        while (inspect >= '0' && inspect <= '9')
        {
            lex[i] = advance(lexer);
            inspect = lexerPeek(lexer);
            i++;
        }
        lex[i] = '\0';
        token.type = TOKEN_NUMBER;
        strcpy(token.lexeme, lex);

        token.line = lexer->line;
        return token;
    }

    if (inspect == '=')
    {
        advance(lexer);
        if (lexerPeek(lexer) == '=')
        {
            advance(lexer);
            token.type = TOKEN_EQUAL_EQUAL;
            strcpy(token.lexeme, "==");
            token.line = lexer->line;
        }
        else
        {
            token.type = TOKEN_EQUAL;
            strcpy(token.lexeme, "=");
            token.line = lexer->line;
        }
        return token;
    }
    if (inspect == '<')
    {
        advance(lexer);
        token.type = TOKEN_LESS;
        strcpy(token.lexeme, "<");
        token.line = lexer->line;
        return token;
    }
    if (inspect == '>')
    {
        advance(lexer);
        token.type = TOKEN_GREATER;
        strcpy(token.lexeme, ">");
        token.line = lexer->line;
        return token;
    }
    if (inspect == '+')
    {
        advance(lexer);
        token.type = TOKEN_PLUS;
        strcpy(token.lexeme, "+");
        token.line = lexer->line;
        return token;
    }
    if (inspect == '-')
    {
        advance(lexer);
        token.type = TOKEN_MINUS;
        strcpy(token.lexeme, "-");
        token.line = lexer->line;
        return token;
    }
    if (inspect == '*')
    {
        advance(lexer);
        token.type = TOKEN_STAR;
        strcpy(token.lexeme, "*");
        token.line = lexer->line;
        return token;
    }
    if (inspect == '/')
    {
        advance(lexer);
        token.type = TOKEN_SLASH;
        strcpy(token.lexeme, "/");
        token.line = lexer->line;
        return token;
    }
    if (inspect == ';')
    {
        advance(lexer);
        token.type = TOKEN_SEMICOLON;
        strcpy(token.lexeme, ";");
        token.line = lexer->line;
        return token;
    }
    if (isAtEnd(lexer) == 1)
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, '\0');
    }
    if (inspect)
    {
        advance(lexer);
        token.type = TOKEN_ERROR;
        token.lexeme[0] = inspect;
        token.lexeme[1] = '\0';
        token.line = lexer->line;
        return token;
    }
}
