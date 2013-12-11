#include <stdio.h>
#include <assert.h>
#include "runtime.h"
#include "opcode.h"
#include "bytecode.h"

// Helpers to play with the stack
#define LOCALS_MAX     10
#define STACK_MAX      10
#define STACK_PUSH(I)  do {                             \
                          assert(sp-stack < STACK_MAX); \
                          *(++sp) = (I);                \
                       } while(0)
#define STACK_POP()    (*sp--)


void run(void *literals[], byte instructions[]) {
  byte    *ip = instructions;       // instruction pointer
  
  Object  *stack[STACK_MAX];        // THE famous stack
  Object **sp = stack;              // stack pointer, keeps track of current position
  
  Object  *locals[LOCALS_MAX] = {}; // where we store our local variables
  
  // Setup the runtime
  Object *self = Object_new();
  
  // Start processing instructions
  while (1) {
    switch (*ip) {
      
    }
    ip++;
  }
}

int main (int argc, char const *argv[]) {
  // Get compiled literals table and instructions from bytecode.h
  void *literals[] = LITERALS;
  byte instructions[] = INSTRUCTIONS;
  
  init_runtime();
  
  // Run that thing!
  run(literals, instructions);

  destroy_runtime();
  
  return 0;
}