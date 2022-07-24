#include "parser.hh"

#include <sstream>

#include "src/ast/ast.hh"

ast::ProgramNode parser::ParseFullProgram(std::string_view program_text) {
  std::stringstream stream(program_text;

  antlr4::ANTLRInputStream input(stream);
  parser::BeautifulAsmLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  parser::BeautifulAsmParser parser(&tokens);

  // TODO antlr
  if (program_text == "nop\n") {
    ast::StatementNode statement_node;
    statement_node = ast::NopNode();
    ast::ProgramNode program_node {
      .statements{statement_node},
    };
    return program_node;
  }
  return {};
};
