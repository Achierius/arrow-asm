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
      chunk,
    },
    .chunk_locations {
      0ull,
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

TEST(Bytecode, ConstantTestBasic) {
  spdlog::cfg::load_env_levels();
  auto bin = makeFlatBinary( {
      { kConstant, 1 },
      { kBiasConstantWindow, 2 },
      { kConstant, 0 },
      { kAddLong },
      { kReturn },
  });
  bin.chunks[0].constants.push_back(1);
  bin.chunks[0].constants.push_back(10);
  bin.chunks[0].constants.push_back(100);
  bin.chunks[0].constants.push_back(1000);
  int return_value = InterpretBytecode(bin);
  EXPECT_EQ(return_value, 110);
}

}
