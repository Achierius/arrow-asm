// copyright sun microsystems, c 1970

#pragma once

#include <string_view>
#include <variant>
#include <vector>
#include <memory>
#include <optional>
#include <string>

namespace ast {

enum AstNodeType {
  kNop = 0,
  kProgram,
  kStatement,
  kFunction,
  kType,
  kCtor,
  kDtor,
  kField,
  kId,
  kRegisterType,
  kObjectType,
  kLong,
  kDouble,
  kPtr,
  kInstruction,
  kArrowLhs,
  kArrowRhs,
  kArrow,
  kLValue,
  kRValue,
  kRegister,
  kMakeNode,
  kImm,
  kArg,
  kInst,
  kCallInst,
  kNoRetInst,
  kNoArgInst,
  kBinaryInst,
  kMemoryInst,
  kIfStmt,
  kElif,
  kElse,
  kMake
};

template <AstNodeType node_type> struct AstNode {
  constexpr static AstNodeType type = node_type;
  
  //std::string_view source_location; // TODO(marcus@)
};

struct NopNode : public AstNode<kNop> {};
struct IdNode : public AstNode<kId> {
  std::string id;
};

// Type Nodes
struct ObjectTypeNode;
struct LongNode : public AstNode<kLong> {
  bool operator==(LongNode const&) const {
    return true;
  }
};
struct DoubleNode : public AstNode<kDouble> {
  bool operator==(DoubleNode const&) const {
    return true;
  }
};
struct PtrNode : public AstNode<kPtr> {
  std::shared_ptr<ObjectTypeNode> element_type;
  bool operator==(PtrNode const& o) const {
    return true;
  }
};
struct RegisterTypeNode : public AstNode<kRegisterType>,
                          public std::variant<std::monostate, LongNode, DoubleNode, PtrNode> {
  // using std::variant<std::monostate, LongNode, DoubleNode, PtrNode>::operator==;
  using std::variant<std::monostate, LongNode, DoubleNode, PtrNode>::variant;
  using std::variant<std::monostate, LongNode, DoubleNode, PtrNode>::operator=;
};
struct ObjectTypeNode : public AstNode<kObjectType>,
                        public std::variant<std::monostate, LongNode, DoubleNode, PtrNode, IdNode> {
  using std::variant<std::monostate, LongNode, DoubleNode, PtrNode, IdNode>::variant;
  using std::variant<std::monostate, LongNode, DoubleNode, PtrNode, IdNode>::operator=;
};

// Register node
enum class RegisterCategory {
  Local,
  Param,
  OutgoingParam,
  Return,
  Static
};
struct RegisterNode : public AstNode<kRegister> {
  // TODO: Add type?
  RegisterCategory category;
  uint8_t register_id;
};

// Instruction nodes
struct LValueNode : public AstNode<kLValue>, public RegisterNode {};
struct RValueNode : public AstNode<kRValue>, public RegisterNode {};
struct MemberNode : public AstNode<kField> {
  RValueNode obj;
  IdNode type;
  IdNode field;
};
struct ArrowLhsNode : public AstNode<kArrowLhs>, public std::variant<std::monostate, LValueNode, MemberNode> {
  using std::variant<std::monostate, LValueNode, MemberNode>::variant;
  using std::variant<std::monostate, LValueNode, MemberNode>::operator=;
};
struct MakeNode : public AstNode<kMake> {
  // We can 'box' all sorts of stuff, including other ptrs or primitives
  ObjectTypeNode type;
};
struct ArrowRhsNode : public AstNode<kArrowRhs>, public std::variant<std::monostate, RValueNode, MemberNode, MakeNode> {
  using std::variant<std::monostate, RValueNode, MemberNode, MakeNode>::variant;
  using std::variant<std::monostate, RValueNode, MemberNode, MakeNode>::operator=;
};
struct ArrowInstNode : public AstNode<kArrow> {
  ArrowLhsNode lhs;
  ArrowRhsNode rhs;
};
struct ImmediateNode : public AstNode<kImm>, public std::variant<std::monostate, int64_t, double> {
  using std::variant<std::monostate, int64_t, double>::variant;
  using std::variant<std::monostate, int64_t, double>::operator=;
};
struct ArgNode : public AstNode<kArg>, public std::variant<std::monostate, RValueNode, ImmediateNode> {
  using std::variant<std::monostate, RValueNode, ImmediateNode>::variant;
  using std::variant<std::monostate, RValueNode, ImmediateNode>::operator=;
};
// Operator/Instruction Nodes
// NoArgNode
enum class NoArgOperator {
  kTrap,
  kRet,
  kBreak,
  kContinue,
};
struct NoArgNode : public AstNode<kNoArgInst> {
  NoArgOperator op;
};
// CallNode
struct CallNode : public AstNode<kCallInst> {
  IdNode id;
};
// NoRetNode
enum class NoRetOperator {
  kPrint,
  kExit,
};
struct NoRetNode : public AstNode<kNoRetInst> {
  NoRetOperator op;
  ArgNode arg;
};
// BinaryNode
enum class BinaryOperator {
  kAdd,
  kSub,
  kMul,
  kDiv,
  kMod,
  kSll,
  kSrl,
  kSra,
  kAnd,
  kOr,
};
struct BinaryNode : public AstNode<kBinaryInst> {
  LValueNode lhs;
  ArgNode arg1;
  ArgNode arg2;
  BinaryOperator op;
};
// MemoryNode
enum class MemoryOperator {
  kLoad,
  kStore
};
struct MemoryNode : public AstNode<kMemoryInst> {
  LValueNode register_dst;
  std::variant<std::monostate, RValueNode, MemberNode> memory_location;
  MemoryOperator op;
};
// IfNode
struct InstructionNode;
struct ElifNode : public AstNode<kElif> {
  ArgNode condition;
  std::vector<std::shared_ptr<InstructionNode>> body;
};
struct ElseNode : public AstNode<kElse> {
  std::vector<std::shared_ptr<InstructionNode>> body;
};
struct IfNode : public AstNode<kIfStmt> {
  ArgNode condition;
  std::vector<ElifNode> elifs;
  std::optional<ElseNode> else_node;
  std::vector<std::shared_ptr<InstructionNode>> body;
};

struct InstructionNode : public AstNode<kInstruction>,
                         public std::variant<std::monostate, ArrowInstNode, CallNode, NoArgNode, NoRetNode,
                                             BinaryNode, MemoryNode, IfNode> {
  using std::variant<std::monostate, ArrowInstNode, CallNode, NoArgNode, NoRetNode, BinaryNode, MemoryNode, IfNode>::variant;
  using std::variant<std::monostate, ArrowInstNode, CallNode, NoArgNode, NoRetNode, BinaryNode, MemoryNode, IfNode>::operator=;
};

// Function nodes
struct FunctionNode : public AstNode<kFunction> {
  IdNode id;
  std::vector<std::shared_ptr<RegisterTypeNode>> params;
  std::vector<std::shared_ptr<InstructionNode>> body;
};

// Type definition nodes
struct FieldNode : public AstNode<kField> {
  IdNode id;
  RegisterTypeNode type;
};
struct CtorNode : public AstNode<kCtor> {
  std::vector<std::shared_ptr<RegisterTypeNode>> params;
  std::vector<std::shared_ptr<InstructionNode>> body;
};
struct DtorNode : public AstNode<kDtor> {
  std::vector<std::shared_ptr<InstructionNode>> body;
};
struct TypeNode : public AstNode<kFunction> {
  IdNode id;
  std::optional<CtorNode> ctor;
  std::optional<DtorNode> dtor;
  std::vector<std::shared_ptr<FieldNode>> fields;
};

struct StatementNode : public AstNode<kStatement>,
                       public std::variant<std::monostate, NopNode, FunctionNode, TypeNode> {
  using std::variant<std::monostate, NopNode, FunctionNode, TypeNode>::variant;
  using std::variant<std::monostate, NopNode, FunctionNode, TypeNode>::operator=;
};

struct ProgramNode : public AstNode<kProgram> {
  std::vector<std::shared_ptr<StatementNode>> statements;
};

} // namespace ast
