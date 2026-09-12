#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#define MAX_LINE_LENGTH 30
#include "bytecode.h"
#include <stdio.h>
#include <string.h>
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

int assemble(char source[][MAX_LINE_LENGTH], int lines, int program[], int maxBytecode, int *byteCodeSize);
int load_source_file(char *filename, char source[][MAX_LINE_LENGTH], int maxLines, struct FileStatus *status);
void file_error_reporting(struct FileStatus *status);
#endif