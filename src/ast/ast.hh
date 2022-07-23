// copyright sun microsystems, c 1970

#pragma once

#include <string_view>
#include <variant>
#include <vector>

namespace ast {

enum AstNodeType {
  kProgram = 0,
  kStatement,
  kNop,
};

template <AstNodeType node_type> struct BaseAstNode {
  constexpr static AstNodeType type = node_type;
  std::string_view source_location; // TODO(marcus@)
};

struct NopNode : public BaseAstNode<kNop> {};

struct StatementNode : public BaseAstNode<kNop>,
                       public std::variant<std::monostate, NopNode> {};

struct ProgramNode : public BaseAstNode<kProgram> {
  std::vector<StatementNode> statements;
};

} // namespace ast
