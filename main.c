#include<stdio.h>
#define MEMORY_CAPACITY 1024
enum Error{
  NO_ERROR,
  STACK_UNDERFLOW,
  STACK_OVERFLOW,
  INVALID_OPCODE,
  INSTRUCTION_OUT_OF_BOUNDS,
  DIVISION_BY_ZERO,
  MEMORY_OUT_OF_BOUNDS
};
struct VM
{
  //ip stands for instruction pointer
  int ip;
  int sp;
  int stack[100];
  int executing;
  int memory[MEMORY_CAPACITY];
  enum Error error;
};

enum Opcode{
    PUSH,
    ADD,
    MUL,
    SUB,
    PRINT,
    JUMP,
    JZ,//jump to zero 
    EQUAL,
    LESS_THAN,
    GREATER_THAN,
    DIV,//this is added to handle division by zero
    STORE,
    HALT
};

void initializeVM(struct VM *virtual_machine){
  virtual_machine->ip=0;
  virtual_machine->sp=0;
  //creating an running VM variable 
  virtual_machine->executing=1;
  //assigning the virtual machine to the error state which for now is no error accured
  virtual_machine->error=NO_ERROR;
}
//poping the values out of the stack  
int pop(struct VM *virtual_machine){

  if (virtual_machine->sp<1)
  {
    virtual_machine->error=STACK_UNDERFLOW;
    virtual_machine->executing=0;
    return 0;
  }

  virtual_machine->sp--;
  return virtual_machine->stack[virtual_machine->sp];

}
//pushing the values into the stack
void push(struct VM *vm,int value){
  if (vm->sp>=100)
    {
     vm->error=STACK_OVERFLOW;
     vm->executing=0;
    }
    else{
      vm->stack[vm->sp]=value;
      vm->sp++;
    }

}
//returning the top value of the stack 
int peek(struct VM *vm){
  if(vm->sp<1){
    vm->error=STACK_UNDERFLOW;
    vm->executing=0;
    return 0;
  }
  return vm->stack[vm->sp-1];
}

void runVM(struct VM *vm,int program[],int byteCodeSize){
  int intruc;

  while(vm->executing!=0)
    {
    if (vm->ip>=byteCodeSize)
    {
    vm->error=INSTRUCTION_OUT_OF_BOUNDS;
    vm->executing=0;
  }
  else{
    intruc=program[vm->ip];
      switch (intruc)
    {
      
        case PUSH:{
          if (vm->ip+1>=byteCodeSize) 
          {
          vm->error=INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing=0;
          }
          else
          {
          push(vm,program[(vm->ip)+1]);
           
          if (vm->error==NO_ERROR)
          {
          vm->ip+=2;
          }
        }

        break;
        }
      
        case ADD :{
          if(vm->sp<2){
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
          }
          else{
          int right=pop(vm);
          int left=pop(vm);

          push(vm,right+left);
          vm->ip++;
          }
        break;
        }

        case MUL:{
           if(vm->sp<2){
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
          }
          else{
          int right=pop(vm);
          int left=pop(vm);

          push(vm,right*left);
          vm->ip++;
          }
        break;
        }

        case SUB:{
          if (vm->sp<2)
          {
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
          }
          else{
          int right=pop(vm);
          int left=pop(vm);

          push(vm,left-right);
          vm->ip++;
          }
        break;
        }

        case JUMP:
        {
          //First check that JUMP's operand exists
          if ((vm->ip+1)>=byteCodeSize)
          {
            vm->error=INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing=0;
          }
          else
          {
            int target=program[(vm->ip)+1];
            if (target<byteCodeSize && target>=0)
            {
              vm->ip=target;
            }
            else{
              vm->error=INSTRUCTION_OUT_OF_BOUNDS;
              vm->executing=0;
            }
          }
  
          break;
        }

        case JZ:
        {
          int value;
          if (vm->ip+1>=byteCodeSize)
          {
            vm->error=INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing=0;
          }
          else{
            int target=program[(vm->ip)+1];
            if (target<byteCodeSize && target>=0)
            {
              value=pop(vm);
              if (vm->error!=NO_ERROR)
              {
                break;
              }
            }
            else{
              vm->error=INSTRUCTION_OUT_OF_BOUNDS;
              vm->executing=0;
            }
            if (value==0)
            {
              vm->ip=target;
            }
            else{
              vm->ip+=2;
            }
          }
          
          break;
        }

        case EQUAL:
        {
          if (vm->sp<2)
          {
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
            break;
          }
          
          int right=pop(vm);
          int left=pop(vm);

          if (left==right)
          {
            push(vm,1);
          }
          else
          {
            push(vm,0);
          }
          vm->ip++;
          break;
        }

        case LESS_THAN:
        {
          if (vm->sp<2)
          {
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
            break;
          }

          int right=pop(vm);
          int left=pop(vm);

          if (left<right)
          {
            push(vm,1);
          }
          else{
            push(vm,0);
          }
          vm->ip++;

          break;
        }

        case GREATER_THAN:
        {
          if (vm->sp<2)
          {
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
            break;
          }

          int right=pop(vm);
          int left=pop(vm);

          if (left>right)
          {
            push(vm,1);
          }
          else{
            push(vm,0);
          }
          vm->ip++;

          break;
        }

        case DIV:
        {
          if (vm->sp<2)
          {
            vm->error=STACK_UNDERFLOW;
            vm->executing=0;
            break;
          }
          int right=pop(vm);
          int left=pop(vm);

          if (right==0)
          {
            vm->error=DIVISION_BY_ZERO;
            vm->executing=0;
            break;
          }
          else{
            push(vm,left/right);
          }
          
          vm->ip++;
          break;
        }

        case STORE: 
        {
          if (vm->ip+1>=byteCodeSize)
          {
            vm->error=INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing=0;
            break;
          }
          else{
            int address=program[vm->ip+1];
            if (address>=0 && address<MEMORY_CAPACITY)
            {
              int value=pop(vm);
              if (vm->error==NO_ERROR)
              {
                vm->memory[address]=value;
              }
              else{
                break;
              } 
            }
            else{
              vm->error=MEMORY_OUT_OF_BOUNDS;
              vm->executing=0;
              break;
            }
          }
          vm->ip=+2;
          break;
        }

        case PRINT:
        {
          int value=peek(vm);
          if (vm->error!=NO_ERROR)
          {
            break;
          }
          else{
          printf("Value is: %d\n",value);
          vm->ip++;
          }
      
        break;
      }

        case HALT:
        {
          vm->executing=0;
        break;
        }

        default:{
        vm->error = INVALID_OPCODE;
        vm->executing = 0;
        break;
      }
    }
  }
 }
}

void errorReporting(int vm_error){
  switch (vm_error)
  {
  case STACK_UNDERFLOW:{
    printf("\n NovaVM Runtime Error: Stack underflow \nNot enough values on the stack to execute this instruction.\n");
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
  case INSTRUCTION_OUT_OF_BOUNDS:{
    printf("\nNovaVM Runtime Error: Instruction out of bounds\nNO certain value for the given instructon.\n");
    break;
  }
  case DIVISION_BY_ZERO:{
    printf("\nNovaVM Runtime Error: Division by zero\nCannot divide by zero.\n");
  }
  default:
    break;
  }
}

int main(){
   
  int program[]={PUSH, 20, PUSH, 5, DIV, PRINT, HALT};
  
  struct VM vm;

  initializeVM(&vm);

  /*now we have to build the stack pointer to know where the top of the stack is 
  and the integer array capable to play the role as an stack */
  int sizeProgram=sizeof(program)/sizeof(program[0]);

  runVM(&vm,program,sizeProgram);
  
  //reporting the error that can happen during the execution of the instructions
  errorReporting(vm.error);
  
  return 0;
}