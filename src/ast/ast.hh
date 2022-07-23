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

template <AstNodeType node_type> struct AstNode {
  constexpr static AstNodeType type = node_type;
  //std::string_view source_location; // TODO(marcus@)
};

struct NopNode : public AstNode<kNop> {};

struct StatementNode : public AstNode<kNop>,
                       public std::variant<std::monostate, NopNode> {
 using std::variant<std::monostate, NopNode>::operator=;
};

struct ProgramNode : public AstNode<kProgram> {
  std::vector<StatementNode> statements;
};

} // namespace ast
