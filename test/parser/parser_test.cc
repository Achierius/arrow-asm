#include "gtest/gtest.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "src/parser/parser.hh"

namespace {
TEST(ParserTest, ParserTestSimple) {
    std::ifstream stream("./test/test_programs/simple.aasm", std::ios::in);
    std::stringstream ss;
    ss << stream.rdbuf();

    std::cout << ss.str() << std::endl;
    auto ast = parser::ParseFullProgram(ss.str());
}
}