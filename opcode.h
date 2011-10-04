enum {
  //                                      ------- Stack -------
  // Opcode                  Operands     before         after
  /* 00 */ CALL,          // index, argc  [rcv, arg...]  [returned]
  /* 01 */ PUSH_NUMBER,   // index        []             [number]
  /* 02 */ PUSH_STRING,   // index        []             [string]
  /* 03 */ PUSH_SELF,     //              []             [self]
  /* 04 */ PUSH_NIL,      //              []             [nil]
  /* 05 */ PUSH_BOOL,     // 1=t, 0=f     []             [true or false]
  /* 06 */ GET_LOCAL,     // index        []             [value]
  /* 07 */ SET_LOCAL,     // index        [value]        []
  /* 08 */ JUMP_UNLESS,   // offset       [test]         []
  /* 09 */ ADD,           //              [a, b]         [result]
  /* 10 */ RETURN         //              []             []
};

typedef unsigned char byte; // 1 byte