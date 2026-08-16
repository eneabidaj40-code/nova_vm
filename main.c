#include<stdio.h>
enum Opcode{
    PUSH,
    ADD,
    MUL,
    SUB,
    PRINT,
    HALT
};
int main(){

    int program[]={PUSH,10,PUSH,3,SUB,PUSH,4,MUL,PRINT,HALT};
    
    int ip=0;
    int intruc=program[ip];

    /*now we have to build the stack pointer to know where the top of the stack is 
    and the integer array capable to play the role as an stack */
    
    int stack_array[100];
    int sp=0;

    //creating an running VM variable 
    int executing=1;
    
    while(executing!=0)
    {
        intruc=program[ip];

        switch (intruc)
    {
        case PUSH:
          stack_array[sp]=program[ip+1];
          sp++;
          ip=ip+2;
        break;

        case ADD:
          sp--;
          int temp=stack_array[sp];
          sp--;
          int temp1=stack_array[sp];
          stack_array[sp]=temp+temp1;
          sp++;
          ip++;
        break;

        case MUL:
          sp--;
          temp=stack_array[sp];
          sp--;
          temp1=stack_array[sp];
          stack_array[sp]=temp*temp1;
          sp++;
          ip++;
        break;

        case SUB:
          sp--;
          int right=stack_array[sp];
          sp--;
          int left=stack_array[sp];
          stack_array[sp]=left-right;
          sp++;
          ip++;
        break;

        case PRINT:
         printf("VAlue is : %d",stack_array[sp-1]);
         ip++;
        break;

        case HALT:
          executing=0;
        break;

        default:
        break;
    }

}
    
    return 0;
}