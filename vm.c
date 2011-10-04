#include <stdio.h>
#include <assert.h>
#include "runtime.h"
#include "opcode.h"

// Helpers to play with the stack
#define STACK_MAX      10
#define STACK_PUSH(I)  do {                             \
                          assert(sp-stack < STACK_MAX); \
                          *(++sp) = (I);                \
                          retain(*sp); } while(0)
#define STACK_POP()    (*sp--)


void run(long literals[], byte instructions[]) {
  byte    *ip = instructions;      // instruction pointer
  
  Object  *stack[STACK_MAX];       // the famous stack
  Object **sp = stack;             // stack pointer
  
  Object  *locals[STACK_MAX] = {}; // where we store our local variables
  
  // Setup the runtime
  Object *self = Object_new();
  retain(self); // make sure self wont be released
  
  // Start processing instructions
  while (1) {
    switch (*ip) {
      case CALL: {
        ip++; // advance to operand (message literal index)
        char *message = (char *)literals[*ip];
        ip++;
        int argc = *ip;
        assert(argc < 10); // HACK max 10 args
        Object *argv[10];
        int i;
        
        // Pop all args from the stack and compile as an array
        for (i = 0; i < argc; ++i) argv[0] = STACK_POP();
        Object *receiver = STACK_POP();
        
        Object *result = call(receiver, message, argv, argc);
        STACK_PUSH(result);
        
        // Release objects
        for (i = 0; i < argc; ++i) release(argv[0]);
        release(receiver);
        
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
        
        release(a);
        release(b);
        
        break;
      }
      case JUMP_UNLESS: {
        ip++; // advance to operand (offset)
        long offset = *ip;
        void *test = STACK_POP();
        
        if (!Object_is_true(test)) ip += offset;
        
        release(test);
        
        break;
      }
      case RETURN:
        goto cleanup;
        break;
        
    }
    ip++;
  }
  
  int i;
cleanup:
  // Release all local values (variable & stack)
  release(self);
  for(i = 0; i < STACK_MAX; ++i) if (locals[i]) release(locals[i]);
  while (sp > stack) release(STACK_POP());
}

int main (int argc, char const *argv[]) {
  // Original code:
  // print("the answer is:")
  // a = 30 + 2
  // if true
  //   print(a)
  // end
  
  // long can store a pointer (a numbers too).
  long literals[] = {
    /* [0] */ (long) "the answer is:",
    /* [1] */ (long) "print",
    /* [2] */ (long) 30,
    /* [3] */ (long) 2
  };
  
  // locals
  // [0] a
  
  byte instructions[] = {
    /* 03 */ PUSH_SELF,
    /* 02 */ PUSH_STRING, 0,    // "the answer is:"
    /* 00 */ CALL,        1, 1, // call "print" w/ 1 arg
    /* 01 */ PUSH_NUMBER, 2,    // 30
    /* 01 */ PUSH_NUMBER, 3,    // 2
    /* 09 */ ADD,
    /* 07 */ SET_LOCAL,   0,    // a
    /* 05 */ PUSH_BOOL,   1,    // true
    /* 08 */ JUMP_UNLESS, 6,    // jump 6 more instructions unless true
    /* 03 */ PUSH_SELF,
    /* 06 */ GET_LOCAL,   0,    // a
    /* 00 */ CALL,        1, 1, // call "print" w/ 1 arg
    /* 10 */ RETURN
  };
  
  init_runtime();
  run(literals, instructions);
  destroy_runtime();
  
  return 0;
}