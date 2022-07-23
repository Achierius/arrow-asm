#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "opcodes.hh"

namespace bytecode {

using Value = int64_t;
using Address = uint64_t; // TODO wrap this in a struct to prevent conversion
using Instruction = struct {
  Opcode opcode;
  int8_t param;
};

// Single function w/ associated constants. Kinda like an elf section but it has
// both a .text and a .data component.
struct BytecodeChunk {
  std::vector<Instruction> code;
  std::vector<Value> constant_pool; // necessary bc we want fixed-length instrs
  std::vector<std::string> symbol_pool;
  // TODO will this need symbols, or can those just be held in the
  // BytecodeExecutable (and thus gathered there directly from the file)?
};

// TODO this will probably not be enough to actually support full separate
// compilation, we'll need some information about what type of symbol each of
// these are at the very least -- but for a first draft the linker could just
// trust that the symbol is the right type and plug the address in??
// Monostate -> undefined symbol
using Symbol = std::variant<std::monostate, Address>;

// TODO this should be rebundled as a class w/ invariants
struct BytecodeExecutable {
  // Addr start_pc; // TODO do we even need to support diff start addresses?
  std::vector<std::pair<BytecodeChunk, Address>> chunks;
  std::vector<Value> globals;

  // My broad idea is that a '.o-equivalent' BytecodeExecutable would have a
  // partial symbol table, viz. one where some entries are declared (i.e. exist
  // in the vector) but not defined (i.e. == std::monostate). To link them
  // together into a useable executable, the chunks from each would have to be
  // relocated into one another and the symbol tables merged -- or perhaps have
  // symbol tables which embed 'external' chunks, since we don't actually need
  // to have everything pinned w/in an address space (?)
  std::unordered_map<std::string, Symbol> symbol_table;
};

} // namespace bytecode
