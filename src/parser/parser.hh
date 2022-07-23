#pragma once

namespace parser {
  ast::ProgramNode ParseFullProgram(std::string_view program_text);
}
