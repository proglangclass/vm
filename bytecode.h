/*
  This file was compiled by the `compile` script.
  It contains the bytecode for the following code:
  
    print("hi")
*/

#define LITERALS { \
    (void *) "hi", \
    (void *) "print", \
  }

#define INSTRUCTIONS { 3, 2, 0, 0, 1, 1, 11 }

/*
PUSH_SELF,          [self]
PUSH_STRING, 0,     [self, "hi"]
CALL,        1, 1   self.print("hi")
RETURN
*/
