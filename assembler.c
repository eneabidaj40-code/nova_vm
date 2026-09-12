#include "assembler.h"
#include "bytecode.h"
#define MAX_LABELS 100
#include <string.h>
#include <stdlib.h>

struct Label
{
    char name[MAX_LINE_LENGTH];
    int address;
};

enum AssemblerError
{
    ASM_NO_ERROR,
    ASM_UNKNOWN_LABEL,
    ASM_UNKNOWN_OPCODE,
    ASM_INVALID_OPERAND_COUNT,
    ASM_DUPLICATE_LABEL,
    ASM_TOO_MANY_LABELS,
    ASM_TOO_MANY_TOKENS,
    ASM_INVALID_LABEL,
    ASM_BYTECODE_OVERFLOW
};

enum FileError
{
    FILE_NO_ERROR,
    FILE_OPEN_ERROR,
    FILE_LINE_TOO_LONG,
    FILE_TOO_MANY_LINES
};

struct FileStatus
{
    enum FileError error;
    int line;
};
struct Assembler
{
    struct Label labels[MAX_LABELS];
    int labelCount;
    int currentAddress;
    int byteCodeSize;
    enum AssemblerError error;
    char errorToken[MAX_LINE_LENGTH];
};

int string_to_opcode(char *string)
{
    for (int i = 0; OPCODE_NAMES[i] != NULL; i++)
    {
        if (strcmp(string, OPCODE_NAMES[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int get_operand_count(int opcode)
{
    switch (opcode)
    {
    case ADD:
    case MUL:
    case SUB:
    case PRINT:
    case EQUAL:
    case LESS_THAN:
    case GREATER_THAN:
    case DIV:
    case RET:
    case HALT:
        return 0;

    case PUSH:
    case JUMP:
    case JZ:
    case STORE:
    case LOAD:
    case STORE_LOCAL:
    case LOAD_LOCAL:
        return 1;

    case CALL:
        return 2;

    default:
        return -1;
    }
}

int tokenize(char *line, char tokens[3][MAX_LINE_LENGTH], struct Assembler *assembler)
{
    int tokenIndex = 0;
    int tokenCnt = 0;
    int tokenCount = 0;
    int insideToken = 0;

    for (int j = 0; line[j] != '\0'; j++)
    {
        if (line[j] == ' ' || line[j] == '\t')
        {
            if (insideToken)
            {
                tokens[tokenIndex][tokenCnt] = '\0';
                tokenCount++;
                tokenIndex++;
                tokenCnt = 0;
                insideToken = 0;
            }
        }
        else
        {
            if (tokenIndex >= 3)
            {
                assembler->error = ASM_TOO_MANY_TOKENS;
                break;
            }
            tokens[tokenIndex][tokenCnt] = line[j];
            tokenCnt++;
            insideToken = 1;
        }
    }

    if (insideToken)
    {
        tokens[tokenIndex][tokenCnt] = '\0';
        tokenCount++;
    }

    return tokenCount - 1;
}

int find_label_address(char *name, struct Assembler *assembler)
{
    for (int i = 0; i < assembler->labelCount; i++)
    {
        if (strcmp(name, assembler->labels[i].name) == 0)
        {
            return assembler->labels[i].address;
        }
    }

    return -1;
}

void resolve_operand(char *operand, int program[], struct Assembler *assembler)
{
    char *endptr;
    int value = strtol(operand, &endptr, 0);

    if (endptr != operand && *endptr == '\0')
    {
        program[assembler->byteCodeSize] = value;
        assembler->byteCodeSize += 1;
    }
    else
    {
        int address = find_label_address(operand, assembler);

        if (address == -1)
        {
            strcpy(assembler->errorToken, operand);
            assembler->error = ASM_UNKNOWN_LABEL;
            return;
        }

        program[assembler->byteCodeSize] = address;
        assembler->byteCodeSize += 1;
    }
}

int load_source_file(char *filename, char source[][MAX_LINE_LENGTH], int maxLines, struct FileStatus *status)
{
    status->error = FILE_NO_ERROR;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        status->error = FILE_OPEN_ERROR;
        return -1;
    }

    char line[MAX_LINE_LENGTH];
    int row = 0;

    while (row < maxLines && fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        int length = strlen(line);

        if (length > 0 && line[length - 1] != '\n')
        {
            int ch = fgetc(fp);

            if (ch != '\n' && ch != EOF)
            {
                while (ch != '\n' && ch != EOF)
                {
                    ch = fgetc(fp);
                }

                status->error = FILE_LINE_TOO_LONG;
                status->line = row + 1;
                fclose(fp);
                return -1;
            }
        }

        line[strcspn(line, "\r\n")] = '\0';
        strcpy(source[row], line);
        row++;
    }

    if (row > maxLines)
    {
        int ch = fgetc(fp);

        if (ch != EOF)
        {
            status->error = FILE_TOO_MANY_LINES;
            status->line = row + 1;
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return row;
}

int is_blank_line(char *line)
{
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] != ' ' && line[i] != '\t')
        {
            return 0;
        }
    }
    return 1;
}

void remove_comment(char *line)
{
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == '#')
        {
            line[i] = '\0';
            return;
        }
    }
}

int label_exists(char *name, struct Assembler *assembler)
{
    for (int i = 0; i < assembler->labelCount; i++)
    {
        if (strcmp(name, assembler->labels[i].name) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int valid_label_name(char *label)
{
    int length = strlen(label);
    int i;

    if ((label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z') || label[0] == '_')
    {
        for (i = 1; i < length; i++)
        {
            if ((label[i] >= 'a' && label[i] <= 'z') || (label[i] >= 'A' && label[i] <= 'Z') ||
                label[i] == '_' || (label[i] >= '0' && label[i] <= '9'))
            {
                continue;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        return 0;
    }

    return 1;
}

void assembler_error_reporting(enum AssemblerError error, struct Assembler *error_assembler)
{
    switch (error)
    {
    case ASM_UNKNOWN_LABEL:
        printf("\nNovaVM Assembler Error: Unknown label %s\n", error_assembler->errorToken);
        break;

    case ASM_UNKNOWN_OPCODE:
        printf("\nNovaVM Assembler Error: Unknown opcode %s\n", error_assembler->errorToken);
        break;

    case ASM_INVALID_OPERAND_COUNT:
        printf("\nNovaVM Assembler Error: Invalid operand count %s\n", error_assembler->errorToken);
        break;

    case ASM_DUPLICATE_LABEL:
        printf("\nNovaVM Assembler Error: Duplicate label %s\n", error_assembler->errorToken);
        break;

    case ASM_TOO_MANY_LABELS:
        printf("\nNovaVM Assembler Error: Too many labels\n");
        break;

    case ASM_TOO_MANY_TOKENS:
        printf("\nNovaVM Assembler Error: Too many tokens\n");
        break;

    case ASM_INVALID_LABEL:
        printf("\nNovaVM Assembler Error: Invalid label %s\n", error_assembler->errorToken);
        break;

    case ASM_BYTECODE_OVERFLOW:
        printf("\nNovaVM Assembler Error: Bytecode overflow\n");
        break;

    default:
        break;
    }
}
void file_error_reporting(struct FileStatus *status)
{
    switch (status->error)
    {
    case FILE_OPEN_ERROR:
        printf("NovaVM File Error: Cannot open source file\n");
        break;

    case FILE_LINE_TOO_LONG:
        printf("NovaVM File Error: Line %d is too long\n", status->line);
        break;

    case FILE_TOO_MANY_LINES:
        printf("NovaVM File Error: Source file has too many lines\n");
        break;

    case FILE_NO_ERROR:
        break;

    default:
        break;
    }
}
int assemble(char source[][MAX_LINE_LENGTH], int lines, int program[], int MAX_BYTECODE, int *byteCodeSize)
{
    char tokens[3][MAX_LINE_LENGTH];

    struct Assembler assembler;
    struct FileStatus status;

    assembler.labelCount = 0;
    assembler.currentAddress = 0;
    assembler.byteCodeSize = 0;
    assembler.error = ASM_NO_ERROR;

    for (int i = 0; i < lines; i++)
    {

        remove_comment(source[i]);

        if (is_blank_line(source[i]))
        {
            continue;
        }

        int operandCount = tokenize(source[i], tokens, &assembler);

        if (assembler.error != ASM_NO_ERROR)
        {
            break;
        }

        int opcode = string_to_opcode(tokens[0]);

        if (opcode != -1)
        {
            assembler.currentAddress += get_operand_count(opcode) + 1;
        }
        else
        {
            if (operandCount == 0 && tokens[0][strlen(tokens[0]) - 1] == ':')
            {
                if (assembler.labelCount >= MAX_LABELS)
                {
                    assembler.error = ASM_TOO_MANY_LABELS;
                    break;
                }

                strcpy(assembler.labels[assembler.labelCount].name, source[i]);
                int len = strlen(assembler.labels[assembler.labelCount].name);

                if (len > 0)
                {
                    assembler.labels[assembler.labelCount].name[len - 1] = '\0';
                }

                if (valid_label_name(assembler.labels[assembler.labelCount].name) != 1)
                {
                    assembler.error = ASM_INVALID_LABEL;
                    strcpy(assembler.errorToken, assembler.labels[assembler.labelCount].name);
                    break;
                }

                if (label_exists(assembler.labels[assembler.labelCount].name, &assembler))
                {
                    assembler.error = ASM_DUPLICATE_LABEL;
                    strcpy(assembler.errorToken, assembler.labels[assembler.labelCount].name);
                    break;
                }

                assembler.labels[assembler.labelCount].address = assembler.currentAddress;
                assembler.labelCount++;
            }
            else
            {
                assembler.error = ASM_UNKNOWN_OPCODE;
                strcpy(assembler.errorToken, tokens[0]);
                break;
            }
        }
    }

    if (assembler.error != ASM_NO_ERROR)
    {
        assembler_error_reporting(assembler.error, &assembler);
        return 0;
    }

    else
    {

        for (int i = 0; i < lines; i++)
        {
            if (is_blank_line(source[i]))
            {
                continue;
            }

            int operandCount = tokenize(source[i], tokens, &assembler);
            int opcode = string_to_opcode(tokens[0]);

            if (assembler.byteCodeSize + operandCount + 1 > MAX_BYTECODE)
            {
                assembler.error = ASM_BYTECODE_OVERFLOW;
                break;
            }

            if (opcode != -1)
            {
                if (operandCount != get_operand_count(opcode))
                {
                    assembler.error = ASM_INVALID_OPERAND_COUNT;
                    strcpy(assembler.errorToken, tokens[0]);
                    break;
                }

                int opCount = get_operand_count(opcode);

                if (opCount == 0)
                {
                    program[assembler.byteCodeSize] = opcode;
                    assembler.byteCodeSize += 1;
                }

                if (opCount == 1)
                {
                    program[assembler.byteCodeSize] = opcode;
                    assembler.byteCodeSize += 1;

                    resolve_operand(tokens[1], program, &assembler);

                    if (assembler.error != ASM_NO_ERROR)
                    {
                        break;
                    }
                }

                if (opCount == 2)
                {
                    program[assembler.byteCodeSize] = opcode;
                    assembler.byteCodeSize += 1;

                    resolve_operand(tokens[1], program, &assembler);

                    if (assembler.error != ASM_NO_ERROR)
                    {
                        break;
                    }

                    resolve_operand(tokens[2], program, &assembler);

                    if (assembler.error != ASM_NO_ERROR)
                    {
                        break;
                    }
                }
            }
        }
    }

    if (assembler.error != ASM_NO_ERROR)
    {
        assembler_error_reporting(assembler.error, &assembler);
        return 0;
    }
    if (status.error != FILE_NO_ERROR)
    {
        file_error_reporting(&status);
        return 0;
    }

    *byteCodeSize = assembler.byteCodeSize;
    return 1;
}