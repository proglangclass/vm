#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include "opcode.h"

typedef int (JitFunc)();

// Registers
#define EAX 0
#define ECX 1
#define EDX 2
#define EBX 3

// Helper macros to assemble code
#define ASM_T(i,T)          *ptr = (T)(i); ptr += sizeof(T)
#define ASM(i)              ASM_T(i, byte)
#define ASM_INT(i)          ASM_T(i, int)
#define ASM_REG2REG(r1,r2)  ASM(0xC0 | r1 << 3 | r2);

// Set a register as being in use
#define REG_PUSH()          registers[ri++]
// Get and release a register.
#define REG_POP()           registers[--ri]

JitFunc *compile(long literals[], byte instructions[]) {
  byte *ip = instructions;
  
  byte *start, *ptr;
  start = ptr = malloc(4096);
  byte registers[] = { EAX, ECX, EDX, EBX };
  int ri = 0; // register index
  
  // Setup stack frame (C calling convention)
  ASM(0x55);                        // push   %ebp
  ASM(0x48); ASM(0x89); ASM(0xe5);  // movq   %rsp,%rbp
  
  while (1) {
    switch (*ip) {
      case PUSH_NUMBER:
        // mov    [int],%eax
        ASM(0xB8 + REG_PUSH());
        ++ip; // advance to operand (literal)
        ASM_INT((long)literals[*ip]);
        break;
        
      case ADD: {
        // add    %ebx,%eax
        ASM(0x01);
        byte reg1 = REG_POP();
        byte reg2 = REG_POP();
        ASM_REG2REG(reg1, reg2);
        break;
      }
      case RETURN:
        ASM(0xC9);  // leave
        ASM(0xC3);  // ret
        goto assemble;
    }
    ip++;
  }
  
  long size;
assemble:
  size = ptr - start;
  JitFunc *func = (JitFunc *)(byte *)mmap(NULL, size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, -1, 0);
  memcpy(func, start, sizeof(byte) * size);
  free(start);
  
  return func;
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
  
  JitFunc *compiled_function = compile(literals, instructions);
  printf("> %d\n", compiled_function());
  
  return 0;
}
