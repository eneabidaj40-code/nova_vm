#include <stdio.h>
#include "lexer.h"
#define MAX_SOURCE_LENGTH 4096
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

char peek(struct Lexer *lexer) // inspect only
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
    file_nova = fopen("file.nova", "r");
    if (file_nova == NULL)
    {
        printf("Cannot open the file\n");
        return 0;
    }

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
int scanToken(struct Lexer *lexer)
{
    struct Token token;
    char inspect = peek(lexer->source[lexer->current]);

    if (inspect > 'a' && inspect < 'z')
    {
        char lex[100];
        int i = 0;
        char consume_char = advance(lexer->source[lexer->current]);

        while (consume_char > 'a' && consume_char < 'z' || consume_char == '_')
        {
            lex[i] = consume_char;
            consume_char = advance(lexer->source[lexer->current]);
            i++;
        }
        if (strcmp(lex, "var") == 0)
        {
            token.type = TOKEN_VAR;
        }
        else
        {
            token.type = TOKEN_IDENTIFIER;
        }
    }

    if (inspect == '\n')
    {
        lexer->line++;
    }
    if (inspect == '=')
    {
        token.type = TOKEN_EQUAL;
    }
    if (inspect == '+')
    {
        token.type = TOKEN_PLUS;
    }
    if (inspect == ';')
    {
        token.type = TOKEN_SEMICOLON;
    }
}
