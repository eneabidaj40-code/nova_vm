#include "assembler.h"
#include "bytecode.h"
#define MAX_LABELS 100
#include<string.h>
#include<stdlib.h>

struct Label{
    char name[MAX_LINE_LENGTH];
    int address;
};

enum AssemblerError{
    ASM_NO_ERROR,
    ASM_UNKNOWN_LABEL
};

struct Assembler{
    struct Label labels[MAX_LABELS];
    int labelCount;
    int currentAddress;
    int byteCodeSize;
    enum AssemblerError error;
    char errorToken[MAX_LINE_LENGTH];
};

int string_to_opcode(char *string){
    for(int i=0;OPCODE_NAMES[i]!=NULL;i++)
    {
        if(strcmp(string,OPCODE_NAMES[i])==0)
        {
            return i;
        }
    }
    return -1;
}

int get_operand_count(int opcode){
    switch(opcode)
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

void tokenize(char *line,char tokens[3][MAX_LINE_LENGTH]){
    int tokenIndex=0;
    int tokenCnt=0;

    for(int j=0;line[j]!='\0';j++)
    {
        if(line[j]!=' ')
        {
            tokens[tokenIndex][tokenCnt]=line[j];
            tokenCnt++;
        }
        else
        {
            tokens[tokenIndex][tokenCnt]='\0';
            tokenIndex++;
            tokenCnt=0;
        }
    }

    tokens[tokenIndex][tokenCnt]='\0';
}

int find_label_address(char *name,struct Assembler *assembler){
    for(int i=0;i<assembler->labelCount;i++)
    {
        if(strcmp(name,assembler->labels[i].name)==0)
        {
            return assembler->labels[i].address;
        }
    }

    return -1;
}

void resolve_operand(char *operand,int program[],struct Assembler *assembler){
    char *endptr;
    int value=strtol(operand,&endptr,0);

    if(endptr!=operand && *endptr=='\0')
    {
        program[assembler->byteCodeSize]=value;
        assembler->byteCodeSize+=1;
    }
    else
    {
        int address=find_label_address(operand,assembler);

        if(address==-1)
        {
            strcpy(assembler->errorToken,operand);
            assembler->error=ASM_UNKNOWN_LABEL;
            return;
        }

        program[assembler->byteCodeSize]=address;
        assembler->byteCodeSize+=1;
    }
}
void assembler_error_reporting(enum AssemblerError error,struct Assembler *error_assembler){
    switch (error)
    {
    case ASM_UNKNOWN_LABEL:
        printf("NovaVM Assembler Error: Unknown label %s",error_assembler->errorToken);
        break;
    
    default:
        break;
    }
}
void assemble(char source[][MAX_LINE_LENGTH],int lines,int program[],int *byteCodeSize){
    char tokens[3][MAX_LINE_LENGTH];

    struct Assembler assembler;

    assembler.labelCount=0;
    assembler.currentAddress=0;
    assembler.byteCodeSize=0;
    assembler.error=ASM_NO_ERROR;

    for(int i=0;i<lines;i++)
    {
        int isLabel=0;

        for(int j=0;source[i][j]!='\0';j++)
        {
            if(source[i][j]==':')
            {
                isLabel=1;
                break;
            }
        }

        if(isLabel)
        {
            strcpy(assembler.labels[assembler.labelCount].name,source[i]);

            int len=strlen(assembler.labels[assembler.labelCount].name);

            if(len>0)
            {
                assembler.labels[assembler.labelCount].name[len-1]='\0';
            }

            assembler.labels[assembler.labelCount].address=assembler.currentAddress;
            assembler.labelCount++;
        }
        else
        {
            tokenize(source[i],tokens);

            int opcode=string_to_opcode(tokens[0]);

            if(opcode!=-1)
            {
                assembler.currentAddress+=get_operand_count(opcode)+1;
            }
        }
    }

    for(int i=0;i<lines;i++)
    {
        tokenize(source[i],tokens);

        int opcode=string_to_opcode(tokens[0]);

        if(opcode!=-1)
        {
            int opCount=get_operand_count(opcode);

            if(opCount==0)
            {
                program[assembler.byteCodeSize]=opcode;
                assembler.byteCodeSize+=1;
            }

            if(opCount==1)
            {
                program[assembler.byteCodeSize]=opcode;
                assembler.byteCodeSize+=1;

                resolve_operand(tokens[1],program,&assembler);

                if(assembler.error!=ASM_NO_ERROR)
                {
                    break;
                }
            }

            if(opCount==2)
            {
                program[assembler.byteCodeSize]=opcode;
                assembler.byteCodeSize+=1;

                resolve_operand(tokens[1],program,&assembler);

                if(assembler.error!=ASM_NO_ERROR)
                {
                    break;
                }

                resolve_operand(tokens[2],program,&assembler);

                if(assembler.error!=ASM_NO_ERROR)
                {
                    break;
                }
            }
        }
    }

    *byteCodeSize=assembler.byteCodeSize;
    if(assembler.error!=ASM_NO_ERROR)
{
    assembler_error_reporting(assembler.error,&assembler);
}
}