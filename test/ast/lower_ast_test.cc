#include "gtest/gtest.h"

#include "src/ast/ast.hh"
#include "src/ast/bytecode_emitter.hh"
#include "src/bytecode/bytecode.hh"
#include "src/bytecode/interpreter.hh"

namespace {

TEST(LowerAstTest, NopTest) {
  // Test Program in AST
  ast::ProgramNode program {
    .statements{{ast::NopNode()}},
  };
  // Expected Instructions
  std::vector<bytecode::Instruction> instructions {{
    { bytecode::Opcode::kNop, 0 },
  }};
  auto executable = ast::LowerAst(program);
  EXPECT_EQ(instructions.size(), executable.chunks[0].first.code.size());
  for (unsigned i = 0; i < instructions.size(); ++i) {
    EXPECT_EQ(instructions[i].opcode, executable.chunks[0].first.code[i].opcode);
    EXPECT_EQ(instructions[i].param, executable.chunks[0].first.code[i].param);
  }
}

}