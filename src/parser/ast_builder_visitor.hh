#include "src/parser/BeautifulAsmBaseVisitor.h"

#include "src/ast/ast.hh"

namespace parser {
  class ASTBuilderVisitor : public BeautifulAsmVisitor {
    public:
      ast::ProgramNode program_ast;
  }
}