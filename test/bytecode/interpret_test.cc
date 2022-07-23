#include "gtest/gtest.h"

#include "src/bytecode/bytecode.hh"
#include "src/bytecode/interpreter.hh"

namespace {

TEST(InterpretBytecodeTest, OneChunkOneNopTest) {
  // Test Program
  bytecode::BytecodeChunk chunk {
    .code{{
      { bytecode::Opcode::kNop, 0 },
    }},
    .constant_pool{},
    .symbol_pool{},
  };

  bytecode::BytecodeExecutable executable {
    .chunks{{ chunk, 0ull }},
    .globals{},
    .symbol_table{},
  };

  int return_code = bytecode::InterpretBytecode(executable);
  EXPECT_EQ(return_code, 0);
}

}