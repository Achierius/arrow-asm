#include "gtest/gtest.h"

#include "src/bytecode/bytecode.hh"
#include "src/bytecode/interpreter.hh"
#include "src/bytecode/opcodes.hh"
#include "spdlog/cfg/env.h"

namespace {

using namespace bytecode;

BytecodeExecutable makeFlatBinary( std::vector<Instruction> code) {
  BytecodeChunk chunk {
    .code = code,
  };
  return BytecodeExecutable {
    .chunks {
      std::make_pair(chunk, 0)
    },
  };
}

TEST(Bytecode, BasicTest) {
  spdlog::cfg::load_env_levels();
  auto bin = makeFlatBinary( {
      { kImmByte, 3 },
      { kReturn },
  });
  int return_value = InterpretBytecode(bin);
  EXPECT_EQ(return_value, 0);
}

}
