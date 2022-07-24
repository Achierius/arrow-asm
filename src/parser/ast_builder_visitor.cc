#include "src/parser/ast_builder_visitor.hh"
#include "src/parser/BeautifulAsmBaseVisitor.h"

#include <iostream>

namespace parser {

  std::any ASTBuilderVisitor::visitProgram(BeautifulAsmParser::ProgramContext *ctx) {
    std::cout << "program" << std::endl;
    for (auto statement : ctx->statement()) {
      visitStatement(statement);
    }
  }

  std::any ASTBuilderVisitor::visitStatement(BeautifulAsmParser::StatementContext *ctx) {
    // if (ctx->function_definition()) {
    //   visitFunction_definition(ctx->function_definition());
    // } else if (ctx->type_definition()) {
    //   visitType_definition(ctx->type_definition());
    // }
    return visitChildren(ctx);
  }

  std::any ASTBuilderVisitor::visitFunction_definition(BeautifulAsmParser::Function_definitionContext *ctx) {
    // if (ctx->function_body()) {
    //   visitFunction_body(ctx->function_body());
    // }
    return visitChildren(ctx);
  }

  std::any ASTBuilderVisitor::visitType_definition(BeautifulAsmParser::Type_definitionContext *ctx) {
    return visitChildren(ctx);
  }

  std::any ASTBuilderVisitor::visitFunction_body(BeautifulAsmParser::Function_bodyContext *ctx) {
    // for (auto instruction : ctx->instruction()) {
    //   visitInstruction(instruction);
    // }
    return visitChildren(ctx);
  }
  
  std::any ASTBuilderVisitor::visitInstruction(BeautifulAsmParser::InstructionContext *ctx) {
    // if (ctx->binary_operator_instruction()) {
    //   visitBinary_operator_instruction(ctx->binary_operator_instruction());
    // }
    return visitChildren(ctx);
  }

  // std::any ASTBuilderVisitor::visitNo_arg_instruction(BeautifulAsmParser::No_arg_instructionContext *ctx) {
  // }

  // std::any ASTBuilderVisitor::visitArrow_instruction(BeautifulAsmParser::Arrow_instructionContext *ctx) {
  // }

  // std::any ASTBuilderVisitor::visitPrint_instruction(BeautifulAsmParser::Print_instructionContext *ctx) {
  // }

  std::any ASTBuilderVisitor::visitBinary_operator_instruction(BeautifulAsmParser::Binary_operator_instructionContext *ctx) {
    std::cout << ctx->getText() << std::endl;
    return visitChildren(ctx);
  }

  // std::any ASTBuilderVisitor::visitMemory_instruction(BeautifulAsmParser::Memory_instructionContext *ctx) {
  // }

  std::any ASTBuilderVisitor::visitBinary_operator(BeautifulAsmParser::Binary_operatorContext *ctx) {
    
  }
}