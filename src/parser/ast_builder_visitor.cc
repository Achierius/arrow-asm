#include "src/parser/ast_builder_visitor.hh"
#include "src/parser/BeautifulAsmBaseVisitor.h"

#include <memory>
#include <iostream>
#include <string>

namespace parser {

  std::any ASTBuilderVisitor::visitProgram(BeautifulAsmParser::ProgramContext *ctx) {
    auto node = ast::ProgramNode();
    for (auto statement : ctx->statement()) {
      node.statements.push_back(std::make_shared<ast::StatementNode>(std::any_cast<ast::StatementNode>(visitStatement(statement))));
    }
    return node;
  }

  std::any ASTBuilderVisitor::visitFunction_definition(BeautifulAsmParser::Function_definitionContext *ctx) {
    auto node = ast::FunctionNode();
    node.id = ast::IdNode{ .id{ctx->ID()->getText()} };
    if (ctx->parameter_list()) {
      node.params = std::any_cast<std::vector<std::shared_ptr<ast::RegisterTypeNode>>>(visitParameter_list(ctx->parameter_list()));
    }
    if (ctx->instructions()) {
      node.body = std::any_cast<std::vector<std::shared_ptr<ast::InstructionNode>>>(visitInstructions(ctx->instructions()));
    }
    return ast::StatementNode(node);
  }

  std::any ASTBuilderVisitor::visitParameter_list(BeautifulAsmParser::Parameter_listContext *ctx) {
    std::vector<std::shared_ptr<ast::RegisterTypeNode>> param_list;
    for (auto register_type : ctx->register_type()) {
      auto param = std::any_cast<ast::RegisterTypeNode>(visitRegister_type(register_type));
      param_list.push_back(std::make_shared<ast::RegisterTypeNode>(param));
    }
    return param_list;
  }

  std::any ASTBuilderVisitor::visitType_definition(BeautifulAsmParser::Type_definitionContext *ctx) {
    ast::TypeNode node;
    for (auto ctor : ctx->constructor()) {
      node.ctor = ast::CtorNode{
        .params{
          (ctor->parameter_list())
            ? std::any_cast<std::vector<std::shared_ptr<ast::RegisterTypeNode>>>(visitParameter_list(ctor->parameter_list()))
            : std::vector<std::shared_ptr<ast::RegisterTypeNode>>()
        },
        .body{
          std::any_cast<std::vector<std::shared_ptr<ast::InstructionNode>>>(visitInstructions(ctor->instructions()))
        }
      };
    }

    for (auto dtor : ctx->destructor()) {
      node.dtor = ast::DtorNode{
        .body{
          std::any_cast<std::vector<std::shared_ptr<ast::InstructionNode>>>(visitInstructions(dtor->instructions()))
        }
      };
    }
    for (auto field : ctx->field()) {
      node.fields.push_back(std::make_shared<ast::FieldNode>(std::any_cast<ast::FieldNode>(visitField(field))));
    }
    return ast::StatementNode(node);
  }

  std::any ASTBuilderVisitor::visitInstructions(BeautifulAsmParser::InstructionsContext *ctx) {
    std::vector<std::shared_ptr<ast::InstructionNode>> instructions;
    for (auto instruction : ctx->instruction()) {
      instructions.push_back(std::any_cast<std::shared_ptr<ast::InstructionNode>>(visitInstruction(instruction)));
    }
    return instructions;
  }

  std::any ASTBuilderVisitor::visitInstruction(BeautifulAsmParser::InstructionContext *ctx) {
    if (ctx->binary_operator_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::BinaryNode>(visitChildren(ctx)));
    if (ctx->call_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::CallNode>(visitChildren(ctx)));
    if (ctx->print_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::NoRetNode>(visitChildren(ctx)));
    if (ctx->memory_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::MemoryNode>(visitChildren(ctx)));
    if (ctx->arrow_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::ArrowInstNode>(visitChildren(ctx)));
    if (ctx->no_arg_instruction())
      return std::make_shared<ast::InstructionNode>(std::any_cast<ast::NoArgNode>(visitChildren(ctx)));
    return nullptr;
  }
  
  std::any ASTBuilderVisitor::visitNo_arg_instruction(BeautifulAsmParser::No_arg_instructionContext *ctx) {
    auto text = ctx->getText();
    if      (text == "trap") return ast::NoArgNode{ .op = ast::NoArgOperator::kTrap };
    else if (text == "ret") return ast::NoArgNode{ .op = ast::NoArgOperator::kRet };
    else if (text == "break") return ast::NoArgNode{ .op = ast::NoArgOperator::kBreak };
    else if (text == "continue") return ast::NoArgNode{ .op = ast::NoArgOperator::kContinue };
    return {};
  }

  std::any ASTBuilderVisitor::visitArrow_instruction(BeautifulAsmParser::Arrow_instructionContext *ctx) {
    ast::ArrowInstNode node;
    node.lhs = std::any_cast<ast::ArrowLhsNode>(visitArrow_lhs(ctx->arrow_lhs()));
    node.rhs = std::any_cast<ast::ArrowRhsNode>(visitArrow_rhs(ctx->arrow_rhs()));
    return node; 
  }
  
  std::any ASTBuilderVisitor::visitArrow_lhs(BeautifulAsmParser::Arrow_lhsContext *ctx) {
    if (ctx->any_field())
      return ast::ArrowLhsNode{std::any_cast<ast::MemberNode>(visitChildren(ctx))};
    if (ctx->any_lvalue())
      return ast::ArrowLhsNode{std::any_cast<ast::LValueNode>(visitChildren(ctx))};
    return {};
  }
    
  std::any ASTBuilderVisitor::visitArrow_rhs(BeautifulAsmParser::Arrow_rhsContext *ctx) {
    if (ctx->any_field())
      return ast::ArrowRhsNode{std::any_cast<ast::MemberNode>(visitChildren(ctx))};
    if (ctx->any_rvalue())
      return ast::ArrowRhsNode{std::any_cast<ast::RValueNode>(visitChildren(ctx))};
    if (ctx->make_constructor())
      return ast::ArrowRhsNode{std::any_cast<ast::MakeNode>(visitChildren(ctx))};
    return {};
  }

  std::any ASTBuilderVisitor::visitMake_constructor(BeautifulAsmParser::Make_constructorContext *ctx) {
    return ast::MakeNode{ .type{std::any_cast<ast::ObjectTypeNode>(visitChildren(ctx))} };
  }
    
  std::any ASTBuilderVisitor::visitCall_instruction(BeautifulAsmParser::Call_instructionContext *ctx) {
    return ast::CallNode{ .id{ ast::IdNode{ .id = ctx->ID()->getText() } } };
  }
  
  std::any ASTBuilderVisitor::visitPrint_instruction(BeautifulAsmParser::Print_instructionContext *ctx) {
    return ast::NoRetNode{ .op = ast::NoRetOperator::kPrint, .arg = std::any_cast<ast::ArgNode>(visitChildren(ctx)) };
  }

  std::any ASTBuilderVisitor::visitBinary_operator_instruction(BeautifulAsmParser::Binary_operator_instructionContext *ctx) {
    ast::BinaryNode node;
    node.op = std::any_cast<ast::BinaryOperator>(visitBinary_operator(ctx->binary_operator()));
    node.lhs = std::any_cast<ast::LValueNode>(visitAny_lvalue(ctx->arg1));
    node.arg1 = std::any_cast<ast::ArgNode>(visitAny_argument(ctx->arg2));
    node.arg2 = std::any_cast<ast::ArgNode>(visitAny_argument(ctx->arg3));
    return node;
  }

  std::any ASTBuilderVisitor::visitMemory_instruction(BeautifulAsmParser::Memory_instructionContext *ctx) {
    ast::MemoryNode node;
    node.op = std::any_cast<ast::MemoryOperator>(visitMemory_operator(ctx->memory_operator()));
    node.register_dst = std::any_cast<ast::LValueNode>(visitAny_lvalue(ctx->arg1));
    node.memory_location = std::any_cast<std::variant<std::monostate, ast::RValueNode, ast::MemberNode>>(visitMemory_destination(ctx->arg2));
    return node;
  }

  std::any ASTBuilderVisitor::visitBinary_operator(BeautifulAsmParser::Binary_operatorContext *ctx) {
    if (ctx->getText() == "add") return ast::BinaryOperator::kAdd;
    if (ctx->getText() == "mul") return ast::BinaryOperator::kMul;
    return {};
  }

  std::any ASTBuilderVisitor::visitMemory_operator(BeautifulAsmParser::Memory_operatorContext *ctx) {
    if (ctx->getText() == "load")  return ast::MemoryOperator::kLoad;
    if (ctx->getText() == "store") return ast::MemoryOperator::kStore;
    return {};
  }
    
  std::any ASTBuilderVisitor::visitMemory_destination(BeautifulAsmParser::Memory_destinationContext *ctx) {
    if (ctx->any_field())
      return std::variant<std::monostate, ast::RValueNode, ast::MemberNode>{std::any_cast<ast::MemberNode>(visitChildren(ctx))};
    if (ctx->any_rvalue())
      return std::variant<std::monostate, ast::RValueNode, ast::MemberNode>{std::any_cast<ast::RValueNode>(visitChildren(ctx))};
    return {};
  }

  std::any ASTBuilderVisitor::visitAny_lvalue(BeautifulAsmParser::Any_lvalueContext *ctx) {
    auto text = ctx->getText();
    ast::LValueNode node;
    if      (text.starts_with("l"))  { node.category = ast::RegisterCategory::Local;         node.register_id = std::stoi(text.substr(1)); }
    else if (text.starts_with("p"))  { node.category = ast::RegisterCategory::Param;         node.register_id = std::stoi(text.substr(1)); }
    else if (text.starts_with("op"))  { node.category = ast::RegisterCategory::OutgoingParam; node.register_id = std::stoi(text.substr(2)); }
    else if (text.starts_with("rr")) { node.category = ast::RegisterCategory::Return;        node.register_id = std::stoi(text.substr(2)); }
    else if (text.starts_with("sr")) { node.category = ast::RegisterCategory::Static;        node.register_id = std::stoi(text.substr(2)); }
    return node;
  }

  std::any ASTBuilderVisitor::visitAny_argument(BeautifulAsmParser::Any_argumentContext *ctx) {
    if (ctx->any_rvalue())
      return ast::ArgNode(std::any_cast<ast::RValueNode>(visitAny_rvalue(ctx->any_rvalue())));
    if (ctx->any_number())
      return ast::ArgNode(std::any_cast<ast::ImmediateNode>(visitAny_number(ctx->any_number())));
    return {};
  }

  std::any ASTBuilderVisitor::visitAny_rvalue(BeautifulAsmParser::Any_rvalueContext *ctx) {
    auto text = ctx->getText();
    ast::RValueNode node;
    if      (text.starts_with("l"))  { node.category = ast::RegisterCategory::Local;         node.register_id = std::stoi(text.substr(1)); }
    else if (text.starts_with("p"))  { node.category = ast::RegisterCategory::Param;         node.register_id = std::stoi(text.substr(1)); }
    else if (text.starts_with("o"))  { node.category = ast::RegisterCategory::OutgoingParam; node.register_id = std::stoi(text.substr(1)); }
    else if (text.starts_with("rr")) { node.category = ast::RegisterCategory::Return;        node.register_id = std::stoi(text.substr(2)); }
    else if (text.starts_with("sr")) { node.category = ast::RegisterCategory::Static;        node.register_id = std::stoi(text.substr(2)); }
    return node;
  }

  std::any ASTBuilderVisitor::visitAny_number(BeautifulAsmParser::Any_numberContext *ctx) {
    if (ctx->getText().find('.'))
      return ast::ImmediateNode(std::stod(ctx->getText()));
    return ast::ImmediateNode(std::stol(ctx->getText()));
  }

  std::any ASTBuilderVisitor::visitAny_field(BeautifulAsmParser::Any_fieldContext *ctx) {
    return ast::MemberNode{
      .obj{std::any_cast<ast::RValueNode>(visitAny_rvalue(ctx->any_rvalue()))},
      .type{ast::IdNode{.id = ctx->type_name->getText()}},
      .field{ast::IdNode{.id = ctx->field_name->getText()}}
    };
  }

  std::any ASTBuilderVisitor::visitObject_type(BeautifulAsmParser::Object_typeContext *ctx) {
    auto text = ctx->getText();
    if      (text.starts_with("long")) { return ast::ObjectTypeNode{ ast::LongNode() }; }
    else if (text.starts_with("double")) { return ast::ObjectTypeNode{ ast::DoubleNode() }; }
    else if (text.starts_with("ptr"))    {
      auto element_type = std::any_cast<ast::ObjectTypeNode>(visitObject_type(ctx->object_type()));
      return ast::ObjectTypeNode{ ast::PtrNode{
        .element_type{ std::make_shared<ast::ObjectTypeNode>(element_type) }
      }};
    }
    return ast::ObjectTypeNode{ ast::IdNode{ .id = ctx->ID()->getText() } };
  }

  std::any ASTBuilderVisitor::visitRegister_type(BeautifulAsmParser::Register_typeContext *ctx) {
    auto text = ctx->getText();
    if      (text.starts_with("long"))   { return ast::RegisterTypeNode{ ast::LongNode() }; }
    else if (text.starts_with("double")) { return ast::RegisterTypeNode{ ast::DoubleNode() }; }
    else if (text.starts_with("ptr"))    {
      auto element_type = std::any_cast<ast::ObjectTypeNode>(visitObject_type(ctx->object_type()));
      return ast::RegisterTypeNode{ ast::PtrNode{
        .element_type{ std::make_shared<ast::ObjectTypeNode>(element_type) }
      }};
    }
    return {}; 
  }

  std::any ASTBuilderVisitor::visitField(BeautifulAsmParser::FieldContext *ctx) {
    return ast::FieldNode{
      .id = ast::IdNode{ .id = ctx->ID()->getText() },
      .type{std::any_cast<ast::RegisterTypeNode>(visitRegister_type(ctx->register_type()))}
    };
  }

}