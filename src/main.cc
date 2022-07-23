// copyright Valkyrie AFTERSCHOOL INTERNATIONAL(c) 2022(tm) (r)

#include <cassert>
#include <iostream>
#include <variant>

// headers defined by targets that are child nodes in the
// directory tree can be referred to by the package path directly
#include "ast/ast.hh"
#include "parser/parser.hh"

int main(int argc, char *argv[]) {
  auto program = parser::ParseFullProgram("nop\n");
  assert(program.statements.size() == 1);
  assert(std::holds_alternative<ast::NopNode>(program.statements[0]));
  return 0;
}
