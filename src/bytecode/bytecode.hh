#pragma once

#include <cstddef>
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>

#include "opcodes.hh"

namespace bytecode {

struct ClassId { int idx; };
struct ChunkId { int idx; };
using Value = int64_t;
using Address = uint64_t; // TODO wrap this in a struct to prevent conversion
struct Instruction {
  Opcode opcode;
  int8_t param = 0;
};

// Single function w/ associated constants. Kinda like an elf section but it has
// both a .text and a .data component.
struct BytecodeChunk {
  std::vector<Instruction> code;
  std::vector<Value> constants; // necessary bc we want fixed-length instrs
  std::vector<std::string> symbol_pool;
  // TODO will this need symbols, or can those just be held in the
  // BytecodeExecutable (and thus gathered there directly from the file)?
};

// TODO this should be rebundled as a class w/ invariants
struct BytecodeExecutable {
  // Addr start_pc; // TODO do we even need to support diff start addresses?
  std::vector<BytecodeChunk> chunks;
  std::vector<Address> chunk_locations;
  std::vector<Value> globals;

  // My broad idea is that a '.o-equivalent' BytecodeExecutable would have a
  // partial symbol table, viz. one where some entries are declared (i.e. exist
  // in the vector) but not defined (i.e. == std::monostate). To link them
  // together into a useable executable, the chunks from each would have to be
  // relocated into one another and the symbol tables merged -- or perhaps have
  // symbol tables which embed 'external' chunks, since we don't actually need
  // to have everything pinned w/in an address space (?)
  int index;
  using Symbol = std::variant<std::monostate, Address, ClassId, ChunkId>;
  std::unordered_map<std::string, Symbol> symbol_table;

  enum ValueType {
    kVoid,
    kLong,
    kDouble,
    kCodePtr,
    kDataPtr,
    kObject,
  };
  struct ClassDataRecord {
    std::string name; // may not actually be necessary
    std::vector<ValueType> fields;
    ChunkId ctor_chunk;
    ChunkId dtor_chunk;
  };
  std::vector<ClassDataRecord> classes;
};

} // namespace bytecode
