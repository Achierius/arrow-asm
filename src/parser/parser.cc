#include "parser.hh"

#include <sstream>

#include "src/ast/ast.hh"
#include "src/parser/ast_builder_visitor.hh"

std::tuple<ast::ProgramNode, antlr4::ANTLRInputStream> parser::ParseFullProgram(std::string program_text) {
  std::stringstream stream(program_text);

  antlr4::ANTLRInputStream input(stream);
  parser::AasmLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  parser::AasmParser parser(&tokens);

  auto program = parser.program();
  ASTBuilderVisitor visitor;

  return std::make_tuple(std::any_cast<ast::ProgramNode>(visitor.visitProgram(program)), input);
};
