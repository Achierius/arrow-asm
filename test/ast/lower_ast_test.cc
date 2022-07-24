#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "src/parser/parser.hh"
#include "src/ast/ast.hh"
#include "src/ast/bytecode_emitter.hh"
#include "src/bytecode/bytecode.hh"

namespace {

TEST(LowerAstTest, NopTest) {
  // Test Program
  std::ifstream stream("./test/test_programs/simple.aasm", std::ios::in);
  std::stringstream ss;
  ss << stream.rdbuf();

  // Expected Instructions
  std::vector<bytecode::Instruction> instructions {{
    { bytecode::Opcode::kNop, 0 },
  }};

  auto program = parser::ParseFullProgram(ss.str());
  auto executable = ast::LowerAst(program);
}

}