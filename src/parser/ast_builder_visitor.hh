#include "src/parser/BeautifulAsmBaseVisitor.h"

#include "src/ast/ast.hh"

namespace parser {
  class ASTBuilderVisitor : public BeautifulAsmBaseVisitor {
    public:
      virtual std::any visitProgram(BeautifulAsmParser::ProgramContext *ctx);
      virtual std::any visitFunction_definition(BeautifulAsmParser::Function_definitionContext *ctx);
      virtual std::any visitType_definition(BeautifulAsmParser::Type_definitionContext *ctx);
      virtual std::any visitInstruction(BeautifulAsmParser::InstructionContext *ctx);
      virtual std::any visitInstructions(BeautifulAsmParser::InstructionsContext *ctx);
      virtual std::any visitNo_arg_instruction(BeautifulAsmParser::No_arg_instructionContext *ctx);
      virtual std::any visitArrow_instruction(BeautifulAsmParser::Arrow_instructionContext *ctx);
      virtual std::any visitArrow_lhs(BeautifulAsmParser::Arrow_lhsContext *ctx);
      virtual std::any visitArrow_rhs(BeautifulAsmParser::Arrow_rhsContext *ctx);
      virtual std::any visitMake_constructor(BeautifulAsmParser::Make_constructorContext *ctx);
      virtual std::any visitCall_instruction(BeautifulAsmParser::Call_instructionContext *ctx);
      virtual std::any visitPrint_instruction(BeautifulAsmParser::Print_instructionContext *ctx);
      virtual std::any visitBinary_operator_instruction(BeautifulAsmParser::Binary_operator_instructionContext *ctx);
      virtual std::any visitMemory_instruction(BeautifulAsmParser::Memory_instructionContext *ctx);
      virtual std::any visitBinary_operator(BeautifulAsmParser::Binary_operatorContext *ctx);
      virtual std::any visitMemory_operator(BeautifulAsmParser::Memory_operatorContext *ctx);
      virtual std::any visitMemory_destination(BeautifulAsmParser::Memory_destinationContext *ctx);
      virtual std::any visitAny_argument(BeautifulAsmParser::Any_argumentContext *ctx);
      virtual std::any visitAny_lvalue(BeautifulAsmParser::Any_lvalueContext *ctx);
      virtual std::any visitAny_rvalue(BeautifulAsmParser::Any_rvalueContext *ctx);
      virtual std::any visitAny_number(BeautifulAsmParser::Any_numberContext *ctx);
      virtual std::any visitAny_field(BeautifulAsmParser::Any_fieldContext *ctx);
      virtual std::any visitObject_type(BeautifulAsmParser::Object_typeContext *ctx);
  };
}