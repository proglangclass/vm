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
                          retain(*sp);                  \
                       } while(0)
#define STACK_POP()    (*sp--)


void run(void *literals[], byte instructions[]) {
  byte    *ip = instructions;       // instruction pointer
  
  Object  *stack[STACK_MAX];        // THE famous stack
  Object **sp = stack;              // stack pointer, keeps track of current position
  
  Object  *locals[LOCALS_MAX] = {}; // where we store our local variables
  
  // Setup the runtime
  Object *self = Object_new();
  retain(self);
  
  // Start processing instructions
  while (1) {
    switch (*ip) {
      case PUSH_NUMBER: {
        ip++; // advance to the operand (literal index)
        STACK_PUSH(Number_new((long)literals[*ip]));
        break;
      }
      case PUSH_STRING: {
        ip++; // advance to the operand (literal index)
        STACK_PUSH(String_new((char *)literals[*ip]));
        break;
      }
      case PUSH_SELF: {
        STACK_PUSH(self);
        break;
      }
      case PUSH_NIL: {
        STACK_PUSH(NilObject);
        break;
      }
      case PUSH_BOOL: {
        ip++; // advance to operand (0 = false, 1 = true)
        if (*ip == 0) {
          STACK_PUSH(FalseObject);
        } else {
          STACK_PUSH(TrueObject);
        }
        break;
      }
      case CALL: {
        ip++; // advance to operand (method name index in literals)
        char *method = literals[*ip];
        ip++; // advance to operand (# of args)
        int argc = *ip;
        Object *argv[10];

        int i;
        for (i = argc - 1; i >= 0; i--) argv[i] = STACK_POP();
        Object *receiver = STACK_POP();

        Object *result = call(receiver, method, argv, argc);

        STACK_PUSH(result);

        // Release all the objects
        for (i = argc - 1; i >= 0; i--) release(argv[i]);
        release(receiver);

        break;
      }
      case RETURN: {
        goto cleanup;
        break;
      }
      case GET_LOCAL: {
        ip++; // advance operand (local index)
        STACK_PUSH(locals[*ip]);
        break;
      }
      case SET_LOCAL: {
        ip++; // local index
        locals[*ip] = STACK_POP();
        break;
      }
      case ADD: {
        Object *a = STACK_POP();
        Object *b = STACK_POP();

        STACK_PUSH(Number_new(Number_value(a) + Number_value(b)));

        release(a);
        release(b);

        break;
      }
      case JUMP_UNLESS: {
        ip++; // operand (offset, # of bytes to jump forward)
        byte offset = *ip;
        Object *condition = STACK_POP();

        if (!Object_is_true(condition)) ip += offset;

        release(condition);

        break;
      }
      case JUMP: {
        ip++; // operand (offset, # of bytes to jump forward)
        byte offset = *ip;

        ip += offset;

        break;
      }
    }
    ip++;
  }

cleanup:
  release(self);
  int i;
  // Release all the local variables
  for (i = 0; i < LOCALS_MAX; i++) if (locals[i]) release(locals[i]);
  // Release everything left on the stack
  while (sp > stack) release(STACK_POP());
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