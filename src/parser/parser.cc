#include "parser.hh"

#include "src/ast/ast.hh"

ast::ProgramNode parser::ParseFullProgram(std::string_view program_text) {
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
