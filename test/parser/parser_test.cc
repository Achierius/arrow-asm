#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "src/parser/parser.hh"

namespace {
TEST(ParserTest, ParserTestSimple) {
    std::ifstream stream("./test/test_programs/statements.aasm", std::ios::in);
    std::stringstream ss;
    ss << stream.rdbuf();

    auto ast = parser::ParseFullProgram(ss.str());
    EXPECT_EQ(ast.statements.size(), 3);
    EXPECT_EQ(std::holds_alternative<ast::FunctionNode>(*ast.statements[0]), true);
    EXPECT_EQ(std::holds_alternative<ast::FunctionNode>(*ast.statements[1]), true);
    EXPECT_EQ(std::holds_alternative<ast::FunctionNode>(*ast.statements[2]), true);
}
}