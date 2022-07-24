#include "src/parser/BeautifulAsmBaseVisitor.h"

namespace parser {

  virtual std::any visitProgram(BeautifulAsmParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(BeautifulAsmParser::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

}