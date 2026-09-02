#ifndef ASSEMBLER_H
#define ASSEMBLER_H
#define MAX_LINE_LENGTH 30
#include "bytecode.h"
#include<stdio.h>
#include<string.h>

void assemble(char source[][MAX_LINE_LENGTH],int lines,int program[],int *byteCodeSize);

#endif