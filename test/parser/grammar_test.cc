#include "gtest/gtest.h"

#include <fstream>
#include <string>
#include <iostream>

#include "src/parser/parser.hh"

namespace {
TEST(ParserTest, GrammarTestSimple) {
  static constexpr auto test_add =
    "fn test_add(long, long) { \
      add p0, p0, p1 \
      ret \
    }";
  // std::fstream stream("test/test_programs/simple.aasm");
  std::stringstream stream(test_add);

  antlr4::ANTLRInputStream input(stream);
  parser::BeautifulAsmLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  // parser::BeautifulAsmParser parser(&tokens);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }
}
}