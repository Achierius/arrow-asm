#include "src/parser/BeautifulAsmBaseVisitor.h"

#include "src/ast/ast.hh"

namespace parser {
  class ASTBuilderVisitor : public BeautifulAsmBaseVisitor {
    public:
      ast::ProgramNode program_ast;

      virtual std::any visitProgram(BeautifulAsmParser::ProgramContext *ctx);
      virtual std::any visitStatement(BeautifulAsmParser::StatementContext *ctx);
      virtual std::any visitFunction_definition(BeautifulAsmParser::Function_definitionContext *ctx);
      virtual std::any visitType_definition(BeautifulAsmParser::Type_definitionContext *ctx);
      virtual std::any visitFunction_body(BeautifulAsmParser::Function_bodyContext *ctx);
      virtual std::any visitInstruction(BeautifulAsmParser::InstructionContext *ctx);
      // virtual std::any visitNo_arg_instruction(BeautifulAsmParser::No_arg_instructionContext *ctx);
      // virtual std::any visitArrow_instruction(BeautifulAsmParser::Arrow_instructionContext *ctx);
      // virtual std::any visitPrint_instruction(BeautifulAsmParser::Print_instructionContext *ctx);
      virtual std::any visitBinary_operator_instruction(BeautifulAsmParser::Binary_operator_instructionContext *ctx);
      // virtual std::any visitMemory_instruction(BeautifulAsmParser::Memory_instructionContext *ctx);
  };
}