/* == Mock runtime ==
 * This runtime is only for demonstration purposes and implements the strict
 * minimum for the example shown in the class.
 */

#include <stdlib.h>
#include <string.h>

enum { tObject, tNumber, tString };

typedef struct {
  char type;
  union {
    char *string;
    int number;
  } value;
  int refcount;
} Object;

static Object *TrueObject;
static Object *FalseObject;
static Object *NilObject;

Object *Object_new() {
  return calloc(1, sizeof(Object));
}

/////////////// GC ///////////////

Object *retain(Object *self) {
  self->refcount++;
  return self;
}

void release(Object *self) {
  self->refcount--;
  if (self->refcount <= 0)
    free(self);
}

///////////////////////////////////


char Object_is_true(Object *self) {
  // false and nil are == false,
  // everything else is true.
  if (self == FalseObject || self == NilObject) return 0;
  return 1;
}

Object *Number_new(int value) {
  Object *object = Object_new();
  object->type = tNumber;
  object->value.number = value;
  return object;
}

int Number_value(Object *self) {
  assert(self->type == tNumber);
  return self->value.number;
}

Object *String_new(char *value) {
  Object *object = Object_new();
  object->type = tString;
  object->value.string = value;
  return object;
}

Object *call(Object *receiver, char *message, Object *argv[], int argc) {
  // HACK hardcoded methods. In a real runtime, you'd have proper classes and methods.
  // See runtime.rb in our interpreter for an example.
  if (strcmp(message, "+") == 0) {
    // Number#+
    return Number_new(receiver->value.number + argv[0]->value.number);
  } else if (strcmp(message, "print") == 0) {
    // Object#print
    switch (argv[0]->type) {
      case tNumber:
        printf("%d\n", argv[0]->value.number);
        return argv[0];
      case tString:
        printf("%s\n", argv[0]->value.string);
        return argv[0];
    }
  }
  
  return 0;
}

void init_runtime() {
  TrueObject = retain(Object_new());
  FalseObject = retain(Object_new());
  NilObject = retain(Object_new());
}

void destroy_runtime() {
  release(TrueObject);
  release(FalseObject);
  release(NilObject);
}