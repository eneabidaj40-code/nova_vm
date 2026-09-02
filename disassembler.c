#include "disassembler.h"
#include "bytecode.h"
#include<stdio.h>

void disassemble(int program[],int sizeProgram){
    for (int i = 0; i < sizeProgram; i++)
    {
        if (program[i]==ADD || program[i]==MUL || program[i]==SUB 
        || program[i]==PRINT || program[i]==EQUAL || program[i]==LESS_THAN 
        || program[i]==GREATER_THAN || program[i]==DIV || program[i]==RET || program[i]==HALT )
        {
            printf("%d: %s\n",i,opcode_to_string(program[i]));
        }
        else if (program[i]==PUSH || program[i]==JUMP || program[i]==JZ || program[i]==STORE 
            || program[i]==LOAD || program[i]==STORE_LOCAL || program[i]==LOAD_LOCAL)
        {
            if (i+1>=sizeProgram)
            {
                break;
            }
            printf("%d: %s %d\n",i,opcode_to_string(program[i]),program[i+1]);
            i++;
        }
        else if (program[i]==CALL)
        {
            if (i+2>=sizeProgram)
            {
                break;
            }
            printf("%d: %s %d %d\n",i,opcode_to_string(program[i]),program[i+1],program[i+2]);
            i+=2;
        }
        else
        {
            printf("%d: %s %d\n",i,opcode_to_string(program[i]),program[i]);
        }
    }
}