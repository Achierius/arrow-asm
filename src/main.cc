// copyright Valkyrie AFTERSCHOOL INTERNATIONAL(c) 2022(tm) (r)

#include <cassert>
#include <iostream>
#include <variant>

#include "ast/ast.hh"
#include "parser/parser.hh"

int main(int argc, char *argv[]) {
  auto program = parser::ParseFullProgram("nop\n");
  assert(program.statements.size() == 1);
  assert(std::holds_alternative<ast::NopNode>(program.statements[0]));
  return 0;
}
