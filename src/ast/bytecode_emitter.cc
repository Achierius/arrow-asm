#include "bytecode_emitter.hh"

#include <cassert>
#include <cstddef>
#include <utility>

#include "src/bytecode/opcodes.hh"

bytecode::BytecodeExecutable ast::LowerAst(const ProgramNode& ast) {
  // TODO actually recurse over ast
  assert(ast.statements.size() == 1);
  // assert(std::holds_alternative<ast::NopNode>(ast.statements[0]));
  bytecode::Instruction nop {
    .opcode = bytecode::Opcode::kNop,
    .param = 0,
  };
  bytecode::BytecodeChunk chunk {
    .code = {nop},
  };
  bytecode::BytecodeExecutable exe {
    .chunks = {chunk},
    .chunk_locations = {0ull},
  };
  return exe;
}
