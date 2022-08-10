#pragma once

// headers defined in sibling nodes need to be fully-qualified from project root
#include "src/ast/ast.hh"
#include "src/parser/AasmLexer.h"
#include "src/parser/AasmParser.h"
#include "src/parser/ast_builder_visitor.hh"
#include <string>
#include <tuple>

namespace parser {
std::tuple<ast::ProgramNode, antlr4::ANTLRInputStream> ParseFullProgram(std::string program_text);
} // namespace parser
