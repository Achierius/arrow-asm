#include "bytecode_emitter.hh"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <array>
#include <optional>
#include <unordered_map>

#include "src/ast/ast.hh"
#include "src/bytecode/opcodes.hh"
#include "src/bytecode/bytecode.hh"

struct FuncInstContext {
  /// @brief Holds T/F for each p register.
  /// If true, is not a valid argument for any instruction.
  std::array<bool, 16> p_usage_map;
  /// @brief The types of the or registers for this function
  std::array<std::shared_ptr<ast::RegisterTypeNode>, 16> or_types;
  /// @brief The value that is currently on the top of the stack, or nullopt for unknown/nothing
  std::optional<ast::ArgNode> top;
  /// @brief Map from immediate to constant index in chunk.
  std::unordered_map<int64_t, int> constants;

  bool same_as_top(ast::ArgNode const& arg) const {
    if (top) {
      auto const& topref = *top;
      if (std::holds_alternative<ast::ImmediateNode>(topref)
          && std::holds_alternative<ast::ImmediateNode>(arg)) {
        auto const& top_imm = std::get<ast::ImmediateNode>(topref);
        auto const& arg_imm = std::get<ast::ImmediateNode>(arg);
        if (std::holds_alternative<int64_t>(top_imm)
            && std::holds_alternative<int64_t>(arg_imm)) {
          return std::get<int64_t>(top_imm) == std::get<int64_t>(arg_imm);
        } else if (std::holds_alternative<double>(top_imm)
                   && std::holds_alternative<double>(arg_imm)) {
          return std::get<double>(top_imm) == std::get<double>(arg_imm);
        }
        return false;
      } else if (std::holds_alternative<ast::RValueNode>(topref)
                 && std::holds_alternative<ast::RValueNode>(arg)) {
        auto const& top_r = std::get<ast::RValueNode>(topref);
        auto const& arg_r = std::get<ast::RValueNode>(arg);
        return top_r.category == arg_r.category && top_r.register_id == arg_r.register_id;
      }
    }
    return false;
  }
};

void Emit(bytecode::BytecodeChunk& chunk, bytecode::Instruction const& inst) {
  chunk.code.push_back(inst);
}

int8_t TranslateRegister(ast::RegisterNode const& node) {
  switch (node.category) {
    // TODO: Assert that the register_id is valid!
    case ast::RegisterCategory::Return:
    case ast::RegisterCategory::Param:
      return 48 + node.register_id;
    case ast::RegisterCategory::Local:
      return 32 + node.register_id;
    case ast::RegisterCategory::OutgoingParam:
      return 0 + node.register_id;
    case ast::RegisterCategory::Static:
    default:
      return node.register_id;
  }
}

void Emit(bytecode::BytecodeChunk& chunk, FuncInstContext& ctx, ast::ArgNode const& arg) {
  // ASSUME: We should have already checked our arg type to ensure it works with our instruction
  if (ctx.same_as_top(arg)) {
    // We can emit a dup!
    Emit(chunk, bytecode::Instruction{
      .opcode = bytecode::Opcode::kDup,
      .param = 0
    });
  } else {
    // If we are an imm, emit that value
    if (std::holds_alternative<ast::ImmediateNode>(arg)) {
      auto const& imm = std::get<ast::ImmediateNode>(arg);
      if (std::holds_alternative<int64_t>(imm)) {
        auto val = std::get<int64_t>(imm);
        // Check to see if this is a constant, if it is, grab it
        auto itr = ctx.constants.find(val);
        int constant_idx;
        if (itr != ctx.constants.end()) {
          // Found the constant, use the index
          constant_idx = itr->second;
        } else {
          constant_idx = chunk.constants.size();
          chunk.constants.push_back(val);
          ctx.constants.insert({val, constant_idx});
        }
        // Need to add it as a constant
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kConstant,
          .param = static_cast<int8_t>(constant_idx)
        });
      } else {
        // TODO: ERROR, don't support floating point imms?
      }
    } else if (std::holds_alternative<ast::RValueNode>(arg)) {
      // otherwise, emit that translated register
      // TODO: Need to disallow using a pointer register (sometimes)
      
      Emit(chunk, bytecode::Instruction{
        .opcode = bytecode::Opcode::kLoadAuxiliary,
        .param = TranslateRegister(std::get<ast::RValueNode>(arg))
      });
    }
  }
}

// TODO: Return bool?
void HandleInstruction(bytecode::BytecodeChunk& chunk, FuncInstContext& ctx, std::shared_ptr<ast::InstructionNode> const& inst) {
  auto& instr = *inst.get();
  // ArrowInstNode, NoArgNode, NoRetNode, BinaryNode, MemoryNode, IfNode
  if (std::holds_alternative<ast::NoArgNode>(instr)) {
    auto& node = std::get<ast::NoArgNode>(instr);
    switch (node.op) {
      case ast::NoArgOperator::kTrap: {
        // Easy.
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kTrap,
          .param = 0
        });
        break;
      }
      case ast::NoArgOperator::kBreak:
      case ast::NoArgOperator::kContinue:

      case ast::NoArgOperator::kRet: {
        // To return, we need to have (on the top of our stack)
        // all of our return values. They would come from all of our prior p and rr writes
        // Alternative, we push all of our return writes to the VERY bottom of our stack
        // and then we pop all of our parameters here before we ret
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kReturn,
          .param = 0
        });
        break;
      }
    }
  } else if (std::holds_alternative<ast::NoRetNode>(instr)) {
    auto& node = std::get<ast::NoRetNode>(instr);
    switch (node.op) {
      case ast::NoRetOperator::kPrint: {
        // We are to print. or0 is our parameter
        // We should ? already have or0 on the top of our stack
        // So just perform a print (ideally we use or0's type)
        auto& param_type = *ctx.or_types[0].get();
        if (std::holds_alternative<ast::LongNode>(param_type)) {
          Emit(chunk, bytecode::Instruction{
            .opcode = bytecode::Opcode::kPrintLong,
            .param = 0
          });
        } else if (std::holds_alternative<ast::DoubleNode>(param_type)) {
          // TODO: ERROR
          // Print a double
        } else if (std::holds_alternative<ast::PtrNode>(param_type)) {
          // TODO: ERROR
          // Print a pointer and move it back
        }
      }
      default:
        // TODO: ERROR
        break;
    }
  } else if (std::holds_alternative<ast::BinaryNode>(instr)) {
    auto& node = std::get<ast::BinaryNode>(instr);
    switch (node.op) {
      case ast::BinaryOperator::kAdd: {
        // TODO: Actually USE the emits to write an add.
        break;
      }
    }
  }
}

bytecode::BytecodeExecutable ast::LowerAst(const ProgramNode& ast) {
  // TODO: Need to perform a pass over the statements first to get a function list
  // so that we can resolve calls (and also dependent types?)

  bytecode::BytecodeExecutable exe{};
  int loc = 0;
  for (auto& stmt : ast.statements) {
    if (std::holds_alternative<ast::FunctionNode>(*stmt.get())) {
      auto func = std::get<ast::FunctionNode>(*stmt.get());
      // Functions are 1-to-1 of blocks
      bytecode::BytecodeChunk chunk{};
      auto symbol = func.id;
      FuncInstContext ctx;
      for (auto& inst : func.body) {
        HandleInstruction(chunk, ctx, inst);
      }
      exe.chunks.push_back(chunk);
      exe.chunk_locations.push_back(loc++);
    }
  }
  return exe;
}
