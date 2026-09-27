#include <stdio.h>
#define MEMORY_CAPACITY 1024
#define CALL_STACK 512
#define LOCAL_MEMORY_CAPACITY 4096
#include "disassembler.h"
#include "assembler.h"
#include "lexer.h"
#include "parser.h"
#include "compiler.h"

enum Error
{
  NO_ERROR,
  STACK_UNDERFLOW,
  STACK_OVERFLOW,
  INVALID_OPCODE,
  INSTRUCTION_OUT_OF_BOUNDS,
  DIVISION_BY_ZERO,
  MEMORY_OUT_OF_BOUNDS,
  CALL_STACK_OVERFLOW,
  CALL_STACK_UNDERFLOW,
  LOCAL_MEMORY_OVERFLOW,
  NO_FUNCTION_CALL
};

// A call frame is basically a small package of information belonging to one specific function call.
struct Frame
{
  // so we need returnAddress to basically to remember where the interpreter need to come back after the func finishes
  int returnAddress;
  // basePointer tho remembers where this function's local variables start
  int basePointer;
  // how Many local variables slots this func needs
  int localCount;
};

struct VM
{
  // ip stands for instruction pointer
  int ip;
  int sp;
  int stack[100];
  int executing;
  int memory[MEMORY_CAPACITY];
  struct Frame callStack[CALL_STACK];
  int call_sp;
  int localMemory[LOCAL_MEMORY_CAPACITY];
  // tells where the next free local slot is
  int local_pointer;
  enum Error error;
};

enum Opcode
{
  PUSH,
  ADD,
  MUL,
  SUB,
  PRINT,
  JUMP,
  JZ, // jump to zero
  EQUAL,
  LESS_THAN,
  GREATER_THAN,
  DIV, // this is added to handle division by zero
  STORE,
  LOAD,
  CALL,
  RET,
  STORE_LOCAL,
  LOAD_LOCAL,
  HALT
};

void initializeVM(struct VM *virtual_machine)
{
  virtual_machine->ip = 0;
  virtual_machine->sp = 0;
  // creating an running VM variable
  virtual_machine->executing = 1;
  for (int i = 0; i < MEMORY_CAPACITY; i++)
  {
    virtual_machine->memory[i] = 0;
  }
  for (int i = 0; i < LOCAL_MEMORY_CAPACITY; i++)
  {
    virtual_machine->localMemory[i] = 0;
  }
  virtual_machine->call_sp = 0;
  virtual_machine->local_pointer = 0;
  // assigning the virtual machine to the error state which for now is no error accured
  virtual_machine->error = NO_ERROR;
}
// poping the values out of the stack
int pop(struct VM *virtual_machine)
{

  if (virtual_machine->sp < 1)
  {
    virtual_machine->error = STACK_UNDERFLOW;
    virtual_machine->executing = 0;
    return 0;
  }

  virtual_machine->sp--;
  return virtual_machine->stack[virtual_machine->sp];
}
// pushing the values into the stack
void push(struct VM *vm, int value)
{
  if (vm->sp >= 100)
  {
    vm->error = STACK_OVERFLOW;
    vm->executing = 0;
  }
  else
  {
    vm->stack[vm->sp] = value;
    vm->sp++;
  }
}
// returning the top value of the stack
int peek(struct VM *vm)
{
  if (vm->sp < 1)
  {
    vm->error = STACK_UNDERFLOW;
    vm->executing = 0;
    return 0;
  }
  return vm->stack[vm->sp - 1];
}

void runVM(struct VM *vm, int program[], int byteCodeSize)
{
  int intruc;

  while (vm->executing != 0)
  {
    if (vm->ip >= byteCodeSize)
    {
      vm->error = INSTRUCTION_OUT_OF_BOUNDS;
      vm->executing = 0;
    }
    else
    {
      intruc = program[vm->ip];
      switch (intruc)
      {

      case PUSH:
      {
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
        }
        else
        {
          push(vm, program[(vm->ip) + 1]);

          if (vm->error == NO_ERROR)
          {
            vm->ip += 2;
          }
        }

        break;
      }

      case ADD:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
        }
        else
        {
          int right = pop(vm);
          int left = pop(vm);

          push(vm, right + left);
          vm->ip++;
        }
        break;
      }

      case MUL:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
        }
        else
        {
          int right = pop(vm);
          int left = pop(vm);

          push(vm, right * left);
          vm->ip++;
        }
        break;
      }

      case SUB:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
        }
        else
        {
          int right = pop(vm);
          int left = pop(vm);

          push(vm, left - right);
          vm->ip++;
        }
        break;
      }

      case JUMP:
      {
        // First check that JUMP's operand exists
        if ((vm->ip + 1) >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
        }
        else
        {
          int target = program[(vm->ip) + 1];
          if (target < byteCodeSize && target >= 0)
          {
            vm->ip = target;
          }
          else
          {
            vm->error = INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing = 0;
          }
        }

        break;
      }

      case JZ:
      {
        int value;
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
        }
        else
        {
          int target = program[(vm->ip) + 1];
          if (target < byteCodeSize && target >= 0)
          {
            value = pop(vm);
            if (vm->error != NO_ERROR)
            {
              break;
            }
          }
          else
          {
            vm->error = INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing = 0;
          }
          if (value == 0)
          {
            vm->ip = target;
          }
          else
          {
            vm->ip += 2;
          }
        }

        break;
      }

      case EQUAL:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
          break;
        }

        int right = pop(vm);
        int left = pop(vm);

        if (left == right)
        {
          push(vm, 1);
        }
        else
        {
          push(vm, 0);
        }
        vm->ip++;
        break;
      }

      case LESS_THAN:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
          break;
        }

        int right = pop(vm);
        int left = pop(vm);

        if (left < right)
        {
          push(vm, 1);
        }
        else
        {
          push(vm, 0);
        }
        vm->ip++;

        break;
      }

      case GREATER_THAN:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
          break;
        }

        int right = pop(vm);
        int left = pop(vm);

        if (left > right)
        {
          push(vm, 1);
        }
        else
        {
          push(vm, 0);
        }
        vm->ip++;

        break;
      }

      case DIV:
      {
        if (vm->sp < 2)
        {
          vm->error = STACK_UNDERFLOW;
          vm->executing = 0;
          break;
        }
        int right = pop(vm);
        int left = pop(vm);

        if (right == 0)
        {
          vm->error = DIVISION_BY_ZERO;
          vm->executing = 0;
          break;
        }
        else
        {
          push(vm, left / right);
        }

        vm->ip++;
        break;
      }

      case STORE:
      {
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        else
        {
          int address = program[vm->ip + 1];
          if (address >= 0 && address < MEMORY_CAPACITY)
          {
            int value = pop(vm);
            if (vm->error == NO_ERROR)
            {
              vm->memory[address] = value;
            }
            else
            {
              break;
            }
          }
          else
          {
            vm->error = MEMORY_OUT_OF_BOUNDS;
            vm->executing = 0;
            break;
          }
        }
        vm->ip += 2;
        break;
      }

      case LOAD:
      {
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        else
        {
          int address = program[vm->ip + 1];
          if (address >= 0 && address < MEMORY_CAPACITY)
          {
            int value = vm->memory[address];
            push(vm, value);
            if (vm->error != NO_ERROR)
            {
              break;
            }
          }
          else
          {
            vm->error = MEMORY_OUT_OF_BOUNDS;
            vm->executing = 0;
            break;
          }
        }
        vm->ip += 2;
        break;
      }

      case CALL:
      {
        if (vm->ip + 1 >= byteCodeSize || vm->ip + 2 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        else
        {
          int target = program[vm->ip + 1];
          vm->callStack[vm->call_sp].localCount = program[vm->ip + 2];

          if (target < byteCodeSize && target >= 0 && vm->callStack[vm->ip].localCount >= 0)
          {
            if (vm->call_sp < CALL_STACK)
            {
              if (vm->local_pointer + vm->callStack[vm->ip].localCount < LOCAL_MEMORY_CAPACITY)
              {
                vm->callStack[vm->call_sp].returnAddress = vm->ip + 3;
                vm->callStack[vm->call_sp].basePointer = vm->local_pointer;
                vm->local_pointer += vm->callStack[vm->call_sp].localCount;
                vm->call_sp++;
                vm->ip = target;
              }
              else
              {
                vm->error = LOCAL_MEMORY_OVERFLOW;
                vm->executing = 0;
                break;
              }
            }
            else
            {
              vm->error = CALL_STACK_OVERFLOW;
              vm->executing = 0;
              break;
            }
          }
          else
          {
            vm->error = INSTRUCTION_OUT_OF_BOUNDS;
            vm->executing = 0;
          }
        }
        break;
      }

      case RET:
      {
        if (vm->call_sp > 0)
        {
          vm->call_sp--;
          vm->local_pointer = vm->callStack[vm->call_sp].basePointer;
          vm->ip = vm->callStack[vm->call_sp].returnAddress;
        }
        else
        {
          vm->error = CALL_STACK_UNDERFLOW;
          vm->executing = 0;
        }
        break;
      }

      case STORE_LOCAL:
      {
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        if (vm->call_sp <= 0)
        {
          vm->error = NO_FUNCTION_CALL;
          vm->executing = 0;
          break;
        }
        int offsetOp = program[vm->ip + 1];
        struct Frame *currentFrame = &vm->callStack[vm->call_sp - 1];

        if (offsetOp < 0 || offsetOp >= currentFrame->localCount)
        {
          vm->error = MEMORY_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        int address = currentFrame->basePointer + offsetOp;
        int value = pop(vm);
        if (vm->error != NO_ERROR)
        {
          break;
        }
        vm->localMemory[address] = value;
        vm->ip += 2;

        break;
      }

      case LOAD_LOCAL:
      {
        if (vm->ip + 1 >= byteCodeSize)
        {
          vm->error = INSTRUCTION_OUT_OF_BOUNDS;
          vm->executing = 0;
          break;
        }
        else
        {
          if (vm->call_sp <= 0)
          {
            vm->error = NO_FUNCTION_CALL;
            vm->executing = 0;
            break;
          }
          else
          {
            int localOffset = program[vm->ip + 1];
            struct Frame *currentFrame = &vm->callStack[vm->call_sp - 1];

            if (localOffset < 0 || localOffset >= currentFrame->localCount)
            {
              vm->error = MEMORY_OUT_OF_BOUNDS;
              vm->executing = 0;
              break;
            }
            else
            {
              int address = currentFrame->basePointer + localOffset;
              int value = vm->localMemory[address];

              push(vm, value);
              if (vm->error == NO_ERROR)
              {
                vm->ip += 2;
              }
            }
          }
        }
        break;
      }

      case PRINT:
      {
        int value = peek(vm);
        if (vm->error != NO_ERROR)
        {
          break;
        }
        else
        {
          printf("Value is: %d\n", value);
          vm->ip++;
        }

        break;
      }

      case HALT:
      {
        vm->executing = 0;
        break;
      }

      default:
      {
        vm->error = INVALID_OPCODE;
        vm->executing = 0;
        break;
      }
      }
    }
  }
}

void errorReporting(int vm_error)
{
  switch (vm_error)
  {
  case STACK_UNDERFLOW:
  {
    printf("\nNovaVM Runtime Error: Stack underflow \nNot enough values on the stack to execute this instruction.\n");
    break;
  }
  case STACK_OVERFLOW:
  {
    printf("\nNovaVM Runtime Error: Stack overflow \nThe stack has reached its maximum capacity.\n");
    break;
  }
  case INVALID_OPCODE:
  {
    printf("\nNovaVM Runtime Error: Invalid opcode\nThe VM encountered an unknown instruction.\n");
    break;
  }
  case INSTRUCTION_OUT_OF_BOUNDS:
  {
    printf("\nNovaVM Runtime Error: Instruction out of bounds\nNO certain value for the given instructon.\n");
    break;
  }
  case DIVISION_BY_ZERO:
  {
    printf("\nNovaVM Runtime Error: Division by zero\nCannot divide by zero.\n");
    break;
  }
  case MEMORY_OUT_OF_BOUNDS:
  {
    printf("\nNovaVM Runtime Error: Memory out of bounds\nCannot go beyond memory space.\n");
    break;
  }
  case CALL_STACK_OVERFLOW:
  {
    printf("\nNovaVM Runtime Error: Call stack overflow\nThe stack has reached its maximum capacity.\n");
    break;
  }
  case CALL_STACK_UNDERFLOW:
  {
    printf("\nNovaVM Runtime Error:Call stack underflow \nNot enough values on the stack to execute this instruction.\n");
    break;
  }
  case LOCAL_MEMORY_OVERFLOW:
  {
    printf("\nNovaVM Runtime Error: Local memory overflow \nThe local memory has reached its maximum capacity.\n");
    break;
  }
  case NO_FUNCTION_CALL:
  {
    printf("\nNovaVM Runtime Error: No function call \nThere isn't any function to be called.\n");
    break;
  }
  default:
    break;
  }
}

int main()
{
  char source[MAX_BYTECODE][MAX_LINE_LENGTH];
  int program[MAX_BYTECODE];
  char novaSource[MAX_SOURCE_LENGTH];

  struct VM vm;
  struct FileStatus status;
  struct Lexer lexer;
  struct Token token;
  struct Parser parser;
  struct Program *astProgram;

  initializeVM(&vm);

  int byteCodeSize = 0;
  /*now we have to build the stack pointer to know where the top of the stack is
  and the integer array capable to play the role as an stack */
  int lines = load_source_file("file.novaasm", source, 100, &status);

  if (status.error != FILE_NO_ERROR)
  {
    file_error_reporting(&status);
    return 1;
  }

  FILE *fp = NULL;
  fp = fopen("file.nova", "r");
  if (fp == NULL)
  {
    printf("Cannot open the file ");
    return 1;
  }

  readFile(fp, novaSource);

  initializeLexer(&lexer, novaSource);
  token = scanToken(&lexer);

  while (token.type != TOKEN_EOF)
  {
    printf("%s line : %d\n", token.lexeme, token.line);
    token = scanToken(&lexer);
  }
  printf("\nParser\n");

  initializeLexer(&lexer, novaSource);
  initializeParser(&parser, &lexer);

  astProgram = parseProgram(&parser);
  if (astProgram == NULL)
  {
    printf("Parser error: %d\n", parser.error);
  }
  else
  {
    for (int i = 0; i < astProgram->declarationCount; i++)
    {
      printf("Name: %s\n", astProgram->declarations[i]->name);
      printf("Type: %d\n", astProgram->declarations[i]->value->type);

      if (astProgram->declarations[i]->value->type == EXPR_NUMBER)
      {
        printf("Number: %d\n", astProgram->declarations[i]->value->data.number);
      }

      printf("\n");
    }
    printf("\nHere\n");
    struct Expression *expr = astProgram->declarations[2]->value;

    if (expr->type == EXPR_BINARY)
    {
      printf("Left type: %d\n", expr->data.binary.left->type);
      printf("Left identifier: %s\n", expr->data.binary.left->data.identifier);

      printf("Operator: %d\n", expr->data.binary.operator);

      printf("Right type: %d\n", expr->data.binary.right->type);
      printf("Right identifier: %s\n", expr->data.binary.right->data.identifier);
    }
  }

  if (assemble(source, lines, program, MAX_BYTECODE, &byteCodeSize) == 1)
  {
    printf("\n--- DISASSEMBLY ---\n");
    disassemble(program, byteCodeSize);

    printf("\n--- VM ---\n");
    runVM(&vm, program, byteCodeSize);

    // reporting the error that can happen during the execution of the instructions
    errorReporting(vm.error);
  }

  return 0;
}