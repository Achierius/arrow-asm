#include "bytecode_emitter.hh"

#include <cassert>
#include <cstddef>
#include <utility>
#include <variant>
#include <array>
#include <optional>
#include <unordered_map>
#include <list>
#include <vector>

#include "src/ast/ast.hh"
#include "src/bytecode/opcodes.hh"
#include "src/bytecode/bytecode.hh"
#include "spdlog/spdlog.h"

struct FuncInstContext {
  /// @brief Holds T/F for each p register.
  /// If true, is not a valid argument for any instruction.
  std::array<bool, 16> p_usage_map;
  /// @brief The types of the or registers for this function
  std::array<std::optional<ast::RegisterTypeNode>, 64> reg_types;
  /// @brief The rvalue that is currently on the top of the stack, or nullopt for unknown/nothing/not an rvalue
  std::list<ast::ArgNode> top_list;
  /// @brief Map from immediate to constant index in chunk.
  std::unordered_map<int64_t, int> constants;

  void assign_top(ast::ArgNode const& arg) {
    top_list.push_front(arg);
  }
  void pop_top() {
    top_list.pop_front();
  }

  bool same_as_top(ast::ArgNode const& arg) const {
    if (!top_list.empty()) {
      auto const& topref = top_list.front();
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

/// @brief Returns true if the two ArgNodes are equivalent types.
/// If they are both immediates, they have the same type,
/// or if they are both registers, they hold the same register type.
bool equal_type(FuncInstContext const& ctx, ast::ArgNode const& arg1, ast::ArgNode const& arg2) {
  if (std::holds_alternative<ast::ImmediateNode>(arg1)
      && std::holds_alternative<ast::ImmediateNode>(arg2)) {
    auto const& top_imm = std::get<ast::ImmediateNode>(arg1);
    auto const& arg_imm = std::get<ast::ImmediateNode>(arg2);
    if (std::holds_alternative<int64_t>(top_imm)
        && std::holds_alternative<int64_t>(arg_imm)) {
      return true;
    } else if (std::holds_alternative<double>(top_imm)
                && std::holds_alternative<double>(arg_imm)) {
      return true;
    }
    return false;
  } else if (std::holds_alternative<ast::RValueNode>(arg1)
              && std::holds_alternative<ast::RValueNode>(arg2)) {
    auto const& arg1_r = std::get<ast::RValueNode>(arg1);
    auto const& arg2_r = std::get<ast::RValueNode>(arg2);
    // Register types can't be optionals at this point
    // We must have SOME knowledge of them (since these are RValues).
    // Unless we don't, in which case they are invalid registers.
    auto const& t1 = ctx.reg_types[TranslateRegister(arg1_r)];
    auto const& t2 = ctx.reg_types[TranslateRegister(arg2_r)];
    if (!t1 || !t2) {
      // If we don't know either type, we simply assume we are of correct type
      // and let our runtime figure it out.
      return true;
    }
    return t1.value() == t2.value();
  }
  return false;
}

void EmitArg(bytecode::BytecodeChunk& chunk, FuncInstContext& ctx, ast::ArgNode const& arg) {
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
  // Still add it to top even if it's a duplicate, makes cleanup easier.
  ctx.assign_top(arg);
}

// TODO: Return bool?
void HandleInstruction(bytecode::BytecodeChunk& chunk, FuncInstContext& ctx, ast::InstructionNode const& inst) {
  auto const& instr = inst;
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
        auto const& arg = node.arg;
        EmitArg(chunk, ctx, arg);
        // TODO: We should check our type here to see what we need to emit
        // Pop our argument off the top
        ctx.pop_top();
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kPrintLong,
          .param = 0
        });
      }
      default:
        // TODO: ERROR
        break;
    }
  } else if (std::holds_alternative<ast::BinaryNode>(instr)) {
    auto const& node = std::get<ast::BinaryNode>(instr);
    auto const& arg1 = node.arg1;
    auto const& arg2 = node.arg2;
    // We want to make sure both of our arguments are of an equivalent type
    if (!equal_type(ctx, arg1, arg2)) {
      // TODO: ERROR (Do not have equivalent type for arguments!)
    }
    EmitArg(chunk, ctx, arg1);
    EmitArg(chunk, ctx, arg2);
    // TODO: Not always Add/Mul LONG
    switch (node.op) {
      case ast::BinaryOperator::kAdd: {
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kAddLong,
          .param = 0
        });
        break;
      }
      case ast::BinaryOperator::kMul: {
        Emit(chunk, bytecode::Instruction{
          .opcode = bytecode::Opcode::kMulLong,
          .param = 0
        });
        break;
      }
      default:
        // TODO: ERROR (Unsupported binary inst)
        break;
    }
    // Pop both arguments from our 'top' stack
    ctx.pop_top();
    ctx.pop_top();
    // Emit the store
    auto dst = TranslateRegister(node.lhs);
    Emit(chunk, bytecode::Instruction{
      .opcode = bytecode::Opcode::kStoreAuxiliary,
      .param = dst
    });
    // TODO: Properly handle type assignment
    ctx.reg_types[dst] = ast::LongNode();
  } else {
    // TODO: ERROR (Unsupported instruction)
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
      FuncInstContext ctx{};
      for (int i = 0; i < func.params.size(); ++i) {
        ctx.reg_types[TranslateRegister(ast::RegisterNode{
          .category = ast::RegisterCategory::Param,
          .register_id = static_cast<int8_t>(i)
        })] = *func.params[i];
      }
      for (auto& inst : func.body) {
        HandleInstruction(chunk, ctx, *inst);
      }
      exe.chunks.push_back(chunk);
      exe.symbol_table[symbol.id] = bytecode::ChunkId{loc};
      exe.chunk_locations.push_back(loc++);
    }
  }
  return exe;
}
