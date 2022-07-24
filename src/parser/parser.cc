#include "parser.hh"

#include <sstream>

#include "src/ast/ast.hh"
#include "src/parser/ast_builder_visitor.hh"


ast::ProgramNode parser::ParseFullProgram(std::string program_text) {
  std::stringstream stream(program_text);

  antlr4::ANTLRInputStream input(stream);
  parser::BeautifulAsmLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  parser::BeautifulAsmParser parser(&tokens);

  auto program = parser.program();
  ASTBuilderVisitor visitor;
  visitor.visitProgram(program);

  return {};

  // TODO antlr
  if (program_text == "nop\n") {
    ast::ProgramNode program_node {
      .statements{std::make_shared<ast::StatementNode>(ast::NopNode{})},
    };
    return program_node;
  }
  return {};
};
