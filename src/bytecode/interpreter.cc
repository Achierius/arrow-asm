#include "interpreter.hh"

#include <cassert>
#include <array>
#include <stack>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "fmt/core.h"
#include "fmt/color.h"

#include "opcodes.hh"

#define DISPATCH_NO_INCR_PC()                                    \
  do {                                                           \
  instr = chunks[chunk_idx].code[pc];         \
  debug_dispatch_hook();                                         \
  goto *(opcode_dispatch_table[static_cast<int>(instr.opcode)]); \
  } while (false);                                               \

#define DISPATCH()      \
  do {                  \
  pc++;    \
  DISPATCH_NO_INCR_PC() \
  } while (false);      \

// I tried doing this with a repetition macro but it was 5am so I gave up
#define EMPTY_OPCODE &&BadOpcode
#define EMPTY_OPCODES_4()  \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode,             \
  &&BadOpcode
#define EMPTY_OPCODES_8()  \
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

  auto& symtab = executable.symbol_table;
  // also don't support symbols yet TODO
  assert(symtab.size() == 0);

  std::array<void*, 256> opcode_dispatch_table = {
    /********** 0x00 **********/
    &&Trap,
    &&Exit,
    &&Breakpoint,
    &&PrintLong,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODES_8(),
    /********** 0x10 **********/
    &&Nop,
    &&AddLong,
    &&SubLong,
    &&MulLong,
    &&IDivLong,
    &&ModuloLong,
    &&LeftShiftLong,
    &&RightShiftLogicalLong,
    &&RightShiftArithmeticLong,
    &&LogicalAndLong,
    &&LogicalOrLong,
    &&UnaryNegate,
    &&AddFloat,
    &&SubFloat,
    &&MulFloat,
    &&DivFloat,
    /********** 0x20 **********/
    EMPTY_OPCODES_16(),
    /********** 0x30 **********/
    EMPTY_OPCODES_16(),
    /********** 0x40 **********/
    &&ImmByte,
    &&Constant,
    &&BiasConstantWindow,
    &&LoadGlobal,
    &&StoreGlobal,
    &&BiasGlobalWindow,
    EMPTY_OPCODE,
    EMPTY_OPCODE,
    EMPTY_OPCODES_8(),
    /********** 0x50 **********/
    EMPTY_OPCODES_16(),
    /********** 0x60 **********/
    &&Dup,
    &&Rot2,
    &&Rot3,
    EMPTY_OPCODE,
    EMPTY_OPCODES_12(),
    /********** 0x70 **********/
    &&Jump,
    &&TestAndJump,
    &&Call,
    &&Return,
    EMPTY_OPCODES_12(),
    /********** 0x80 **********/
    &&Allocate,
    &&AllocateImm,
    &&Deallocate,
    &&LoadClassConstructor,
    &&LoadClassDestructor,
    &&LoadObjectField,
    &&StoreObjectField,
    EMPTY_OPCODE,
    EMPTY_OPCODES_8(),
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
  int chunk_idx = 0;       // current executing chunk ID
  int cycle_count = 0;     // how many instructions we've executed
  int pc = 0; // within chunk
  int constant_window_base = 0; // within current chunk
  std::stack<Value> data_stack;
  std::stack<std::pair<int, int>> return_stack; // chunk_idx, pc
  Instruction instr;

  // debug-only hook that gets called every time we dispatch to let us log stuff
  auto debug_dispatch_hook = [&](){
    spdlog::debug("{:0>6} {:0>8} {:0>16x}: {} {}", cycle_count, chunk_idx,
                  pc, instr.opcode, instr.param);
    cycle_count++;
  };

  auto Pop = [&data_stack]() -> Value {
    Value val = data_stack.top();
    data_stack.pop();
    return val;
  };
  auto Peek = [&data_stack]() -> Value {
    return data_stack.top();
  };
  auto Push = [&data_stack](Value val) {
    data_stack.push(val);
  };

  DISPATCH_NO_INCR_PC(); // kick it off and it'll drive itself

BadOpcode:
  // TODO spdlog this
  spdlog::critical("bad opcode at {:0>8}:{:0>16x}", chunk_idx, pc);
  std::abort();
Trap:
  spdlog::critical("user code trap");
  std::abort();
Exit:
  return Peek();
Breakpoint:
  // TODO unimplemented
  goto BadOpcode;
PrintLong: {
  Value val = Pop();

  std::cout << std::dec << val;
  DISPATCH();
}
Nop:
  DISPATCH();
AddLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 + tos);
  DISPATCH();
}
SubLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 - tos);
  DISPATCH();
}
MulLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 * tos);
  DISPATCH();
}
IDivLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 / tos);
  DISPATCH();
}
ModuloLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 % tos);
  DISPATCH();
}
LeftShiftLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 << tos);
  DISPATCH();
}
RightShiftLogicalLong: {
  uint64_t tos {static_cast<uint64_t>(Pop())};
  uint64_t tos1 {static_cast<uint64_t>(Pop())};
  Push(tos1 >> tos);
  DISPATCH();
}
RightShiftArithmeticLong: {
  long tos {Pop()};
  long tos1 {Pop()};
  Push(tos1 >> tos);
  DISPATCH();
}
ImmByte:
  Push(static_cast<int8_t>(instr.param));
  DISPATCH();
Constant: {
  int index = constant_window_base + static_cast<int8_t>(instr.param);
  Value val = chunks[chunk_idx].constants.at(index);
  Push(val);
  DISPATCH();
}
BiasConstantWindow: {
  constant_window_base += static_cast<int8_t>(instr.param);
  DISPATCH();
}
LoadGlobal: {
  // TODO
  goto BadOpcode;
}
BiasGlobalWindow: {
  // TODO
  goto BadOpcode;
}
StoreGlobal: {
  // TODO
  goto BadOpcode;
}
Dup:
  Push(Peek());
  DISPATCH();
Rot2: {
  Value tos {Pop()};
  Value tos1 {Pop()};
  Push(tos);
  Push(tos1);
  DISPATCH();
}
Rot3: {
  Value tos {Pop()};
  Value tos1 {Pop()};
  Value tos2 {Pop()};
  Push(tos1);
  Push(tos);
  Push(tos2);
  DISPATCH();
}
LogicalAndLong: {
  // TODO
  goto BadOpcode;
}
LogicalOrLong: {
  // TODO
  goto BadOpcode;
}
UnaryNegate: {
  // TODO
  goto BadOpcode;
}
AddFloat: {
  // TODO
  goto BadOpcode;
}
SubFloat: {
  // TODO
  goto BadOpcode;
}
MulFloat: {
  // TODO
  goto BadOpcode;
}
DivFloat: {
  // TODO
  goto BadOpcode;
}
Jump: {
  // TODO
  goto BadOpcode;
}
TestAndJump: {
  // TODO
  goto BadOpcode;
}
Call: {
  // TODO
  goto BadOpcode;
}
Return: {
  // TODO
  goto BadOpcode;
}
LoadClassConstructor: {
  // TODO
  goto BadOpcode;
}
LoadClassDestructor: {
  // TODO
  goto BadOpcode;
}
LoadObjectField: {
  // TODO
  goto BadOpcode;
}
StoreObjectField: {
  // TODO
  goto BadOpcode;
}
Deallocate: {
  // TODO
  goto BadOpcode;
}
Allocate: {
  // TODO
  goto BadOpcode;
}
AllocateImm: {
  // TODO
  goto BadOpcode;
}

  __builtin_unreachable();
}
