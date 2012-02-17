#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "opcode.h"
#include "funcalloc.h"

typedef int (JitFunc)();

// Registers
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3

// Helper macros to assemble code
#define EMIT_T(i,T)          *ptr = (T)(i); ptr += sizeof(T)
#define EMIT(i)              EMIT_T(i, byte)
#define EMIT_INT(i)          EMIT_T(i, int)
#define EMIT_REG2REG(r1,r2)  EMIT(0xC0 | r1 << 3 | r2);

// Emit x86-64 machine code dynamicaly
void *precompile() {
  // Allocate memory to write the instructions to
  byte *ptr = funcalloc(4096);
  JitFunc *func = (JitFunc *)ptr;
  
  // Emit x86-64 machine code the equivalent of:
  // int *func() {
  //   return 30 + 2;
  // }
  
  // Setup stack frame (C calling convention)
  EMIT(0x55);                            // push   %ebp
  EMIT(0x48); EMIT(0x89); EMIT(0xe5);    // movq   %rsp,%rbp
  
  EMIT(0xB8 + EAX); EMIT_INT(30);        // mov    [int],%eax
  EMIT(0xB8 + ECX); EMIT_INT(2);         // mov    [int],%ecx
  EMIT(0x01); EMIT_REG2REG(ECX, EAX);    // add    %ecx,%eax
  
  EMIT(0xC9);                            // leave
  EMIT(0xC3);                            // ret
  
  // Execute the compiled function
  int value = func();
  printf("> %d\n", value);
  
  // Release the function
  funcfree(func, 4096);
}


/////// Here we compile our VM bytecode to x86-64 machine code ///////

// Set a register as being in use
#define REG_PUSH()          registers[ri++]
// Get and release a register.
#define REG_POP()           registers[--ri]

// Compile instructions to x86-64 machine code into a pointer to a function.
void compile(long literals[], byte instructions[], JitFunc *func) {
  byte *ip = instructions;
  
  byte *ptr = (byte *)func;
  byte registers[] = { EAX, ECX, EDX, EBX };
  int ri = 0; // register index
  
  // Setup stack frame (C calling convention)
  EMIT(0x55);                                       // push   %ebp
  EMIT(0x48); EMIT(0x89); EMIT(0xe5);               // movq   %rsp,%rbp
  
  while (1) {
    switch (*ip) {
      case PUSH_NUMBER: {
        ++ip; // advance to operand (literal)
        int number = (int)literals[*ip];
        EMIT(0xB8 + REG_PUSH()); EMIT_INT(number);  // mov    [int],%eax
        
        break;
      }
      case ADD: {
        byte reg1 = REG_POP();
        byte reg2 = REG_POP();
        EMIT(0x01); EMIT_REG2REG(reg1, reg2);      // add    %ebx,%eax
        break;
      }
      case RETURN:
        EMIT(0xC9);                                // leave
        EMIT(0xC3);                                // ret
        return;
    }
    ip++;
  }
}

int main(int argc, char const *argv[]) {
  long literals[] = {
    /* [0] */ (long) 30,
    /* [1] */ (long) 2
  };
  
  byte instructions[] = {
    PUSH_NUMBER, 0, // 30
    PUSH_NUMBER, 1, // 2
    ADD,
    RETURN,
  };
  
  // Allocate memory to write the instructions to
  JitFunc *func = funcalloc(4096);

  // Compile to the function pointer (func)
  compile(literals, instructions, func);
  // Execute the function
  printf("> %d\n", func());
  
  // Release memory used by function
  funcfree(func, 4096);
  
  return 0;
}
