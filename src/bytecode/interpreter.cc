#include "interpreter.hh"

#include <cassert>
#include <array>
#include <stack>
#include <iostream>

#include "opcodes.hh"

#define DISPATCH_NO_INCR_PC()                                    \
  do {                                                           \
  instr = chunks[chunk_ptr].first.code[instruction_ptr];         \
  goto *(opcode_dispatch_table[static_cast<int>(instr.opcode)]); \
  } while (false);                                               \

#define DISPATCH()      \
  do {                  \
  instruction_ptr++;    \
  DISPATCH_NO_INCR_PC() \
  } while (false);      \

// I tried doing this with a repetition macro but it was 5am so I gave up
#define EMPTY_OPCODE &&BadOpcode
#define EMPTY_OPCODES_4    \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode
#define EMPTY_OPCODES_8    \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode
#define EMPTY_OPCODES_12() \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode
#define EMPTY_OPCODES_16() \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode

int bytecode::InterpretBytecode(BytecodeExecutable executable) {
  // first we have to assemble the executable into something coherent
  // but I won't do that right now, so instead
  // TODO support more than one chunk
  auto& chunks = executable.chunks;
  assert(chunks.size() == 1);

  // also don't support symbols yet TODO
  assert(executable.symbol_table.size() == 0);

  std::array<void*, 256> opcode_dispatch_table = {
    /********** 0x00 **********/
    &&Trap,
    &&Return,
    &&Breakpoint,
    &&PrintLong,
    &&PrintChar,
    EMPTY_OPCODES_12(),
    /********** 0x10 **********/
    &&Nop,
    &&AddLong,
    &&MulLong,
    EMPTY_OPCODE,
    EMPTY_OPCODES_12(),
    /********** 0x20 **********/
    EMPTY_OPCODES_16(),
    /********** 0x30 **********/
    EMPTY_OPCODES_16(),
    /********** 0x40 **********/
    &&ImmByte,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODES_12(),
    /********** 0x50 **********/
    EMPTY_OPCODES_16(),
    /********** 0x60 **********/
    &&Dup,
    &&Rot2,
    &&Rot3,
    /********** 0x70 **********/
    EMPTY_OPCODES_16(),
    /********** 0x80 **********/
    EMPTY_OPCODES_16(),
    /********** 0x90 **********/
    EMPTY_OPCODES_16(),
    /********** 0xA0 **********/
    EMPTY_OPCODES_16(),
    /********** 0xB0 **********/
    EMPTY_OPCODES_16(),
    /********** 0xC0 **********/
    EMPTY_OPCODES_16(),
    /********** 0xD0 **********/
    EMPTY_OPCODES_16(),
    /********** 0xE0 **********/
    EMPTY_OPCODES_16(),
    /********** 0xF0 **********/
    EMPTY_OPCODES_16(),
  };

  // set up virtual machine state
  int chunk_ptr = 0; // current executing chunk
  int instruction_ptr = 0; // within chunk
  std::stack<Value> stack;
  Instruction instr;

  DISPATCH_NO_INCR_PC(); // kick it off and it'll drive itself

BadOpcode:
  // TODO spdlog this
Trap:
  std::abort();
Return:
  return stack.top();
Breakpoint:
  // TODO unimplemented
  goto BadOpcode;
PrintLong: {
  Value val = stack.top();
  stack.pop();
  std::cout << std::dec << val;
  DISPATCH();
}
PrintChar:
  // TODO unimplemented
  goto BadOpcode;
Nop:
  DISPATCH();
AddLong: {
  Value val_1 = stack.top();
  stack.pop();
  Value val_2 = stack.top();
  stack.pop();
  stack.push(val_1 + val_2);
  DISPATCH();
}
MulLong: {
  Value val_1 = stack.top();
  stack.pop();
  Value val_2 = stack.top();
  stack.pop();
  stack.push(val_1 * val_2);
  DISPATCH();
}
ImmByte:
  stack.push(static_cast<int8_t>(instr.param));
  DISPATCH();
Dup: {
  Value val_1 = stack.top();
  stack.push(val_1);
}
Rot2: {
  Value val_1 = stack.top();
  stack.pop();
  Value val_2 = stack.top();
  stack.pop();
  stack.push(val_1);
  stack.push(val_2);
  DISPATCH();
}
Rot3:
  Value val_1 = stack.top();
  stack.pop();
  Value val_2 = stack.top();
  stack.pop();
  Value val_3 = stack.top();
  stack.pop();
  stack.push(val_2);
  stack.push(val_1);
  stack.push(val_3);
  DISPATCH();

  __builtin_unreachable();
}
