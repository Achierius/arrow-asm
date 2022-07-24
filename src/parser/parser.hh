#pragma once

// headers defined in sibling nodes need to be fully-qualified from project root
#include "src/ast/ast.hh"
#include "src/parser/BeautifulAsmLexer.h"
#include "src/parser/BeautifulAsmParser.h"
#include <string>

namespace parser {
ast::ProgramNode ParseFullProgram(std::string program_text);
} // namespace parser
