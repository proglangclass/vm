#include <stdio.h>
#include <assert.h>
#include "runtime.h"
#include "opcode.h"

// Helpers to play with the stack
#define STACK_MAX      10
#define STACK_PUSH(I)  do {                             \
                          assert(sp-stack < STACK_MAX); \
                          *(++sp) = (I);                \
                       } while(0)
#define STACK_POP()    (*sp--)


void run(long literals[], byte instructions[]) {
  byte    *ip = instructions;      // instruction pointer
  
  Object  *stack[STACK_MAX];       // the famous stack
  Object **sp = stack;             // stack pointer
  
  Object  *locals[STACK_MAX] = {}; // where we store our local variables
  
  // Setup the runtime
  Object *self = Object_new();
  
  // Start processing instructions
  while (1) {
    switch (*ip) {
      case CALL: {
        ip++; // advance to operand (method literal index)
        char *method = (char *)literals[*ip];
        ip++; // advance to operand (# of args)
        
        int argc = *ip;
        assert(argc < 10); // HACK max 10 args
        Object *argv[10];
        
        // Pop all args from the stack and compile as an array
        int i;
        for (i = 0; i < argc; ++i) argv[0] = STACK_POP();
        Object *receiver = STACK_POP();
        
        Object *result = call(receiver, method, argv, argc);
        STACK_PUSH(result);
        
        break;
      }
      case PUSH_NUMBER:
        ip++; // advance to operand (literal index)
        STACK_PUSH(Number_new((long)literals[*ip]));
        break;
        
      case PUSH_STRING:
        ip++; // advance to operand (literal index)
        STACK_PUSH(String_new((char *)literals[*ip]));
        break;
        
      case PUSH_SELF:
        STACK_PUSH(self);
        break;
        
      case PUSH_NIL:
        STACK_PUSH(NilObject);
        break;
      
      case PUSH_BOOL:
        ip++; // advance to operand (0 = false, 1 = true)
        if (*ip == 0) {
          STACK_PUSH(FalseObject);
        } else {
          STACK_PUSH(TrueObject);
        }
        break;
        
      case GET_LOCAL:
        ip++; // advance to operand (local index)
        STACK_PUSH(locals[*ip]);
        break;
        
      case SET_LOCAL:
        ip++; // advance to operand (local index)
        locals[*ip] = STACK_POP();
        break;
        
      case ADD: {
        Object *a = STACK_POP();
        Object *b = STACK_POP();
        
        STACK_PUSH(Number_new(Number_value(a) + Number_value(b)));
        
        break;
      }
      case JUMP_UNLESS: {
        ip++; // advance to operand (offset)
        long offset = *ip;
        void *test = STACK_POP();
        
        if (!Object_is_true(test)) ip += offset;
        
        break;
      }
      case RETURN:
        return;
        
    }
    ip++;
  }
}

int main (int argc, char const *argv[]) {
  // long can store a pointer (and numbers too).
  long literals[] = {
    
  };
  
  byte instructions[] = {  };
  
  init_runtime();
  run(literals, instructions);
  destroy_runtime();
  
  return 0;
}