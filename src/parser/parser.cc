#include "parser.hh"

#include <sstream>

#include "src/ast/ast.hh"

ast::ProgramNode parser::ParseFullProgram(std::string_view program_text) {
  std::stringstream stream(program_text.data());

  antlr4::ANTLRInputStream input(stream);
  parser::BeautifulAsmLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  parser::BeautifulAsmParser parser(&tokens);

  // TODO antlr
  if (program_text == "nop\n") {
    ast::ProgramNode program_node {
      .statements{std::make_unique<ast::NopNode>()},
    };
    return program_node;
  }
  return {};
};
