#pragma once

#include "src/ast/ast.hh"
#include <string_view>

namespace parser {
ast::ProgramNode ParseFullProgram(std::string_view program_text);
} // namespace parser
