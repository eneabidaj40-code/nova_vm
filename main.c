#include<stdio.h>
enum Error{
  NO_ERROR,
  STACK_UNDERFLOW,
  STACK_OVERFLOW,
  INVALID_OPCODE
};
struct VM
{
  int ip;
  int sp;
  int stack[100];
  int executing;
  enum Error error;
};

enum Opcode{
    PUSH,
    ADD,
    MUL,
    SUB,
    PRINT,
    HALT
};

void errorReporting(int vm_error){
  switch (vm_error)
  {
  case STACK_UNDERFLOW:{
    printf("\n NovaVM Runtime Error: Stack underflow \n Not enough values on the stack to execute this instruction.\n");
    break;
  }
  case STACK_OVERFLOW:{
    printf("\nNovaVM Runtime Error: Stack overflow \nThe stack has reached its maximum capacity.\n");
    break;
  }
  case INVALID_OPCODE:{
    printf("\nNovaVM Runtime Error: Invalid opcode\nThe VM encountered an unknown instruction.\n");
    break;
  }
  default:
    break;
  }
}
int main(){
   
  int program[]={PUSH,10,99,HALT};
  
  struct VM vm;

  //assigning the virtual machine to the error state which for now is no error accured
  vm.error=NO_ERROR;

  vm.ip=0;
  int intruc=program[vm.ip];

  /*now we have to build the stack pointer to know where the top of the stack is 
  and the integer array capable to play the role as an stack */
    
  vm.sp=0;

  //creating an running VM variable 
  vm.executing=1;
    
  while(vm.executing!=0)
    {
      intruc=program[vm.ip];
      switch (intruc)
    {
      
        case PUSH:{
          if (vm.sp>=100)
          {
            vm.error=STACK_OVERFLOW;
            vm.executing=0;
          }
          else{
          vm.stack[vm.sp]=program[(vm.ip)+1];
          vm.sp++;
          vm.ip+=2;
          }
        break;
        }
      
        case ADD :{
          if(vm.sp<2){
            vm.error=STACK_UNDERFLOW;
            vm.executing=0;
          }
          else{
          vm.sp--;
          int temp=vm.stack[vm.sp];

          vm.sp--;
          int temp1=vm.stack[vm.sp];

          vm.stack[vm.sp]=temp+temp1;
          vm.sp++;
          vm.ip++;
          }
        break;
        }

        case MUL:{
           if(vm.sp<2){
            vm.error=STACK_UNDERFLOW;
            vm.executing=0;
          }
          else{
          vm.sp--;
          int temp=vm.stack[vm.sp];

          vm.sp--;
          int temp1=vm.stack[vm.sp];

          vm.stack[vm.sp]=temp*temp1;
          vm.sp++;
          vm.ip++;
          }
        break;
        }

        case SUB:{
          if (vm.sp<2)
          {
            vm.error=STACK_UNDERFLOW;
            vm.executing=0;
          }
          else{
          vm.sp--;
          int right=vm.stack[vm.sp];

          vm.sp--;
          int left=vm.stack[vm.sp];

          vm.stack[vm.sp]=left-right;
          vm.sp++;
          vm.ip++;
          }
        break;
        }

        case PRINT:{
        if(vm.sp<1){
        vm.error=STACK_UNDERFLOW;
        vm.executing = 0;
        }

        else{
        printf("Value is: %d\n",vm.stack[vm.sp-1]);
        vm.ip++;
        }
        break;
      }
        

        case HALT:{
          vm.executing=0;
        break;
        }

        default:{
        vm.error = INVALID_OPCODE;
        vm.executing = 0;
        break;
        }
    }

  }
  //reporting the error that can happen during the execution of the instructions
  errorReporting(vm.error);
  
  return 0;
}