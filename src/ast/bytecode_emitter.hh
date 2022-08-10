#pragma once

#include "src/ast/ast.hh"
#include "src/bytecode/bytecode.hh"
#include "src/bytecode/opcodes.hh"

#include "ANTLRInputStream.h"

namespace ast {

// TODO I think this should actually return a vector of chunks, and then we
// could have a pseudo-linking step connect those together (????maybe????)
bytecode::BytecodeExecutable LowerAst(const ProgramNode& ast, antlr4::ANTLRInputStream& input);

} // namespace ast;
